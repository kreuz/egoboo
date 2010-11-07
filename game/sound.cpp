//********************************************************************************************
//*
//*    This file is part of Egoboo.
//*
//*    Egoboo is free software: you can redistribute it and/or modify it
//*    under the terms of the GNU General Public License as published by
//*    the Free Software Foundation, either version 3 of the License, or
//*    (at your option) any later version.
//*
//*    Egoboo is distributed in the hope that it will be useful, but
//*    WITHOUT ANY WARRANTY; without even the implied warranty of
//*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//*    General Public License for more details.
//*
//*    You should have received a copy of the GNU General Public License
//*    along with Egoboo.  If not, see <http://www.gnu.org/licenses/>.
//*
//********************************************************************************************

/// \file sound.c
/// \brief Sound code in Egoboo is implemented using SDL_mixer.
/// \details

#include "sound.h"
#include "camera.h"
#include "log.h"
#include "game.h"
#include "char.inl"

#include "file_formats/pip_file.h"

#include "egoboo_vfs.h"
#include "egoboo_setup.h"
#include "egoboo_fileutil.h"
#include "egoboo_setup.h"
#include "egoboo_strutil.h"

#include "egoboo.h"

#include <SDL.h>

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#define LOOPED_COUNT 256

/// Data needed to store and manipulate a looped sound
struct snd_looped_sound_data
{
    int         channel;
    Mix_Chunk * chunk;
    CHR_REF     object;
};

static t_list< snd_looped_sound_data, LOOPED_COUNT  > LoopedList;

static void   LoopedList_init();
static void   LoopedList_clear();
static bool_t LoopedList_free_one( size_t index );
static size_t LoopedList_get_free();

static bool_t LoopedList_validate();
static size_t LoopedList_add( Mix_Chunk * sound, int loops, const CHR_REF &  object );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

// Sound using SDL_Mixer
static bool_t mixeron         = bfalse;

ego_snd_config snd;

// music
bool_t      musicinmemory = bfalse;
Mix_Music * musictracksloaded[MAXPLAYLISTLENGTH];
Sint8       songplaying   = INVALID_SOUND;

Mix_Chunk * g_wavelist[GSND_COUNT];

// text filenames for the global sounds
static const char * wavenames[GSND_COUNT] =
{
    "coinget",
    "defend",
    "weather1",
    "weather2",
    "coinfall",
    "lvlup",
    "pitfall",
    "shieldblock"
};

static bool_t sound_atexit_registered = bfalse;

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
static bool_t sdl_audio_initialize();
static bool_t sdl_mixer_initialize();
static void   sdl_mixer_quit( void );

int    _calculate_volume( fvec3_t   diff );
bool_t _update_stereo_channel( int channel, fvec3_t   diff );
bool_t _update_channel_volume( int channel, int volume, fvec3_t   diff );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

snd_config_data_t * snd_get_config()
{
    return static_cast<snd_config_data_t*>( &snd );
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// define a little stack for interrupting music sounds with other music

#define MUSIC_STACK_COUNT 20
static int music_stack_depth = 0;

/// The data needed to store a single music track on the music_stack[]
struct ego_music_stack_element
{
    Mix_Music * mus;
    int         number;
};

static ego_music_stack_element music_stack[MUSIC_STACK_COUNT];

static bool_t music_stack_pop( Mix_Music ** mus, int * song );

//--------------------------------------------------------------------------------------------
static void music_stack_finished_callback( void )
{
    // this function is only called when a music function is finished playing
    // pop the saved music off the stack

    // unfortunately, it seems that SDL_mixer does not support saving the position of
    // the music stream, so the music track will restart from the beginning

    Mix_Music * mus;
    int         song;

    // grab the next song
    if ( music_stack_pop( &mus, &song ) )
    {
        // play the music
        Mix_PlayMusic( mus, 0 );

        songplaying = song;

        // set the volume
        Mix_VolumeMusic( snd.musicvolume );
    }
}

//--------------------------------------------------------------------------------------------
bool_t music_stack_push( Mix_Music * mus, int song )
{
    if ( music_stack_depth >= MUSIC_STACK_COUNT - 1 )
    {
        music_stack_depth = MUSIC_STACK_COUNT - 1;
        return bfalse;
    }

    music_stack[music_stack_depth].mus    = mus;
    music_stack[music_stack_depth].number = song;

    music_stack_depth++;

    return btrue;
}

//--------------------------------------------------------------------------------------------
bool_t music_stack_pop( Mix_Music ** mus, int * song )
{
    if ( NULL == mus || NULL == song ) return bfalse;

    // fail if music isn't loaded
    if ( !musicinmemory )
    {
        *mus = NULL;
        *song = INVALID_SOUND;
        return bfalse;
    }

    // set the default to be song 0
    *song = 0;
    *mus  = musictracksloaded[*song];

    // pop the stack, if possible
    if ( music_stack_depth > 0 )
    {
        music_stack_depth--;

        *mus  = music_stack[music_stack_depth].mus;
        *song = music_stack[music_stack_depth].number;
    }

    return btrue;
}

//--------------------------------------------------------------------------------------------
void music_stack_init()
{
    // push on the default music value
    music_stack_push( musictracksloaded[0], 0 );

    // register the callback
    Mix_HookMusicFinished( music_stack_finished_callback );
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
bool_t sdl_audio_initialize()
{
    bool_t retval = btrue;

    // make sure that SDL audio is turned on
    if ( 0 == SDL_WasInit( SDL_INIT_AUDIO ) )
    {
        log_info( "Intializing SDL Audio... " );
        if ( SDL_InitSubSystem( SDL_INIT_AUDIO ) < 0 )
        {
            log_message( "Failed!\n" );
            log_warning( "SDL error == \"%s\"\n", SDL_GetError() );

            retval = bfalse;
            snd.musicvalid = bfalse;
            snd.soundvalid = bfalse;
        }
        else
        {
            retval = btrue;
            log_message( "Success!\n" );
        }
    }

    return retval;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
bool_t sdl_mixer_initialize()
{
    /// \author ZF
    /// \details  This initializes the SDL_mixer services

    if ( !mixeron && ( snd.musicvalid || snd.soundvalid ) )
    {
        const SDL_version* link_version = Mix_Linked_Version();
        log_info( "Initializing SDL_mixer audio services version %d.%d.%d... ", link_version->major, link_version->minor, link_version->patch );
        if ( Mix_OpenAudio( cfg.sound_highquality_base ? MIX_HIGH_QUALITY : MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, snd.buffersize ) < 0 )
        {
            mixeron = bfalse;
            log_message( "Failure!\n" );
            log_warning( "Unable to initialize audio: %s\n", Mix_GetError() );
        }
        else
        {
            Mix_VolumeMusic( snd.musicvolume );
            Mix_AllocateChannels( snd.maxsoundchannel );

            // initialize the music stack
            music_stack_init();

            mixeron = btrue;

            atexit( sdl_mixer_quit );
            sound_atexit_registered = btrue;

            log_message( "Success!\n" );
        }
    }

    return mixeron;
}

//--------------------------------------------------------------------------------------------
void sdl_mixer_quit( void )
{
    if ( mixeron && ( 0 != SDL_WasInit( SDL_INIT_AUDIO ) ) )
    {
        Mix_CloseAudio();
        mixeron = bfalse;
    }
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// This function enables the use of SDL_Audio and SDL_Mixer functions, returns btrue if success
bool_t sound_initialize()
{
    bool_t retval = bfalse;
    if ( sdl_audio_initialize() )
    {
        retval = sdl_mixer_initialize();
    }

    if ( retval )
    {
        LoopedList_init();
    }

    return retval;
}

//--------------------------------------------------------------------------------------------
Mix_Chunk * sound_load_chunk_vfs( const char * szFileName )
{
    STRING      full_file_name;
    Mix_Chunk * tmp_chunk;
    bool_t      file_exists = bfalse;

    if ( !mixeron ) return NULL;
    if ( INVALID_CSTR( szFileName ) ) return NULL;

    // blank out the data
    tmp_chunk = NULL;

    // try an ogg file
    SDL_snprintf( full_file_name, SDL_arraysize( full_file_name ), "%s.%s", szFileName, "ogg" );
    if ( vfs_exists( full_file_name ) )
    {
        file_exists = btrue;
        tmp_chunk = Mix_LoadWAV( vfs_resolveReadFilename( full_file_name ) );
    }

    if ( NULL == tmp_chunk )
    {
        // try a wav file
        SDL_snprintf( full_file_name, SDL_arraysize( full_file_name ), "%s.%s", szFileName, "wav" );
        if ( vfs_exists( full_file_name ) )
        {
            file_exists = btrue;
            tmp_chunk = Mix_LoadWAV( vfs_resolveReadFilename( full_file_name ) );
        }
    }

    if ( file_exists && NULL == tmp_chunk )
    {
        // there is an error only if the file exists and can't be loaded
        log_warning( "Sound file not found/loaded %s.\n", szFileName );
    }

    return tmp_chunk;
}

//--------------------------------------------------------------------------------------------
Mix_Music * sound_load_music( const char * szFileName )
{
    STRING      full_file_name;
    Mix_Music * tmp_music;
    bool_t      file_exists = bfalse;

    if ( !mixeron ) return NULL;
    if ( INVALID_CSTR( szFileName ) ) return NULL;

    // blank out the data
    tmp_music = NULL;

    // try a wav file
    SDL_snprintf( full_file_name, SDL_arraysize( full_file_name ), "%s.%s", szFileName, "wav" );
    if ( vfs_exists( full_file_name ) )
    {
        file_exists = btrue;
        tmp_music = Mix_LoadMUS( full_file_name );
    }

    if ( NULL == tmp_music )
    {
        // try an ogg file
        tmp_music = NULL;
        SDL_snprintf( full_file_name, SDL_arraysize( full_file_name ), "%s.%s", szFileName, "ogg" );
        if ( vfs_exists( full_file_name ) )
        {
            file_exists = btrue;
            tmp_music = Mix_LoadMUS( full_file_name );
        }
    }

    if ( file_exists && NULL == tmp_music )
    {
        // there is an error only if the file exists and can't be loaded
        log_warning( "Music file not found/loaded %s.\n", szFileName );
    }

    return tmp_music;
}

//--------------------------------------------------------------------------------------------
bool_t sound_load( snd_mix_ptr * ptr, const char * szFileName, snd_mix_type type )
{

    if ( !mixeron ) return bfalse;
    if (( NULL == ptr ) ) return bfalse;

    // clear out the data
    ptr->ptr.unk = NULL;
    ptr->type    = MIX_UNKNOWN;

    if ( INVALID_CSTR( szFileName ) ) return bfalse;

    switch ( type )
    {
        case MIX_MUS:
            ptr->ptr.mus = sound_load_music( szFileName );
            if ( NULL != ptr->ptr.mus )
            {
                ptr->type = MIX_MUS;
            }
            break;

        case MIX_SND:
            ptr->ptr.snd = sound_load_chunk_vfs( szFileName );
            if ( NULL != ptr->ptr.snd )
            {
                ptr->type = MIX_SND;
            }
            break;

        case MIX_UNKNOWN:
            /* do nothing */
            break;

        default:
            // there is an error only if the file exists and can't be loaded
            log_debug( "%s - Mix type recognized %d.\n", __FUNCTION__, type );
            break;
    };

    return MIX_UNKNOWN != ptr->type;
}

//--------------------------------------------------------------------------------------------
int sound_play_mix( fvec3_t   pos, snd_mix_ptr * ptr )
{
    int retval = INVALID_SOUND_CHANNEL;
    if ( !snd.soundvalid || !mixeron )
    {
        return INVALID_SOUND_CHANNEL;
    }
    if (( NULL == ptr ) || MIX_UNKNOWN == ptr->type || ( NULL == ptr->ptr.unk ) )
    {
        log_debug( "Unable to load sound. (%s)\n", Mix_GetError() );
        return INVALID_SOUND_CHANNEL;
    }

    retval = INVALID_SOUND_CHANNEL;
    if ( MIX_SND == ptr->type )
    {
        retval = sound_play_chunk( pos, ptr->ptr.snd );
    }
    else if ( MIX_MUS == ptr->type )
    {
        // !!!!this will override the music!!!!

        // add the old stream to the stack
        music_stack_push( musictracksloaded[songplaying], songplaying );

        // push on a new stream, play only once
        retval = Mix_PlayMusic( ptr->ptr.mus, 1 );

        // invalidate the song
        songplaying = INVALID_SOUND;

        // since music_stack_finished_callback() is registered using Mix_HookMusicFinished(),
        // it will resume when ptr->ptr.mus is finished playing
    }

    return retval;
}

//--------------------------------------------------------------------------------------------
void sound_restart()
{
    if ( mixeron )
    {
        Mix_CloseAudio();
        mixeron = bfalse;
    }

    // loose the info on the currently playing song
    if ( snd.musicvalid || snd.soundvalid )
    {
        if ( -1 != Mix_OpenAudio( cfg.sound_highquality_base ? MIX_HIGH_QUALITY : MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, snd.buffersize ) )
        {
            mixeron = btrue;
            Mix_AllocateChannels( snd.maxsoundchannel );
            Mix_VolumeMusic( snd.musicvolume );

            // initialize the music stack
            music_stack_init();

            if ( !sound_atexit_registered )
            {
                atexit( sdl_mixer_quit );
                sound_atexit_registered = btrue;
            }
        }
        else
        {
            log_warning( "%s - Cannot get the sound module to restart. (%s)\n", __FUNCTION__, Mix_GetError() );
        }
    }
}

//------------------------------------
// Mix_Chunk stuff -------------------
//------------------------------------

int _calculate_volume( fvec3_t   diff )
{
    /// \author BB
    /// \details  This calculates the volume a sound should have depending on
    //  the distance from the camera

    float dist2;
    int volume;
    float render_size;

    // approximate the radius of the area that the camera sees
    render_size = renderlist.all_count * ( GRID_SIZE / 2 * GRID_SIZE / 2 ) / 4;

    dist2 = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

    // adjust for the local_listening skill
    if ( local_stats.listen_level > 0 ) dist2 *= 0.66f * 0.66f;

    volume  = 255 * render_size / ( render_size + dist2 );
    volume  = ( volume * snd.soundvolume ) / 100;

    return volume;
}

bool_t _update_channel_volume( int channel, int volume, fvec3_t   diff )
{
    float pan;
    float cosval;
    int leftvol, rightvol;

    // determine the angle away from "forward"
    pan = ATAN2( diff.y, diff.x ) - PCamera->turn_z_rad;
    volume *= ( 2.0f + COS( pan ) ) / 3.0f;

    // determine the angle from the left ear
    pan += 1.5f * PI;

    // determine the panning
    cosval = COS( pan );
    cosval *= cosval;

    leftvol  = cosval * 128;
    rightvol = 128 - leftvol;

    leftvol  = (( 127 + leftvol ) * volume ) >> 8;
    rightvol = (( 127 + rightvol ) * volume ) >> 8;

    // apply the volume adjustments
    Mix_SetPanning( channel, leftvol, rightvol );

    return btrue;
}

//--------------------------------------------------------------------------------------------
int sound_play_chunk_looped( fvec3_t pos, Mix_Chunk * pchunk, int loops, const CHR_REF & owner )
{
    /// \author ZF
    /// \details This function plays a specified sound and returns which channel its using

    int channel = INVALID_SOUND_CHANNEL;
    fvec3_t   diff;
    int volume;

    if ( !snd.soundvalid || !mixeron || NULL == pchunk ) return INVALID_SOUND_CHANNEL;

    // only play sound effects if the game is running
    if ( rv_success != ( rv_success == GProc->running() ) )  return INVALID_SOUND_CHANNEL;

    // measure the distance in tiles
    diff = fvec3_sub( pos.v, PCamera->track_pos.v );
    volume = _calculate_volume( diff );

    // play the sound
    if ( volume > 0 )
    {
        // play the sound
        channel = Mix_PlayChannel( -1, pchunk, loops );

        if ( INVALID_SOUND_CHANNEL == channel )
        {
            /// \note ZF@> disabled this warning because this happens really often
            //log_debug( "Unable to play sound. (%s)\n", Mix_GetError() );
        }
        else
        {
            if ( 0 != loops )
            {
                // add the sound to the LoopedList
                LoopedList_add( pchunk, channel, owner );
            }

            // Set left/right panning
            _update_channel_volume( channel, volume, diff );
        }
    }

    return channel;
}

//--------------------------------------------------------------------------------------------
int sound_play_chunk_full( Mix_Chunk * pchunk )
{
    /// \author ZF
    /// \details This function plays a specified sound at full possible volume and returns which channel its using

    int channel = INVALID_SOUND_CHANNEL;

    if ( !snd.soundvalid || !mixeron || NULL == pchunk ) return INVALID_SOUND_CHANNEL;

    // only play sound effects if the game is running
    if ( rv_success != ( rv_success == GProc->running() ) )  return INVALID_SOUND_CHANNEL;

    // play the sound
    channel = Mix_PlayChannel( -1, pchunk, 0 );

    // we are still limited by the global sound volume
    Mix_Volume( channel, ( 128*snd.soundvolume ) / 100 );

    return channel;
}

//--------------------------------------------------------------------------------------------
void sound_stop_channel( int whichchannel )
{
    /// \author ZF
    /// \details  Stops a sound effect playing in the specified channel
    if ( mixeron && snd.soundvalid )
    {
        Mix_HaltChannel( whichchannel );
    }
}

//------------------------------------
// Mix_Music stuff -------------------
//------------------------------------
void sound_play_song( int songnumber, Uint16 fadetime, int loops )
{
    /// \author ZF
    /// \details  This functions plays a specified track loaded into memory
    if ( !snd.musicvalid || !mixeron ) return;

    if ( songplaying != songnumber )
    {
        // Mix_FadeOutMusic(fadetime);      // Stops the game too

        if ( loops != 0 )
        {
            if ( INVALID_SOUND != songplaying )
            {
                music_stack_push( musictracksloaded[songplaying], songplaying );
            }
        }

        Mix_FadeInMusic( musictracksloaded[songnumber], loops, fadetime );

        songplaying = songnumber;
    }
}

//--------------------------------------------------------------------------------------------
void sound_finish_song( Uint16 fadetime )
{
    Mix_Music * mus;
    int         song;

    if ( !snd.musicvalid || !mixeron || songplaying == MENU_SONG ) return;

    if ( !musicinmemory )
    {
        Mix_HaltMusic();
        return;
    }

    // set the defaults
    mus  = musictracksloaded[MENU_SONG];
    song = MENU_SONG;

    // try to grab the last song playing
    music_stack_pop( &mus, &song );

    if ( INVALID_SOUND == song )
    {
        // some wierd error
        Mix_HaltMusic();
    }
    else
    {
        if ( INVALID_SOUND != songplaying )
        {
            Mix_FadeOutMusic( fadetime );
        }

        // play the music
        Mix_FadeInMusic( mus, -1, fadetime );

        // set the volume
        Mix_VolumeMusic( snd.musicvolume );

        songplaying = song;
    }
}

//--------------------------------------------------------------------------------------------
void sound_stop_song()
{
    /// \author ZF
    /// \details This function sets music track to pause

    if ( mixeron && snd.musicvalid )
    {
        Mix_HaltMusic();
    }
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void load_global_waves()
{
    /// \author ZZ
    /// \details  This function loads the global waves

    STRING wavename;
    int cnt;

    // Grab these sounds from the basicdat dir
    SDL_snprintf( wavename, SDL_arraysize( wavename ), "mp_data/%s", wavenames[GSND_GETCOIN] );
    g_wavelist[GSND_GETCOIN] = sound_load_chunk_vfs( wavename );

    SDL_snprintf( wavename, SDL_arraysize( wavename ), "mp_data/%s", wavenames[GSND_DEFEND] );
    g_wavelist[GSND_DEFEND] = sound_load_chunk_vfs( wavename );

    SDL_snprintf( wavename, SDL_arraysize( wavename ), "mp_data/%s", wavenames[GSND_COINFALL] );
    g_wavelist[GSND_COINFALL] = sound_load_chunk_vfs( wavename );

    SDL_snprintf( wavename, SDL_arraysize( wavename ), "mp_data/%s", wavenames[GSND_LEVELUP] );
    g_wavelist[GSND_LEVELUP] = sound_load_chunk_vfs( wavename );

    SDL_snprintf( wavename, SDL_arraysize( wavename ), "mp_data/%s", wavenames[GSND_PITFALL] );
    g_wavelist[GSND_PITFALL] = sound_load_chunk_vfs( wavename );

    SDL_snprintf( wavename, SDL_arraysize( wavename ), "mp_data/%s", wavenames[GSND_SHIELDBLOCK] );
    g_wavelist[GSND_SHIELDBLOCK] = sound_load_chunk_vfs( wavename );

    /*
    // These new values todo should determine global sound and particle effects
    Weather Type: DROPS, RAIN, SNOW
    Water Type: LAVA, WATER, DARK
    */

    for ( cnt = 0; cnt < GSND_COUNT; cnt++ )
    {
        Mix_Chunk * ptmp;

        SDL_snprintf( wavename, SDL_arraysize( wavename ), "mp_data/sound%d", cnt );
        ptmp = sound_load_chunk_vfs( wavename );

        // only overwrite with a valid sound file
        if ( NULL != ptmp )
        {
            g_wavelist[cnt] = ptmp;
        }
    }
}

//--------------------------------------------------------------------------------------------
void load_all_music_sounds_vfs()
{
    /// ZF@> This function loads all of the music sounds
    STRING loadpath;
    STRING songname;
    vfs_FILE *playlist;
    Uint8 cnt;

    if ( musicinmemory || !snd.musicvalid ) return;

    // Open the playlist listing all music files
    SDL_snprintf( loadpath, SDL_arraysize( loadpath ), "mp_data/music/playlist.txt" );
    playlist = vfs_openRead( loadpath );
    if ( NULL == playlist )
    {
        log_warning( "Error reading music list. (%s)\n", loadpath );
        return;
    }

    // Load the music data into memory
    for ( cnt = 0; cnt < MAXPLAYLISTLENGTH && !vfs_eof( playlist ); cnt++ )
    {
        if ( goto_colon( NULL, playlist, btrue ) )
        {
            fget_string( playlist, songname, SDL_arraysize( songname ) );

            SDL_snprintf( loadpath, SDL_arraysize( loadpath ), ( "mp_data/music/%s" ), songname );
            musictracksloaded[cnt] = Mix_LoadMUS( vfs_resolveReadFilename( loadpath ) );
        }
    }
    musicinmemory = btrue;

    // A small helper for us developers
    if ( cnt == MAXPLAYLISTLENGTH )
    {
        log_debug( "Play list is full. Consider increasing MAXPLAYLISTLENGTH (currently %i).", MAXPLAYLISTLENGTH );
    }

    vfs_close( playlist );
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

bool_t snd_config_init( snd_config_data_t * psnd )
{
    // Initialize the sound settings and set all values to default
    if ( NULL == psnd ) return bfalse;

    psnd->soundvalid        = bfalse;
    psnd->musicvalid        = bfalse;
    psnd->musicvolume       = 50;                            // The sound volume of music
    psnd->soundvolume       = 75;          // Volume of sounds played
    psnd->maxsoundchannel   = 16;      // Max number of sounds playing at the same time
    psnd->buffersize        = 2048;
    psnd->highquality       = bfalse;

    return btrue;
}

//--------------------------------------------------------------------------------------------
bool_t snd_config_synch( snd_config_data_t * psnd, config_data_t * pcfg )
{
    if ( NULL == psnd && NULL == pcfg ) return bfalse;

    if ( NULL == pcfg )
    {
        return snd_config_init( psnd );
    }

    // coerce pcfg to have valid values
    pcfg->sound_channel_count = CLIP( pcfg->sound_channel_count, 8, 128 );
    pcfg->sound_buffer_size   = CLIP( pcfg->sound_buffer_size, 512, 8196 );

    if ( NULL != psnd )
    {
        psnd->soundvalid      = pcfg->sound_allowed;
        psnd->soundvolume     = pcfg->sound_volume;
        psnd->musicvalid      = pcfg->music_allowed;
        psnd->musicvolume     = pcfg->music_volume;
        psnd->maxsoundchannel = pcfg->sound_channel_count;
        psnd->buffersize      = pcfg->sound_buffer_size;
        psnd->highquality     = pcfg->sound_highquality;
    }

    return btrue;
}

//--------------------------------------------------------------------------------------------
void sound_fade_all()
{
    if ( mixeron )
    {
        Mix_FadeOutChannel( -1, 500 );     // Stop all sounds that are playing
    }
}

//--------------------------------------------------------------------------------------------
void fade_in_music( Mix_Music * music )
{
    if ( mixeron )
    {
        Mix_FadeInMusic( music, -1, 500 );
    }
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void   LoopedList_init()
{
    /// \author BB
    /// \details  setup the looped sound list
    LOOP_REF cnt;
    size_t tnc;

    for ( cnt = 0; cnt < LOOPED_COUNT; cnt++ )
    {
        // clear out all of the data
        SDL_memset( LoopedList.lst + cnt, 0, sizeof( snd_looped_sound_data ) );

        LoopedList.lst[cnt].channel = INVALID_SOUND_CHANNEL;
        LoopedList.lst[cnt].chunk   = NULL;
        LoopedList.lst[cnt].object  = CHR_REF( MAX_CHR );

        tnc = cnt.get_value();
        LoopedList.used_ref[tnc] = LOOPED_COUNT;
        LoopedList.free_ref[tnc] = tnc;
    }

    LoopedList._used_count = 0;
    LoopedList._free_count = LOOPED_COUNT;
}

//--------------------------------------------------------------------------------------------
bool_t LoopedList_validate()
{
    /// \author BB
    /// \details  do the free and used indices have valid values?

    bool_t retval;

    retval = btrue;
    if ( LOOPED_COUNT != LoopedList._free_count + LoopedList._used_count )
    {
        // punt!
        LoopedList_clear();
        retval = bfalse;
    }

    return retval;
}

//--------------------------------------------------------------------------------------------
bool_t LoopedList_free_one( size_t index )
{
    /// \author BB
    /// \details  free a looped sound only if it is actually being used
    size_t   cnt;
    LOOP_REF ref;

    if ( !LoopedList_validate() ) return bfalse;

    // is the index actually free?
    for ( cnt = 0; cnt < LoopedList._used_count; cnt++ )
    {
        if ( index == LoopedList.used_ref[cnt] ) break;
    }

    // was anything found?
    if ( cnt >= LoopedList._used_count ) return bfalse;

    // swap the value with the one on the top of the stack
    SWAP( size_t, LoopedList.used_ref[cnt], LoopedList.used_ref[LoopedList._used_count-1] );

    LoopedList._used_count--;
    LoopedList.update_guid++;

    // push the value onto the free stack
    LoopedList.free_ref[LoopedList._free_count] = index;

    LoopedList._free_count++;
    LoopedList.update_guid++;

    // clear out the data
    ref = LOOP_REF( index );
    LoopedList.lst[ref].channel = INVALID_SOUND_CHANNEL;
    LoopedList.lst[ref].chunk   = NULL;
    LoopedList.lst[ref].object  = CHR_REF( MAX_CHR );

    return btrue;
}

//--------------------------------------------------------------------------------------------
size_t LoopedList_get_free()
{
    size_t index;

    if ( !LoopedList_validate() ) return bfalse;

    LoopedList._free_count--;
    LoopedList.update_guid++;

    index = LoopedList.free_ref[LoopedList._free_count];

    // push the value onto the used stack
    LoopedList.used_ref[LoopedList._used_count] = index;

    LoopedList._used_count++;
    LoopedList.update_guid++;

    return index;
}

//--------------------------------------------------------------------------------------------
void LoopedList_clear()
{
    /// \author BB
    /// \details  shut off all the looped sounds

    LOOP_REF cnt;

    for ( cnt = 0; cnt < LOOPED_COUNT; cnt++ )
    {
        if ( INVALID_SOUND_CHANNEL != LoopedList.lst[cnt].channel )
        {
            Mix_FadeOutChannel( LoopedList.lst[cnt].channel, 500 );

            // clear out the data
            LoopedList.lst[cnt].channel = INVALID_SOUND_CHANNEL;
            LoopedList.lst[cnt].chunk   = NULL;
            LoopedList.lst[cnt].object  = CHR_REF( MAX_CHR );
        }
    }

    LoopedList_init();
}

//--------------------------------------------------------------------------------------------
size_t LoopedList_add( Mix_Chunk * sound, int channel, const CHR_REF &  ichr )
{
    /// \author BB
    /// \details  add a looped sound to the list

    size_t index;

    if ( NULL == sound || INVALID_SOUND_CHANNEL == channel || !INGAME_CHR( ichr ) ) return LOOPED_COUNT;

    if ( LoopedList._used_count >= LOOPED_COUNT ) return LOOPED_COUNT;
    if ( !LoopedList_validate() ) return LOOPED_COUNT;

    index = LoopedList_get_free();
    if ( index != LOOPED_COUNT )
    {
        // set up the LoopedList entry at the empty index
        LOOP_REF ref = LOOP_REF( index );

        LoopedList.lst[ref].chunk   = sound;
        LoopedList.lst[ref].channel = channel;
        LoopedList.lst[ref].object  = ichr;
    }

    return index;
}

//--------------------------------------------------------------------------------------------
bool_t LoopedList_remove( int channel )
{
    /// \author BB
    /// \details  remove a looped sound from the used list

    size_t cnt;
    bool_t retval;

    if ( 0 == LoopedList._used_count ) return bfalse;

    if ( !LoopedList_validate() ) return bfalse;

    retval = bfalse;
    for ( cnt = 0; cnt < LoopedList._used_count; cnt++ )
    {
        size_t index = LoopedList.used_ref[cnt];

        if ( index != LOOPED_COUNT )
        {
            LOOP_REF ref = LOOP_REF( index );

            if ( channel == LoopedList.lst[ref].channel )
            {
                retval = LoopedList_free_one( cnt );
                break;
            }
        }
    }

    return retval;
}

//--------------------------------------------------------------------------------------------
bool_t _update_stereo_channel( int channel, fvec3_t   diff )
{
    /// \author BB
    /// \details  This updates the stereo image of a looped sound

    int       volume;

    if ( INVALID_SOUND_CHANNEL == channel ) return bfalse;

    volume = _calculate_volume( diff );
    return _update_channel_volume( channel, volume, diff );
}

//--------------------------------------------------------------------------------------------
void looped_update_all_sound()
{
    size_t cnt;

    for ( cnt = 0; cnt < LoopedList._used_count; cnt++ )
    {
        fvec3_t   diff;
        size_t    index;
        LOOP_REF   ref;
        snd_looped_sound_data * plooped;

        index = LoopedList.used_ref[cnt];
        if ( index < 0 || index >= LOOPED_COUNT ) continue;

        ref = LOOP_REF( index );

        if ( INVALID_SOUND_CHANNEL == LoopedList.lst[ref].channel ) continue;
        plooped = LoopedList.lst + ref;

        if ( !INGAME_CHR( plooped->object ) )
        {
            // not a valid object
            fvec3_t   tmp_diff = VECT3( 0, 0, 0 );

            _update_stereo_channel( plooped->channel, tmp_diff );
        }
        else
        {
            // make the sound stick to the object
            diff = fvec3_sub( ChrObjList.get_data_ref( plooped->object ).pos.v, PCamera->track_pos.v );

            _update_stereo_channel( plooped->channel, diff );
        }
    }
}

//--------------------------------------------------------------------------------------------
bool_t looped_stop_object_sounds( const CHR_REF &  ichr )
{
    /// \author BB
    /// \details  free any looped sound(s) being made by a certain character
    int    freed;
    size_t cnt;
    bool_t found;

    if ( !ALLOCATED_CHR( ichr ) ) return bfalse;

    // we have to do this a funny way, because it is hard to guarantee how the
    // "delete"/"free" function LoopedList_free_one() will free an entry, and because
    // each object could have multiple looped sounds

    freed = 0;
    found = btrue;
    while ( found && LoopedList._used_count > 0 )
    {
        found = bfalse;
        for ( cnt = 0; cnt < LoopedList._used_count; cnt++ )
        {
            LOOP_REF ref;

            size_t index = LoopedList.used_ref[cnt];
            if ( index < 0 || index >= LOOPED_COUNT ) continue;

            ref = LOOP_REF( index );

            if ( LoopedList.lst[ref].object == ichr )
            {
                int channel = LoopedList.lst[ref].channel;

                if ( LoopedList_free_one( index ) )
                {
                    freed++;
                    found = btrue;
                    sound_stop_channel( channel );
                    break;
                }
            }
        }
    }

    return freed > 0;
}

//--------------------------------------------------------------------------------------------
void sound_finish_sound()
{
    Mix_FadeOutChannel( -1, 500 );     // Stop all in-game sounds that are playing
    sound_finish_song( 500 );          // Fade out the existing song and pop the music stack
}

//--------------------------------------------------------------------------------------------
void sound_free_chunk( Mix_Chunk * pchunk )
{
    if ( mixeron )
    {
        Mix_FreeChunk( pchunk );
    }
}

//--------------------------------------------------------------------------------------------
int get_current_song_playing()
{
    /// \author ZF
    /// \details  This gives read access to the private variable 'songplaying'
    return songplaying;
}

