#pragma once

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

#include "egoboo_typedef.h"
#include "cap_file.h"

#include "sound.h"
#include "script.h"
#include "md2.h"
#include "graphic.h"

#include "egoboo.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

// Attack directions
#define ATK_FRONT  0x0000
#define ATK_RIGHT  0x4000
#define ATK_BEHIND 0x8000
#define ATK_LEFT   0xC000

#define MAXXP      200000                        // Maximum experience
#define MAXMONEY     9999                        // Maximum money

#define MAX_CAP    MAX_PROFILE

typedef enum e_turn_modes
{
    TURNMODE_VELOCITY = 0,                       // Character gets rotation from velocity (normal)
    TURNMODE_WATCH,                              // For watch towers, look towards waypoint
    TURNMODE_SPIN,                               // For spinning objects
    TURNMODE_WATCHTARGET,                        // For combat intensive AI
    TURNMODE_COUNT
} TURN_MODE;

#define MANARETURNSHIFT     22                      // ChrList.lst[ichr].manareturn/MANARETURNSHIFT = mana regen per second

#define TURNSPD             0.01f                 // Cutoff for turning or same direction
#define SPINRATE            200                   // How fast spinners spin
#define WATCHMIN            0.01f                 // Tolerance for TURNMODE_WATCH

#define GRIP_VERTS                       4
enum e_grip_offset
{
    GRIP_ORIGIN    =               0,                // Spawn attachments at the center
    GRIP_LAST      =               1,                // Spawn particles at the last vertex
    GRIP_LEFT      =              (1 * GRIP_VERTS),  // Left weapon grip starts  4 from last
    GRIP_RIGHT     =              (2 * GRIP_VERTS),  // Right weapon grip starts 8 from last

    // aliases
    GRIP_INVENTORY =               GRIP_ORIGIN,
    GRIP_ONLY      =               GRIP_LEFT
};
typedef enum e_grip_offset grip_offset_t;

grip_offset_t slot_to_grip_offset( slot_t slot );
slot_t        grip_offset_to_slot( grip_offset_t grip );

#define PITDEPTH            -30                     // Depth to kill character

#define NOSKINOVERRIDE      -1          // For import

#define HURTDAMAGE           256                     // Minimum damage for hurt animation

//Knockbacks
#define REEL                7600.0f     // Dampen for melee knock back
#define REELBASE            0.35f

//Water
#define WATERJUMP           12
#define RIPPLETOLERANCE     60          // For deep water
#define SPLASHTOLERANCE     10
#define RIPPLEAND           15          // How often ripples spawn

// Stats
#define LOWSTAT             256                     // Worst...
#define PERFECTSTAT         (60*256)                // Maximum stat without magic effects
#define PERFECTBIG          (100*256)               // Perfect life or mana...
#define HIGHSTAT            (100*256)                // Absolute max adding enchantments as well

// Throwing
#define THROWFIX            30.0f                    // To correct thrown velocities
#define MINTHROWVELOCITY    15.0f
#define MAXTHROWVELOCITY    75.0f

// Inventory
#define MAXNUMINPACK        6                       // Max number of items to carry in pack
#define PACKDELAY           25                      // Time before inventory rotate again
#define GRABDELAY           25                      // Time before grab again

// Z velocity
#define FLYDAMPEN           0.001f                    // Levelling rate for flyers
#define JUMPDELAY           20                      // Time between jumps
#define JUMPINFINITE        255                     // Flying character
#define SLIDETOLERANCE      10                      // Stick to ground better
#define PLATTOLERANCE       50                     // Platform tolerance...
#define PLATADD             -10                     // Height add...
#define PLATASCEND          0.10f                     // Ascension rate
#define PLATKEEP            0.90f                     // Retention rate
#define MOUNTTOLERANCE      (2 * PLATTOLERANCE)
#define STOPBOUNCING        0.1f // 1.0f                // To make objects stop bouncing
#define DROPZVEL            7
#define DROPXYVEL           8

//Timer resets
#define DAMAGETILETIME      32                            // Invincibility time
#define DAMAGETIME          16                            // Invincibility time
#define DEFENDTIME          16                            // Invincibility time
#define BORETIME            generate_randmask( 120, 255 ) // IfBored timer
#define CAREFULTIME         50                            // Friendly fire timer
#define SIZETIME            50                            // Time it takes to resize a character

//------------------------------------
// Team variables
//------------------------------------
#define TEAM_EVIL            4                       // E
#define TEAM_GOOD            6                       // G
#define TEAM_NULL            13                      // N
#define TEAM_DAMAGE          26                      // For damage tiles
#define TEAM_MAX             27                      // Teams A-Z, +1 more for damage tiles
#define NOLEADER            0xFFFF                   // If the team has no leader...

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
struct s_team
{
    bool_t  hatesteam[TEAM_MAX];     // Don't damage allies...
    Uint16  morale;                 // Number of characters on team
    Uint16  leader;                 // The leader of the team
    Uint16  sissy;                  // Whoever called for help last
};
typedef struct s_team team_t;

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// the data necessary to cache the last values required to create the character matrix

// bits that tell you which variables to look at
enum e_matrix_cache_type
{
    MAT_UNKNOWN   = 0,
    MAT_CHARACTER = (1 << 0),
    MAT_WEAPON    = (1 << 1)
};

typedef enum e_matrix_cache_type matrix_cache_type_t;

struct s_matrix_cache
{
    // is the cache data valid?
    bool_t valid;

    // is the matrix data valid?
    bool_t matrix_valid;

    // how was the matrix made?
    matrix_cache_type_t type;

    //---- MAT_CHARACTER data

    // the "Euler" rotation angles in 16-bit form
    GLvector3 rotate;

    // the translate vector
    GLvector3 pos;

    //---- MAT_WEAPON data

    Uint16  grip_chr;                   // !=MAX_CHR if character is a held weapon
    slot_t  grip_slot;                  // SLOT_LEFT or SLOT_RIGHT
    Uint16  grip_verts[GRIP_VERTS];     // Vertices which describe the weapon grip
    Uint32  vert_update;                // last time the object's vertices were updated

    //---- data used for both

    // the body fixed scaling
    GLvector3 scale;
};
typedef struct s_matrix_cache matrix_cache_t;

matrix_cache_t * matrix_cache_init(matrix_cache_t * mcache);

//--------------------------------------------------------------------------------------------
// all the model data that the renderer needs to render the character
struct s_chr_instance
{
    // position info
    GLmatrix       matrix;          // Character's matrix
    matrix_cache_t matrix_cache;    // Did we make one yet?

    Uint16         turn_z;

    // render mode info
    Uint8          alpha;           // 255 = Solid, 0 = Invisible
    Uint8          light;           // 1 = Light, 0 = Normal
    Uint8          sheen;           // 0-15, how shiny it is
    bool_t         enviro;          // Environment map?

    // color info
    Uint8          redshift;        // Color channel shifting
    Uint8          grnshift;
    Uint8          blushift;

    // texture info
    Uint16         texture;         // The texture id of the character's skin
    Uint16         uoffset;         // For moving textures
    Uint16         voffset;

    // lighting
    // Uint16         light_turn_z;    // Character's light rotation 0 to 0xFFFF
    // Uint8          lightlevel_amb;  // 0-255, terrain light
    // Uint8          lightlevel_dir;  // 0-255, terrain light

    // model info
    Uint16         imad;            // Character's model

    // animation info
    Uint16         frame_nxt;       // Character's frame
    Uint16         frame_lst;       // Character's last frame
    Uint8          ilip;            // Character's frame in betweening
    float          flip;            // Character's frame in betweening
    float          rate;

    // linear interpolated frame vertices
    Uint32         color_amb;
    GLvector4      col_amb;
    Uint8          max_light, min_light;

    size_t         vlst_size;
    GLvertex       vlst[MAXVERTICES];

    // graphical optimizations
    bool_t         indolist;        // Has it been added yet?

    // the save data to determine whether re-calculation of vlst is necessary
    float  save_flip;
    Uint32 save_frame;
    int    save_vmin;
    int    save_vmax;
    Uint16 save_frame_nxt;
    Uint16 save_frame_lst;
    Uint32 save_update_wld;

    // the save data to determine whether re-calculation of lighting data is necessary
    Uint32 save_lighting_update_wld;
};

typedef struct s_chr_instance chr_instance_t;

//--------------------------------------------------------------------------------------------
struct s_chr_environment
{
    // floor stuff
    Uint8  twist;
    float  floor_level;           // Height of tile
    float  level;                 // Height of a tile or a platform
    float  zlerp;
    bool_t grounded;              // standing on something?

    // friction stuff
    bool_t is_slipping;
    bool_t is_slippy,    is_watery;
    float  air_friction, ice_friction;
    float fluid_friction_xy, fluid_friction_z;
    float traction, friction_xy;

    // misc states
    bool_t   inwater;
    float    new_vx, new_vy;
    GLvector3 acc;
};
typedef struct s_chr_environment chr_environment_t;

//--------------------------------------------------------------------------------------------
// Data for doing the physics in bump_all_objects(). should prevent you from being bumped into a wall
struct s_phys_data
{
    GLvector3      apos_0, apos_1;
    GLvector3      avel;

    float          bumpdampen;                    // Character bump mass
    Uint32         weight;                        // Weight ( for pressure plates )
    float          dampen;                        // Bounciness
};
typedef struct s_phys_data phys_data_t;

//--------------------------------------------------------------------------------------------
// definition of the level 0 character "bumper"
struct s_chr_bumper_0
{
    float  size;        // Size of bumpers
    float  sizebig;     // For octagonal bumpers
    float  height;      // Distance from head to toe
};
typedef struct s_chr_bumper_0 chr_bumper_0_t;

//--------------------------------------------------------------------------------------------
// definition of the level 1 character "bumper"
struct s_chr_bumper_1
{
    float min_x,  max_x;
    float min_y,  max_y;
    float min_z,  max_z;
    float min_xy, max_xy;
    float min_yx, max_yx;
};
typedef struct s_chr_bumper_1 chr_bumper_1_t;

//--------------------------------------------------------------------------------------------
struct s_chr
{
    EGO_OBJECT_STUFF

    // character state
    ai_state_t     ai;              // ai data
    latch_t        latch;

    // character stats
    Uint8          gender;          // Gender

    Uint8          lifecolor;       // Bar color
    Sint16         life;            // Basic character stats
    Sint16         lifemax;         //  All 8.8 fixed point
    Uint16         lifeheal;
    Sint16         lifereturn;      // Regeneration/poison

    Uint8          manacolor;       // Bar color
    Sint16         mana;            // Mana stuff
    Sint16         manamax;
    Sint16         manaflow;
    Sint16         manareturn;

    Sint16         strength;        // Strength
    Sint16         wisdom;          // Wisdom
    Sint16         intelligence;    // Intelligence
    Sint16         dexterity;       // Dexterity

    Uint32         experience;                    // Experience
    Uint8          experiencelevel;               // Experience Level

    // what the character is holding
    Uint8          pack_ispacked;    // Is it in the inventory?
    Uint8          pack_waspacked;   // Temporary thing...
    Uint16         pack_next;        // Link to the next item
    Uint8          pack_count;       // How many
    Sint16         money;            // Money
    Uint8          ammomax;          // Ammo stuff
    Uint16         ammo;
    Uint16         holdingwhich[SLOT_COUNT]; // !=MAX_CHR if character is holding something
    Uint16         inventory[INVEN_COUNT];   // !=MAX_CHR if character is storing something

    // team stuff
    Uint8          team;            // Character's team
    Uint8          baseteam;        // Character's starting team

    // enchant data
    Uint16         firstenchant;                  // Linked list for enchants
    Uint16         undoenchant;                   // Last enchantment spawned

    float          fat;                           // Character's size
    float          fat_goto;                      // Character's size goto
    Uint8          fat_goto_time;                  // Time left in size change

    // jump stuff
    float          jump_power;                    // Jump power
    Uint8          jumptime;                      // Delay until next jump
    Uint8          jumpnumber;                    // Number of jumps remaining
    Uint8          jumpnumberreset;               // Number of jumps total, 255=Flying
    Uint8          jumpready;                     // For standing on a platform character

    // attachments
    Uint16         attachedto;                    // !=MAX_CHR if character is a held weapon
    slot_t         inwhich_slot;                  // SLOT_LEFT or SLOT_RIGHT

    // platform stuff
    Uint8          platform;                      // Can it be stood on
    Uint16         holdingweight;                 // For weighted buttons
    Uint16         onwhichplatform;               // Am I on a platform?

    // combat stuff
    Uint8          damagetargettype;              // Type of damage for AI DamageTarget
    Uint8          reaffirmdamagetype;            // For relighting torches
    Uint8          damagemodifier[DAMAGE_COUNT];  // Resistances and inversion
    Uint8          defense;                       // Base defense rating
    Uint16         damageboost;                   // Add to swipe damage

    // sound stuff
    Sint8          soundindex[SOUND_COUNT];       // a map for soundX.wav to sound types
    int            loopedsound_channel;           // Which sound channel it is looping on, -1 is none.

    // missle handling
    Uint8          missiletreatment;              // For deflection, etc.
    Uint8          missilecost;                   // Mana cost for each one
    Uint16         missilehandler;                // Who pays the bill for each one...

    // "variable" properties
    bool_t         is_hidden;
    Uint8          alive;                         // Is it alive?
    Uint8          waskilled;                     // Fix for network
    bool_t         isplayer;                      // btrue = player
    bool_t         islocalplayer;                 // btrue = local player
    Uint8          invictus;                      // Totally invincible?
    Uint8          iskursed;                      // Can't be dropped?
    Uint8          nameknown;                     // Is the name known?
    Uint8          ammoknown;                     // Is the ammo known?
    Uint8          hitready;                      // Was it just dropped?
    Uint8          isequipped;                    // For boots and rings and stuff

    // "constant" properties
    Uint8          isitem;                        // Is it grabbable?
    bool_t         cangrabmoney;                  // Picks up coins?
    Uint8          openstuff;                     // Can it open chests/doors?
    Uint8          stickybutt;                    // Rests on floor
    bool_t         isshopitem;                    // Spawned in a shop?
    Uint8          ismount;                       // Can you ride it?
    bool_t         canbecrushed;                  // Crush in a door?
    bool_t         canchannel;                    // Can it convert life to mana?
    Sint16         manacost;                      // Mana cost to use

    // misc timers
    Sint16         grogtime;                      // Grog timer
    Sint16         dazetime;                      // Daze timer
    Sint16         boretime;                      // Boredom timer
    Uint8          carefultime;                   // "You hurt me!" timer
    Uint16         reloadtime;                    // Time before another shot
    Uint8          damagetime;                    // Invincibility timer

    // graphica info
    Uint8          flashand;        // 1,3,7,15,31 = Flash, 255 = Don't
    Uint8          transferblend;   // Give transparency to weapons?
    bool_t         icon;            // Show the icon?
    Uint8          sparkle;         // Sparkle color or 0 for off
    Uint8          staton;          // Display stats?
    Uint16         uoffvel;         // Moving texture speed
    Uint16         voffvel;
    Uint32         shadowsize;      // Size of shadow
    Uint32         shadowsizesave;  // Without size modifiers
    Uint16         ibillboard;      // The attached billboard

    // action info
    Uint8          actionready;                   // Ready to play a new one
    Uint8          action;                        // Character's action
    bool_t         keepaction;                    // Keep the action playing
    bool_t         loopaction;                    // Loop it too
    Uint8          nextaction;                    // Character's action to play next

    // model info
    bool_t         is_overlay;                    // Is this an overlay? Track aitarget...
    Uint16         skin;                          // Character's skin
    Uint8          iprofile;                      // Character's profile
    Uint8          basemodel;                     // The true form
    Uint8          basealpha;
    Uint8          baselight;
    chr_instance_t inst;                          // the render data

    // Skills
    Sint8           shieldproficiency;            // Can it use shields?
    bool_t          canjoust;
    bool_t          canuseadvancedweapons;
    bool_t          canseeinvisible;
    bool_t          canseekurse;
    bool_t          canusedivine;
    bool_t          canusearcane;
    bool_t          canusetech;
    bool_t          candisarm;
    bool_t          canbackstab;
    bool_t          canusepoison;
    bool_t          canread;
    bool_t            hascodeofconduct;

    // collision info

    // note - to make it easier for things to "hit" one another (like a damage particle from
    //        a torch hitting a grub bug), Aaron sometimes made the bumper size much different
    //        than the shape of the actual object.
    //        The old bumper data that is read from the data.txt file will be kept in
    //        the struct "bump". A new bumper that actually matches the size of the object will
    //        be kept in the struct "collision"
    chr_bumper_0_t   bump;
    chr_bumper_0_t   bump_save;

    chr_bumper_0_t   collision_0;
    chr_bumper_1_t   collision_1;


    Uint8          stoppedby;                     // Collision mask

    // character location data
    GLvector3      pos_stt;                       // Starting position
    GLvector3      pos;                           // Character's position
    GLvector3      vel;                           // Character's velocity

    Uint16         turn_z;                        // Character's z-rotation 0 to 0xFFFF
    Uint16         map_turn_y;
    Uint16         map_turn_x;

    GLvector3      pos_old;                       // Character's last position

    bool_t         safe_valid;
    GLvector3      pos_safe;                      // Character's last safe position

    GLvector3      vel_old;                       // Character's last velocity

    Uint16         turn_old_z;

    Uint32         onwhichfan;                    // Where the char is
    Uint32         onwhichblock;                  // The character's collision block
    Uint16         fanblock_next;                 // Next character on fanblock

    // movement properties
    Uint8          waterwalk;                     // Always above watersurfacelevel?
    TURN_MODE      turnmode;                      // Turning mode
    Uint8          sneakspd;                      // Sneaking if above this speed
    Uint8          walkspd;                       // Walking if above this speed
    Uint8          runspd;                        // Running if above this speed
    float          maxaccel;                      // Maximum acceleration
    Uint8          flyheight;                     // Height to stabilize at

    // data for doing the physics in bump_all_objects()
    phys_data_t       phys;
    chr_environment_t enviro;

    // a timer BB added in to make mounts and dismounts not so unpredictable
    int                dismount_timer;

};

typedef struct s_chr chr_t;

//--------------------------------------------------------------------------------------------
// list definitions
//--------------------------------------------------------------------------------------------

extern team_t TeamList[TEAM_MAX];

extern cap_t CapList[MAX_CAP];

#define VALID_CAP_RANGE( ICAP ) ( ((ICAP) >= 0) && ((ICAP) < MAX_CAP) )
#define VALID_CAP( ICAP )       ( VALID_CAP_RANGE( ICAP ) && CapList[ICAP].loaded )
#define INVALID_CAP( ICAP )     ( !VALID_CAP_RANGE( ICAP ) || !CapList[ICAP].loaded )

DEFINE_LIST_EXTERN(chr_t, ChrList, MAX_CHR );

#define VALID_CHR_RANGE( ICHR ) ( ((ICHR) >= 0) && ((ICHR) < MAX_CHR) )
#define ALLOCATED_CHR( ICHR )   ( VALID_CHR_RANGE( ICHR ) && ChrList.lst[ICHR].allocated )
#define ACTIVE_CHR( ICHR )      ( ALLOCATED_CHR( ICHR ) && ChrList.lst[ICHR].active )
#define INACTIVE_CHR( ICHR )    ( ALLOCATED_CHR( ICHR ) && !ChrList.lst[ICHR].active )

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// Function prototypes

void drop_money( Uint16 character, Uint16 money );
void call_for_help( Uint16 character );
void give_experience( Uint16 character, int amount, Uint8 xptype, bool_t override_invictus );
void give_team_experience( Uint8 team, int amount, Uint8 xptype );
void damage_character( Uint16 character, Uint16 direction,
                       IPair damage, Uint8 damagetype, Uint8 team,
                       Uint16 attacker, Uint16 effects, bool_t ignoreinvincible );
void kill_character( Uint16 character, Uint16 killer );
bool_t heal_character( Uint16 character, Uint16 healer, int amount, bool_t ignoreinvincible);
void spawn_poof( Uint16 character, Uint16 profile );
void reset_character_alpha( Uint16 character );
void reset_character_accel( Uint16 character );
bool_t detach_character_from_mount( Uint16 character, Uint8 ignorekurse, Uint8 doshop );

void flash_character_height( Uint16 character, Uint8 valuelow, Sint16 low,
                             Uint8 valuehigh, Sint16 high );
void flash_character( Uint16 character, Uint8 value );

void free_one_character_in_game( Uint16 character );
//void make_one_weapon_matrix( Uint16 iweap, Uint16 iholder, bool_t do_phys  );
void make_all_character_matrices(bool_t do_physics);
void free_inventory_in_game( Uint16 character );

void keep_weapons_with_holders();
void make_one_character_matrix( Uint16 cnt );

void update_all_characters( void );
void move_all_characters( void );
void cleanup_all_characters( void );

void do_level_up( Uint16 character );
bool_t setup_xp_table(Uint16 character);

void free_all_chraracters();

Uint32 __chrhitawall( Uint16 character, float nrm[] );

int chr_count_free();

char * chop_create( Uint16 profile );

Uint16 spawn_one_character( GLvector3 pos, Uint16 profile, Uint8 team, Uint8 skin, Uint16 facing, const char *name, Uint16 override );
void respawn_character( Uint16 character );
Uint16 change_armor( Uint16 character, Uint16 skin );
void change_character( Uint16 cnt, Uint16 profile, Uint8 skin, Uint8 leavewhich );
void change_character_full( Uint16 ichr, Uint16 profile, Uint8 skin, Uint8 leavewhich );
bool_t cost_mana( Uint16 character, int amount, Uint16 killer );
void switch_team( Uint16 character, Uint8 team );
void issue_clean( Uint16 character );
int  restock_ammo( Uint16 character, IDSZ idsz );
void attach_character_to_mount( Uint16 character, Uint16 mount, grip_offset_t grip_off );
bool_t inventory_add_item( Uint16 item, Uint16 character );
Uint16 inventory_get_item( Uint16 character, grip_offset_t grip_off, bool_t ignorekurse );
void drop_keys( Uint16 character );
bool_t drop_all_items( Uint16 character );
bool_t character_grab_stuff( Uint16 chara, grip_offset_t grip, bool_t people );

void chr_play_action( Uint16 character, Uint16 action, Uint8 actionready );
void chr_set_frame( Uint16 character, Uint16 action, int frame, Uint16 lip );

bool_t export_one_character_name( const char *szSaveName, Uint16 character );
bool_t export_one_character_profile( const char *szSaveName, Uint16 character );
bool_t export_one_character_skin( const char *szSaveName, Uint16 character );
int    load_one_character_profile( const char *szLoadName, int slot_override, bool_t required );

void character_swipe( Uint16 cnt, slot_t slot );

int check_skills( Uint16 who, IDSZ whichskill );

bool_t looped_stop_object_sounds( Uint16 character );

bool_t is_invictus_direction( Uint16 direction, Uint16 character, Uint16 effects );

void   init_slot_idsz();

bool_t ai_add_order( ai_state_t * pai, Uint32 value, Uint16 counter );

bool_t chr_make_text_billboard( Uint16 ichr, const char * txt, SDL_Color color, int lifetime_secs );
const char * chr_get_name( Uint16 ichr );

Uint16 ChrList_get_free();

//---------------------------------------------------------------------------------------------
// Quest system
bool_t quest_add_idsz( const char *whichplayer, IDSZ idsz );
Sint16 quest_modify_idsz( const char *whichplayer, IDSZ idsz, Sint16 adjustment );
Sint16 quest_check( const char *whichplayer, IDSZ idsz );

//---------------------------------------------------------------------------------------------
// helper functions

void init_all_cap();
void release_all_cap();
bool_t release_one_cap( Uint16 icap );

bool_t cap_is_type_idsz( Uint16 icap, IDSZ test_idsz );

Uint16 chr_get_ipro(Uint16 ichr);
Uint16 chr_get_icap(Uint16 ichr);
Uint16 chr_get_imad(Uint16 ichr);
Uint16 chr_get_ieve(Uint16 ichr);
Uint16 chr_get_ipip(Uint16 ichr, Uint16 ipip);
Uint16 chr_get_iteam(Uint16 ichr);
Uint16 chr_get_iteam_base(Uint16 ichr);

struct s_object_profile * chr_get_ppro(Uint16 ichr);
struct s_cap * chr_get_pcap(Uint16 ichr);
struct s_mad * chr_get_pmad(Uint16 ichr);
struct s_eve * chr_get_peve(Uint16 ichr);
struct s_pip * chr_get_ppip(Uint16 ichr, Uint16 ipip);

Mix_Chunk      * chr_get_chunk_ptr( chr_t * pchr, int index );
Mix_Chunk      * chr_get_chunk( Uint16 ichr, int index );
team_t         * chr_get_pteam(Uint16 ichr);
team_t         * chr_get_pteam_base(Uint16 ichr);
ai_state_t     * chr_get_pai(Uint16 ichr);
chr_instance_t * chr_get_pinstance(Uint16 ichr);

Uint16   team_get_ileader( Uint16 iteam );
chr_t  * team_get_pleader( Uint16 iteam );

bool_t team_hates_team(Uint16 ipredator, Uint16 iprey);

IDSZ chr_get_idsz(Uint16 ichr, Uint16 type);

void chr_update_size( chr_t * pchr );
void chr_init_size( chr_t * pchr, cap_t * pcap );
void chr_set_size( chr_t * pchr, float size );
void chr_set_width( chr_t * pchr, float width );
void chr_set_shadow( chr_t * pchr, float width );
void chr_set_height( chr_t * pchr, float height );
void chr_set_fat( chr_t * pchr, float fat );

bool_t chr_has_idsz( Uint16 ichr, IDSZ idsz );
bool_t chr_is_type_idsz( Uint16 ichr, IDSZ idsz );
bool_t chr_has_vulnie( Uint16 item, Uint16 weapon_profile );

Uint32 chr_get_icon_ref( Uint16 item );

const char * describe_stat(int value_high);

void reset_teams();

bool_t chr_update_matrix( chr_t * pchr );
bool_t chr_teleport( Uint16 ichr, float x, float y, float z, Uint16 turn_z );

bool_t chr_request_terminate( Uint16 ichr );

chr_t * chr_update_hide( chr_t * pchr );

bool_t ai_state_set_changed( ai_state_t * pai);

bool_t chr_matrix_valid( chr_t * pchr );

bool_t chr_getMatUp(chr_t *pchr, GLvector3 * pvec);
bool_t chr_getMatRight(chr_t *pchr, GLvector3 * pvec);
bool_t chr_getMatForward(chr_t *pchr, GLvector3 * pvec);
bool_t chr_getMatTranslate(chr_t *pchr, GLvector3 * pvec);