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

/// @file font_ttf.c
/// @brief TTF management
/// @details True-type font drawing functionality.  Uses Freetype 2 & OpenGL
/// to do its business.

#include "font_ttf.h"
#include "log.h"

#include "extensions/ogl_include.h"
#include "extensions/ogl_debug.h"
#include "extensions/SDL_GL_extensions.h"

#include "egoboo_typedef.h"
#include "egoboo_strutil.h"
#include "egoboo_math.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <SDL.h>

#include "egoboo_mem.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#define DEFAULT_DISPLAY_TEXT 20

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
static int fnt_atexit_registered = 0;

int fnt_init()
{
    /// @details BB@> Make sure the TTF library was initialized

    int initialized;

    initialized = TTF_WasInit();
    if ( !initialized )
    {
        log_info( "Initializing the SDL_ttf font handler version %i.%i.%i... ", SDL_TTF_MAJOR_VERSION, SDL_TTF_MINOR_VERSION, SDL_TTF_PATCHLEVEL );
        if ( TTF_Init() < 0 )
        {
            log_message( "Failed!\n" );
        }
        else
        {
            log_message( "Success!\n" );

            if ( !fnt_atexit_registered )
            {
                fnt_atexit_registered  = 1;
                atexit( TTF_Quit );
            }

            initialized = 1;
        }
    }

    return initialized;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
TTF_Font * fnt_loadFont( const char *fileName, int pointSize )
{
    TTF_Font *retval = NULL;

    if ( !fnt_init() )
    {
        printf( "fnt_loadFont: Could not initialize SDL_TTF!\n" );
    }
    else
    {
        // Try and open the font
        retval = TTF_OpenFont( fileName, pointSize );
    }

    return retval;
}

//--------------------------------------------------------------------------------------------
void fnt_freeFont( TTF_Font *font )
{
    if ( NULL != font )
    {
        TTF_CloseFont( font );
    }
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
display_list_t * fnt_vappend_text( display_list_t * tx_lst, TTF_Font * ttf_ptr, int x, int y, const char *format, va_list args )
{
    display_item_t * pitem = NULL;

    // trap cases that will generate an error
    if ( NULL == ttf_ptr || NULL == format ) return tx_lst;

    // convert the text to a display item
    pitem = fnt_vconvertText( pitem, ttf_ptr, format, args );
    if ( NULL != pitem )
    {
        display_item_set_pos( pitem, x, y );
    }

    // try to append the item
    pitem = display_list_append( tx_lst, pitem );
    if ( NULL != pitem )
    {
        // the append failed. we must delete the local pitem pointer
        pitem = display_item_free( pitem, btrue );
    }

    return tx_lst;
}

//--------------------------------------------------------------------------------------------
display_list_t * fnt_append_text( display_list_t * tx_lst, TTF_Font * ttf_ptr, int x, int y, const char *format, ... )
{
    va_list args;
    display_list_t * ret;

    va_start( args, format );
    ret = fnt_vappend_text( tx_lst, ttf_ptr, x, y, format, args );
    va_end( args );

    return ret;
}

//--------------------------------------------------------------------------------------------
display_list_t * fnt_append_append_text_literal( display_list_t * tx_lst, TTF_Font * ttf_ptr, int x, int y, const char *text )
{
    display_item_t * pitem = NULL;

    // trap cases that will generate an error
    if ( NULL == ttf_ptr || NULL == text ) return tx_lst;

    // convert the text to a display item
    pitem = fnt_convertText_literal( pitem, ttf_ptr, text );
    if ( NULL != pitem )
    {
        display_item_set_pos( pitem, x, y );
    }

    // try to append the item
    pitem = display_list_append( tx_lst, pitem );
    if ( NULL != pitem )
    {
        // the append failed. we must delete the local pitem pointer
        pitem = display_item_free( pitem, btrue );
    }

    return tx_lst;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
display_item_t * fnt_print_raw( display_item_t * display_src, TTF_Font * ttf_ptr, SDL_Color color, const char * szText )
{
    bool_t print_error       = bfalse;
    bool_t display_ptr_local = bfalse;

    SDL_Surface * tmp_surface = NULL;

    // no font = nothing
    if ( NULL == ttf_ptr )
    {
        print_error = btrue;
        goto fnt_print_raw_finish;
    }

    // make sure there is a display
    if ( NULL == display_src )
    {
        display_src       = display_item_new();
        display_ptr_local = btrue;
    }

    // create the text on the temporary surface
    tmp_surface = TTF_RenderText_Blended( ttf_ptr, szText, color );
    if ( NULL == tmp_surface )
    {
        print_error = btrue;
        goto fnt_print_raw_finish;
    }
    else
    {
        // upload the texture
        display_src = display_item_validate_texture( display_src );

        if ( !SDL_GL_uploadSurface( tmp_surface, display_item_texture_name( display_src ), display_item_texCoords( display_src ) ) )
        {
            print_error = btrue;
            goto fnt_print_raw_finish;
        }
    }

fnt_print_raw_finish:

    // copy over some display info
    if ( NULL != display_src && NULL != tmp_surface )
    {
        frect_t * prect = display_item_prect( display_src );
        if ( NULL != prect )
        {
            prect->w = tmp_surface->w;
            prect->h = tmp_surface->h;
        }
    }

    // Done with the surface
    if ( NULL != tmp_surface )
    {
        SDL_FreeSurface( tmp_surface );
        tmp_surface = NULL;
    }

    // "delete" the display_src on an error
    if ( print_error )
    {
        display_src = display_item_free( display_src, !display_ptr_local );
    }

    return display_src;
}

//--------------------------------------------------------------------------------------------
display_item_t * fnt_vprintf( display_item_t * tx_ptr, TTF_Font * ttf_ptr, SDL_Color color, const char *format, va_list args )
{
    int rv;
    STRING szText = EMPTY_CSTR;

    // evaluate the variable args
    rv = vsnprintf( szText, SDL_arraysize( szText ) - 1, format, args );

    if ( rv < 0 )
    {
        tx_ptr = display_item_free( tx_ptr, btrue );
    }
    else
    {
        tx_ptr = fnt_print_raw( tx_ptr, ttf_ptr, color, szText );
    }

    return tx_ptr;
}

//--------------------------------------------------------------------------------------------
display_item_t * fnt_drawText_raw( display_item_t *tx_ptr, TTF_Font * font_ptr, int x, int y, const char *text )
{
    SDL_Color color = { 0xFF, 0xFF, 0xFF, 0 };

    tx_ptr = fnt_print_raw( tx_ptr, font_ptr, color, text );
    if ( NULL != tx_ptr )
    {
        frect_t * prect = display_item_prect( tx_ptr );
        if ( NULL != prect )
        {
            // set the position of the display
            prect->x = x;
            prect->y = y;
        }

        // And draw the darn thing
        display_item_draw( tx_ptr );
    }

    return tx_ptr;
}

//--------------------------------------------------------------------------------------------
display_item_t * fnt_vconvertText( display_item_t *tx_ptr, TTF_Font * ttf_ptr, const char *format, va_list args )
{
    SDL_Color color = { 0xFF, 0xFF, 0xFF, 0 };

    return fnt_vprintf( tx_ptr, ttf_ptr, color, format, args );
}

//--------------------------------------------------------------------------------------------
display_item_t * fnt_convertText_literal( display_item_t *tx_ptr, TTF_Font * ttf_ptr, const char *text )
{
    SDL_Color color = { 0xFF, 0xFF, 0xFF, 0 };

    return fnt_print_raw( tx_ptr, ttf_ptr, color, text );
}

//--------------------------------------------------------------------------------------------
display_item_t * fnt_convertText( display_item_t *tx_ptr, TTF_Font * ttf_ptr, const char *format, ... )
{
    va_list args;

    va_start( args, format );
    tx_ptr = fnt_vconvertText( tx_ptr, ttf_ptr, format, args );
    va_end( args );

    return tx_ptr;
}

//--------------------------------------------------------------------------------------------
const char * fnt_vgetTextSize( TTF_Font * ttf_ptr, int *pwidth, int *pheight, const char *format, va_list args )
{
    static char text[4096] = EMPTY_CSTR;

    int rv;
    int loc_width = 0, loc_height = 0;

    // clear the string
    text[0] = CSTR_END;

    // convert the string
    rv = vsnprintf( text, SDL_arraysize( text ) - 1, format, args );

    // if there was an error converting the string, return
    if ( rv < 0 || NULL == text || '\0' == text[0] ) return NULL;

    // if there is no font pointer, do nothing
    if ( NULL != ttf_ptr ) return NULL;

    if ( NULL == pwidth ) pwidth  = &loc_width;
    if ( NULL == pheight ) pheight = &loc_height;

    TTF_SizeText( ttf_ptr, text, pwidth, pheight );

    return text;
}

//--------------------------------------------------------------------------------------------
const char * fnt_getTextSize( TTF_Font * ttf_ptr, int *pwidth, int *pheight, const char *format, ... )
{
    const char * rv;
    va_list args;

    va_start( args, format );
    rv = fnt_vgetTextSize( ttf_ptr, pwidth, pheight, format, args );
    va_end( args );

    return rv;
}

//--------------------------------------------------------------------------------------------
/** font_drawTextBox
 * Draws a text string into a box, splitting it into lines according to newlines in the string.
 * @warning Doesn't pay attention to the width/height arguments yet.
 *
 * @var font    - The font to draw with
 * @var text    - The text to draw
 * @var x       - The x position to start drawing at
 * @var y       - The y position to start drawing at
 * @var width   - Maximum width of the box (not implemented)
 * @var height  - Maximum height of the box (not implemented)
 * @var spacing - Amount of space to move down between lines. (usually close to your font size)
 */

int fnt_vconvertTextBox( display_list_t * tx_lst, TTF_Font * ttf_ptr, int x, int y, int spacing, const char *format, va_list args )
{
    int retval;
    int vsnprintf_rv;

    char text[4096] = EMPTY_CSTR;

    vsnprintf_rv = vsnprintf( text, SDL_arraysize( text ) - 1, format, args );

    // some problem printing the text?
    if ( vsnprintf_rv < 0 )
    {
        tx_lst = display_list_dtor( tx_lst, bfalse );
        retval = 0;
    }
    else
    {
        retval = fnt_convertTextBox_literal( tx_lst, ttf_ptr, x, y, spacing, text );
    }

    return retval;
}

//--------------------------------------------------------------------------------------------
int fnt_convertTextBox_literal( display_list_t * tx_lst, TTF_Font * ttf_ptr, int x, int y, int spacing, const char *text )
{
    size_t cnt = 0;
    size_t len;
    char *buffer, *line;
    display_item_t * loc_display_ptr = NULL;
    size_t texture_lst_size;
    bool_t local_tex_lst;

    // assume that we are passed a valid tx_lst
    local_tex_lst = bfalse;

    // count the size of the display list
    texture_lst_size = display_list_size( tx_lst );

    // abort on an error
    if ( NULL == text || '\0' == text[0] ) goto fnt_convertTextBox_literal_finish;

    // create a valid tx_lst, if needed
    if ( NULL == tx_lst )
    {
        tx_lst = display_list_ctor( tx_lst, DEFAULT_DISPLAY_TEXT );
        local_tex_lst = btrue;
    }

    // Split the passed in text into separate lines
    len = strlen( text );
    buffer = EGOBOO_NEW_ARY( char, len + 1 );
    strncpy( buffer, text, len );

    // take care in case there is no '\n' at the end of the string
    line = strtok( buffer, "\n" );
    if ( NULL == line ) line = buffer;

    // initialize the loop
    cnt = 0;
    while ( NULL != line && cnt < texture_lst_size )
    {
        fnt_append_append_text_literal( tx_lst, ttf_ptr, x, y, line );
        y += spacing;

        line = strtok( NULL, "\n" );
        cnt++;
    }

    // finish up the text, but don't store it
    while ( NULL != line )
    {
        loc_display_ptr = fnt_convertText_literal( loc_display_ptr, ttf_ptr, line );
        display_item_set_pos( loc_display_ptr, x, y );

        y += spacing;
        line = strtok( NULL, "\n" );
        cnt++;
    }

    EGOBOO_DELETE_ARY( buffer );

fnt_convertTextBox_literal_finish:

    // was there an "error"?
    if ( NULL != tx_lst && cnt > texture_lst_size )
    {
        // deconstruct the tx_lst
        tx_lst = display_list_dtor( tx_lst, local_tex_lst );
    }
    else
    {
        // determine the bound of the list
        display_list_pbound( tx_lst, NULL );
    }

    return cnt;
}

//--------------------------------------------------------------------------------------------
int fnt_convertTextBox( display_list_t * tx_lst, TTF_Font * ttf_ptr, int x, int y, int spacing, const char *format, ... )
{
    va_list args;
    int cnt;

    va_start( args, format );
    cnt = fnt_vconvertTextBox( tx_lst, ttf_ptr, x, y, spacing, format, args );
    va_end( args );

    return cnt;
}

//--------------------------------------------------------------------------------------------
const char * fnt_vgetTextBoxSize( TTF_Font *ttf_ptr, int spacing, int *pwidth, int *pheight, const char *format, va_list args )
{
    int rv;
    static char text[4096] = EMPTY_CSTR;

    int count = 0;

    char *buffer, *line;
    size_t len;
    int loc_width = 0, loc_height = 0;

    // clear the string
    text[0] = CSTR_END;

    // convert the string
    rv = vsnprintf( text, SDL_arraysize( text ) - 1, format, args );

    // if there was an error converting the string, return
    if ( rv < 0 || NULL == text || '\0' == text[0] ) return NULL;

    // if there is no font, we can't do anything
    if ( NULL == ttf_ptr ) return NULL;

    // handle the optional parameters
    if ( NULL == pwidth ) pwidth  = &loc_width;
    if ( NULL == pheight ) pheight = &loc_height;

    // Allocate a buffer to hold a copy of the string
    len = strlen( text );
    buffer = EGOBOO_NEW_ARY( char, len + 1 );
    strncpy( buffer, text, len );

    // take care in case there is no '\n' at the end of the string
    line = strtok( buffer, "\n" );
    if ( NULL == line ) line = buffer;

    // initialize the loop
    *pwidth = *pheight = 0;
    count = 0;

    // loop until finished with all lines
    while ( NULL != line )
    {
        int tmp_w, tmp_h;

        TTF_SizeText( ttf_ptr, line, &tmp_w, &tmp_h );

        *pwidth = ( *pwidth > tmp_w ) ? *pwidth : tmp_w;
        *pheight += spacing;

        line = strtok( NULL, "\n" );

        count++;
    }

    EGOBOO_DELETE_ARY( buffer );

    return text;
}

//--------------------------------------------------------------------------------------------
const char * fnt_getTextBoxSize( TTF_Font *ttf_ptr, int spacing, int *pwidth, int *pheight, const char *format, ... )
{
    const char * rv;
    va_list      args;

    va_start( args, format );
    rv = fnt_vgetTextBoxSize( ttf_ptr, spacing, pwidth, pheight, format, args );
    va_end( args );

    return rv;
}