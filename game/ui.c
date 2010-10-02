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

/// @file ui.c
/// @brief The Egoboo GUI
/// @details A basic library for implementing user interfaces, based off of Casey Muratori's
/// IMGUI.  (https://mollyrocket.com/forums/viewtopic.php?t=134)

#include "ui.h"
#include "graphic.h"
#include "texture.h"

#include "ogl_debug.h"
#include "SDL_extensions.h"

#include <string.h>
#include <SDL_opengl.h>

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#define UI_MAX_JOYSTICKS 8
#define UI_CONTROL_TIMER 100

struct s_ui_control_info
{
    int    timer;
    float  scr[2];
    float  vrt[2];
};
typedef struct s_ui_control_info ui_control_info_t;

/// The data to describe the UI state
struct UiContext
{
    // Tracking control focus stuff
    ui_id_t active;
    ui_id_t hot;

    // info on the mouse control
    ui_control_info_t mouse;
    ui_control_info_t joy;
    ui_control_info_t joys[UI_MAX_JOYSTICKS];

    // Basic cursor state
    float cursor_X, cursor_Y;
    int   cursor_Released;
    int   cursor_Pressed;

    STRING defaultFontName;
    float  defaultFontSize;
    Font  *defaultFont;
    Font  *activeFont;

    // virtual window
    float vw, vh, ww, wh;

    // define the forward transform
    float aw, ah, bw, bh;

    // define the inverse transform
    float iaw, iah, ibw, ibh;
};

static struct UiContext ui_context;

static const GLfloat ui_white_color[]  = {1.00f, 1.00f, 1.00f, 1.00f};

static const GLfloat ui_active_color[]  = {0.00f, 0.00f, 0.90f, 0.60f};
static const GLfloat ui_hot_color[]     = {0.54f, 0.00f, 0.00f, 1.00f};
static const GLfloat ui_normal_color[]  = {0.66f, 0.00f, 0.00f, 0.60f};

static const GLfloat ui_active_color2[] = {0.00f, 0.45f, 0.45f, 0.60f};
static const GLfloat ui_hot_color2[]    = {0.00f, 0.28f, 0.28f, 1.00f};
static const GLfloat ui_normal_color2[] = {0.33f, 0.00f, 0.33f, 0.60f};

static void ui_virtual_to_screen_abs( float vx, float vy, float *rx, float *ry );
static void ui_screen_to_virtual_abs( float rx, float ry, float *vx, float *vy );

static void ui_virtual_to_screen_rel( float vx, float vy, float *rx, float *ry );
static void ui_screen_to_virtual_rel( float rx, float ry, float *vx, float *vy );

static void ui_joy_init();
static void ui_cursor_update();
static bool_t ui_joy_set(SDL_JoyAxisEvent * evt_ptr );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// Core functions
int ui_begin( const char *default_font, int default_font_size )
{
    // initialize the font handler
    fnt_init();

    memset( &ui_context, 0, sizeof( ui_context ) );

    ui_context.active = ui_context.hot = UI_Nothing;

    ui_context.defaultFontSize = default_font_size;
    strncpy( ui_context.defaultFontName, default_font, SDL_arraysize( ui_context.defaultFontName ) );

    ui_set_virtual_screen( sdl_scr.x, sdl_scr.y, sdl_scr.x, sdl_scr.y );

    ui_joy_init();

    return 1;
}

//--------------------------------------------------------------------------------------------
void ui_end()
{
    // clear out the default font
    if ( NULL != ui_context.defaultFont )
    {
        fnt_freeFont( ui_context.defaultFont );
        ui_context.defaultFont = NULL;
    }

    // clear out the active font
    ui_context.activeFont = NULL;

    memset( &ui_context, 0, sizeof( ui_context ) );
}

//--------------------------------------------------------------------------------------------
void ui_Reset()
{
    ui_context.active = ui_context.hot = UI_Nothing;
}

//--------------------------------------------------------------------------------------------
bool_t ui_handleSDLEvent( SDL_Event *evt )
{
    bool_t handled;

    if ( NULL == evt ) return bfalse;

    handled = btrue;
    switch ( evt->type )
    {
        case SDL_JOYBUTTONDOWN:
        case SDL_MOUSEBUTTONDOWN:
            ui_context.cursor_Released = 0;
            ui_context.cursor_Pressed = 1;
            break;

        case SDL_JOYBUTTONUP:
        case SDL_MOUSEBUTTONUP:
            ui_context.cursor_Pressed = 0;
            ui_context.cursor_Released = 1;
            break;

        case SDL_MOUSEMOTION:
            // convert the screen coordinates to our "virtual coordinates"
            ui_context.mouse.scr[0] = evt->motion.x;
            ui_context.mouse.vrt[1] = evt->motion.y;
            ui_screen_to_virtual_abs( ui_context.mouse.scr[0], ui_context.mouse.vrt[1], &(ui_context.mouse.vrt[0]), &(ui_context.mouse.vrt[1]) );
            ui_context.mouse.timer  = 2 * UI_CONTROL_TIMER;
            break;

        case SDL_JOYAXISMOTION:
            ui_joy_set( &(evt->jaxis) );
            break;

        case SDL_VIDEORESIZE:
            if ( SDL_VIDEORESIZE == evt->resize.type )
            {
                // the video has been re-sized, if the game is active, the
                // view matrix needs to be recalculated and possibly the
                // auto-formatting for the menu system and the ui system must be
                // recalculated

                // grab all the new SDL screen info
                SDLX_Get_Screen_Info( &sdl_scr, bfalse );

                // set the ui's virtual screen size based on the graphic system's
                // configuration
                gfx_set_virtual_screen( &gfx );
            }
            break;

        default:
            handled = bfalse;
    }

    return handled;
}

//--------------------------------------------------------------------------------------------
void ui_beginFrame( float deltaTime )
{
    ATTRIB_PUSH( "ui_beginFrame", GL_ENABLE_BIT );
    GL_DEBUG( glDisable )( GL_DEPTH_TEST );
    GL_DEBUG( glDisable )( GL_CULL_FACE );
    GL_DEBUG( glEnable )( GL_TEXTURE_2D );

    GL_DEBUG( glEnable )( GL_BLEND );
    GL_DEBUG( glBlendFunc )( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    GL_DEBUG( glViewport )( 0, 0, sdl_scr.x, sdl_scr.y );

    // Set up an ortho projection for the gui to use.  Controls are free to modify this
    // later, but most of them will need this, so it's done by default at the beginning
    // of a frame
    GL_DEBUG( glMatrixMode )( GL_PROJECTION );
    GL_DEBUG( glPushMatrix )();
    GL_DEBUG( glLoadIdentity )();
    GL_DEBUG( glOrtho )( 0, sdl_scr.x, sdl_scr.y, 0, -1, 1 );

    GL_DEBUG( glMatrixMode )( GL_MODELVIEW );
    GL_DEBUG( glLoadIdentity )();

    // hotness gets reset at the start of each frame
    ui_context.hot = UI_Nothing;

    // update the cursor position
    ui_cursor_update();
}

//--------------------------------------------------------------------------------------------
void ui_endFrame()
{
    // Draw the cursor last
    float x, y;

    ui_virtual_to_screen_abs( ui_context.cursor_X-5, ui_context.cursor_Y-5, &x, &y );

	//Draw the cursor, but only if it inside the screen
	draw_one_icon( ( TX_REF )TX_CURSOR, x, y, NOSPARKLE );

/*
	float x1,y1,x2,y2;
    GL_DEBUG( glDisable )( GL_TEXTURE_2D );
    ui_virtual_to_screen_abs( ui_context.cursor_X-5, ui_context.cursor_Y-5, &x1, &y1 );
    ui_virtual_to_screen_abs( ui_context.cursor_X+5, ui_context.cursor_Y+5, &x2, &y2 );
    GL_DEBUG( glColor4f )( 1,1,1,1 );
    GL_DEBUG( glBegin )( GL_QUADS );
    {
        GL_DEBUG( glVertex2f )( x1, y1 );
        GL_DEBUG( glVertex2f )( x1, y2 );
        GL_DEBUG( glVertex2f )( x2, y2 );
        GL_DEBUG( glVertex2f )( x2, y1 );
    }
    GL_DEBUG_END();


    GL_DEBUG( glEnable )( GL_TEXTURE_2D );
*/

    // Restore the OpenGL matrices to what they were
    GL_DEBUG( glMatrixMode )( GL_PROJECTION );
    GL_DEBUG( glPopMatrix )();

    GL_DEBUG( glMatrixMode )( GL_MODELVIEW );
    GL_DEBUG( glLoadIdentity )();

    // Re-enable any states disabled by gui_beginFrame
    ATTRIB_POP( "ui_endFrame" );

    // Clear input states at the end of the frame
    ui_context.cursor_Pressed = ui_context.cursor_Released = 0;
}

//--------------------------------------------------------------------------------------------
// Utility functions
int ui_mouseInside( float vx, float vy, float vwidth, float vheight )
{
    float vright, vbottom;

    vright  = vx + vwidth;
    vbottom = vy + vheight;
    if ( vx <= ui_context.cursor_X && vy <= ui_context.cursor_Y && ui_context.cursor_X <= vright && ui_context.cursor_Y <= vbottom )
    {
        return 1;
    }

    return 0;
}

//--------------------------------------------------------------------------------------------
void ui_setactive( ui_id_t id )
{
    ui_context.active = id;
}

//--------------------------------------------------------------------------------------------
void ui_sethot( ui_id_t id )
{
    ui_context.hot = id;
}

//--------------------------------------------------------------------------------------------
void ui_setWidgetactive( ui_Widget_t * pw )
{
    if ( NULL == pw )
    {
        ui_context.active = UI_Nothing;
    }
    else
    {
        ui_context.active = pw->id;

        pw->timeout = egoboo_get_ticks() + 100;
        if ( HAS_SOME_BITS( pw->mask, UI_BITS_CLICKED ) )
        {
            // use exclusive or to flip the bit
            pw->state ^= UI_BITS_CLICKED;
        };
    };
}

//--------------------------------------------------------------------------------------------
void ui_setWidgethot( ui_Widget_t * pw )
{
    if ( NULL == pw )
    {
        ui_context.hot = UI_Nothing;
    }
    else if (( ui_context.active == pw->id || ui_context.active == UI_Nothing ) )
    {
        if ( pw->timeout < egoboo_get_ticks() )
        {
            pw->timeout = egoboo_get_ticks() + 100;

            if ( HAS_SOME_BITS( pw->mask, UI_BITS_MOUSEOVER ) && ui_context.hot != pw->id )
            {
                // use exclusive or to flip the bit
                pw->state ^= UI_BITS_MOUSEOVER;
            };
        };

        // Only allow hotness to be set if this control, or no control is active
        ui_context.hot = pw->id;
    }
}

//--------------------------------------------------------------------------------------------
Font* ui_getFont()
{
    return ( NULL != ui_context.activeFont ) ? ui_context.activeFont : ui_context.defaultFont;
}

//--------------------------------------------------------------------------------------------
Font* ui_setFont( Font * font )
{
    ui_context.activeFont = font;

    return ui_context.activeFont;
}

//--------------------------------------------------------------------------------------------
// Behaviors
ui_buttonValues ui_buttonBehavior( ui_id_t id, float vx, float vy, float vwidth, float vheight )
{
    ui_buttonValues result = BUTTON_NOCHANGE;

    // If the mouse is over the button, try and set hotness so that it can be cursor_clicked
    if ( ui_mouseInside( vx, vy, vwidth, vheight ) )
    {
        ui_sethot( id );
    }

    // Check to see if the button gets cursor_clicked on
    if ( ui_context.active == id )
    {
        if ( ui_context.cursor_Released == 1 )
        {
            if ( ui_context.hot == id ) result = BUTTON_UP;

            ui_setactive( UI_Nothing );
        }
    }
    else if ( ui_context.hot == id )
    {
        if ( ui_context.cursor_Pressed == 1 )
        {
            if ( ui_context.hot == id ) result = BUTTON_DOWN;

            ui_setactive( id );
        }
    }

    return result;
}

//--------------------------------------------------------------------------------------------
ui_buttonValues ui_WidgetBehavior( ui_Widget_t * pWidget )
{
    ui_buttonValues result = BUTTON_NOCHANGE;

    // If the mouse is over the button, try and set hotness so that it can be cursor_clicked
    if ( ui_mouseInside( pWidget->vx, pWidget->vy, pWidget->vwidth, pWidget->vheight ) )
    {
        ui_setWidgethot( pWidget );
    }

    // Check to see if the button gets cursor_clicked on
    if ( ui_context.active == pWidget->id )
    {
        if ( ui_context.cursor_Released == 1 )
        {
            // mouse button up
            if ( ui_context.active == pWidget->id ) result = BUTTON_UP;

            ui_setWidgetactive( NULL );
        }
    }
    else if ( ui_context.hot == pWidget->id )
    {
        if ( ui_context.cursor_Pressed == 1 )
        {
            // mouse button down
            if ( ui_context.hot == pWidget->id ) result = BUTTON_DOWN;

            ui_setWidgetactive( pWidget );
        }
    }

    return result;
}

//--------------------------------------------------------------------------------------------
// Drawing
void ui_drawButton( ui_id_t id, float vx, float vy, float vwidth, float vheight, GLXvector4f pcolor )
{
    float x1, x2, y1, y2;

    GLXvector4f color_1 = { 0.0f, 0.0f, 0.9f, 0.6f };
    GLXvector4f color_2 = { 0.54f, 0.0f, 0.0f, 1.0f };
    GLXvector4f color_3 = { 0.66f, 0.0f, 0.0f, 0.6f };

    // Draw the button
    GL_DEBUG( glDisable )( GL_TEXTURE_2D );

    if ( NULL == pcolor )
    {
        if ( ui_context.active != UI_Nothing && ui_context.active == id && ui_context.hot == id )
        {
            pcolor = color_1;
        }
        else if ( ui_context.hot != UI_Nothing && ui_context.hot == id )
        {
            pcolor = color_2;
        }
        else
        {
            pcolor = color_3;
        }
    }

    // convert the virtual coordinates to screen coordinates
    ui_virtual_to_screen_abs( vx, vy, &x1, &y1 );
    ui_virtual_to_screen_abs( vx + vwidth, vy + vheight, &x2, &y2 );

    GL_DEBUG( glColor4fv )( pcolor );
    GL_DEBUG( glBegin )( GL_QUADS );
    {
        GL_DEBUG( glVertex2f )( x1, y1 );
        GL_DEBUG( glVertex2f )( x1, y2 );
        GL_DEBUG( glVertex2f )( x2, y2 );
        GL_DEBUG( glVertex2f )( x2, y1 );
    }
    GL_DEBUG_END();

    GL_DEBUG( glEnable )( GL_TEXTURE_2D );
}

//--------------------------------------------------------------------------------------------
void ui_drawImage( ui_id_t id, oglx_texture_t *img, float vx, float vy, float vwidth, float vheight, GLXvector4f image_tint )
{
    GLXvector4f tmp_tint = {1, 1, 1, 1};

    float vw, vh;
    float tx, ty;
    float x1, x2, y1, y2;

    // handle optional parameters
    if ( NULL == image_tint ) image_tint = tmp_tint;

    if ( img )
    {
        if ( vwidth == 0 || vheight == 0 )
        {
            vw = img->imgW;
            vh = img->imgH;
        }
        else
        {
            vw = vwidth;
            vh = vheight;
        }

        tx = ( float ) oglx_texture_GetImageWidth( img )  / ( float ) oglx_texture_GetTextureWidth( img );
        ty = ( float ) oglx_texture_GetImageHeight( img ) / ( float ) oglx_texture_GetTextureHeight( img );

        // convert the virtual coordinates to screen coordinates
        ui_virtual_to_screen_abs( vx, vy, &x1, &y1 );
        ui_virtual_to_screen_abs( vx + vw, vy + vh, &x2, &y2 );

        // Draw the image
        oglx_texture_Bind( img );

        GL_DEBUG( glColor4fv )( image_tint );

        GL_DEBUG( glBegin )( GL_QUADS );
        {
            GL_DEBUG( glTexCoord2f )( 0,  0 );  GL_DEBUG( glVertex2f )( x1, y1 );
            GL_DEBUG( glTexCoord2f )( tx,  0 );  GL_DEBUG( glVertex2f )( x2, y1 );
            GL_DEBUG( glTexCoord2f )( tx, ty );  GL_DEBUG( glVertex2f )( x2, y2 );
            GL_DEBUG( glTexCoord2f )( 0, ty );  GL_DEBUG( glVertex2f )( x1, y2 );
        }
        GL_DEBUG_END();
    }
}

//--------------------------------------------------------------------------------------------
void ui_drawWidgetButton( ui_Widget_t * pw )
{
    GLfloat * pcolor = NULL;
    bool_t bactive, bhot;

    bactive = ui_context.active == pw->id && ui_context.hot == pw->id;
    bactive = bactive || 0 != ( pw->mask & pw->state & UI_BITS_CLICKED );
    bhot    = ui_context.hot == pw->id;
    bhot    = bhot || 0 != ( pw->mask & pw->state & UI_BITS_MOUSEOVER );

    if ( 0 != pw->mask )
    {
        if ( bactive )
        {
            pcolor = ui_normal_color2;
        }
        else if ( bhot )
        {
            pcolor = ui_hot_color;
        }
        else
        {
            pcolor = ui_normal_color;
        }
    }
    else
    {
        if ( bactive )
        {
            pcolor = ui_active_color;
        }
        else if ( bhot )
        {
            pcolor = ui_hot_color;
        }
        else
        {
            pcolor = ui_normal_color;
        }
    }

    ui_drawButton( pw->id, pw->vx, pw->vy, pw->vwidth, pw->vheight, pcolor );
}

//--------------------------------------------------------------------------------------------
void ui_drawWidgetImage( ui_Widget_t * pw )
{
    if ( NULL != pw && NULL != pw->img )
    {
        ui_drawImage( pw->id, pw->img, pw->vx, pw->vy, pw->vwidth, pw->vheight, NULL );
    }
}

//--------------------------------------------------------------------------------------------
/** ui_drawTextBox
 * Draws a text string into a box, splitting it into lines according to newlines in the string.
 * @warning Doesn't pay attention to the width/height arguments yet.
 *
 * text    - The text to draw
 * x       - The x position to start drawing at
 * y       - The y position to start drawing at
 * width   - Maximum width of the box (not implemented)
 * height  - Maximum height of the box (not implemented)
 * spacing - Amount of space to move down between lines. (usually close to your font size)
 */
void ui_drawTextBox( Font * font, const char *text, float vx, float vy, float vwidth, float vheight, float vspacing )
{
    float x1, x2, y1, y2;
    float spacing;

    if ( NULL == font ) font = ui_getFont();

    // convert the virtual coordinates to screen coordinates
    ui_virtual_to_screen_abs( vx, vy, &x1, &y1 );
    ui_virtual_to_screen_abs( vx + vwidth, vy + vheight, &x2, &y2 );
    spacing = ui_context.ah * vspacing;

    // draw using screen coordinates
    fnt_drawTextBox( font, NULL, x1, y1, x2 - x1, y2 - y1, spacing, text );
}

//--------------------------------------------------------------------------------------------
// Controls
ui_buttonValues ui_doButton( ui_id_t id, const char *text, Font * font, float vx, float vy, float vwidth, float vheight )
{
    ui_buttonValues result;
    int text_w, text_h;
    int text_x, text_y;

    // Do all the logic type work for the button
    result = ui_buttonBehavior( id, vx, vy, vwidth, vheight );

    // Draw the button part of the button
    ui_drawButton( id, vx, vy, vwidth, vheight, NULL );

    // And then draw the text that goes on top of the button
    if ( NULL == font ) font = ui_getFont();
    if ( NULL != font && NULL != text && '\0' != text[0] )
    {
        float x1, x2, y1, y2;

        // convert the virtual coordinates to screen coordinates
        ui_virtual_to_screen_abs( vx, vy, &x1, &y1 );
        ui_virtual_to_screen_abs( vx + vwidth, vy + vheight, &x2, &y2 );

        // find the vwidth & vheight of the text to be drawn, so that it can be centered inside
        // the button
        fnt_getTextSize( font, text, &text_w, &text_h );

        text_x = (( x2 - x1 ) - text_w ) / 2 + x1;
        text_y = (( y2 - y1 ) - text_h ) / 2 + y1;

        GL_DEBUG( glColor3f )( 1, 1, 1 );
        fnt_drawText( font, NULL, text_x, text_y, text );
    }

    return result;
}

//--------------------------------------------------------------------------------------------
ui_buttonValues ui_doImageButton( ui_id_t id, oglx_texture_t *img, float vx, float vy, float vwidth, float vheight, GLXvector3f image_tint )
{
    ui_buttonValues result;

    // Do all the logic type work for the button
    result = ui_buttonBehavior( id, vx, vy, vwidth, vheight );

    // Draw the button part of the button
    ui_drawButton( id, vx, vy, vwidth, vheight, NULL );

    // And then draw the image on top of it
    ui_drawImage( id, img, vx + 5, vy + 5, vwidth - 10, vheight - 10, image_tint );

    return result;
}

//--------------------------------------------------------------------------------------------
ui_buttonValues ui_doImageButtonWithText( ui_id_t id, oglx_texture_t *img, const char *text, Font * font, float vx, float vy, float vwidth, float vheight )
{
    ui_buttonValues result;

    float text_x, text_y;
    int   text_w, text_h;

    // Do all the logic type work for the button
    result = ui_buttonBehavior( id, vx, vy, vwidth, vheight );

    // Draw the button part of the button
    ui_drawButton( id, vx, vy, vwidth, vheight, NULL );

    // Draw the image part
    ui_drawImage( id, img, vx + 5, vy + 5, 0, 0, NULL );

    // And draw the text next to the image
    // And then draw the text that goes on top of the button
    if ( NULL == font ) font = ui_getFont();
    if ( NULL != font )
    {
        float x1, x2, y1, y2;

        // convert the virtual coordinates to screen coordinates
        ui_virtual_to_screen_abs( vx, vy, &x1, &y1 );
        ui_virtual_to_screen_abs( vx + vwidth, vy + vheight, &x2, &y2 );

        // find the vwidth & vheight of the text to be drawn, so that it can be centered inside
        // the button
        fnt_getTextSize( font, text, &text_w, &text_h );

        text_x = ( img->imgW + 10 ) * ui_context.aw + x1;
        text_y = (( y2 - y1 ) - text_h ) / 2         + y1;

        GL_DEBUG( glColor3f )( 1, 1, 1 );
        fnt_drawText( font, NULL, text_x, text_y, text );
    }

    return result;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
ui_buttonValues ui_doWidget( ui_Widget_t * pw )
{
    ui_buttonValues result;

    float text_x, text_y;
    int   text_w, text_h;
    float img_w;

    // Do all the logic type work for the button
    result = ui_WidgetBehavior( pw );

    // Draw the button part of the button
    ui_drawWidgetButton( pw );

    // draw any image on the left hand side of the button
    img_w = 0;
    if ( NULL != pw->img )
    {
        ui_Widget_t wtmp;

        // Draw the image part
        GL_DEBUG( glColor3f )( 1, 1, 1 );

        ui_shrinkWidget( &wtmp, pw, 5 );
        wtmp.vwidth = wtmp.vheight;

        ui_drawWidgetImage( &wtmp );

        // get the non-virtual image width
        img_w = pw->img->imgW * ui_context.aw;
    }

    // And draw the text on the right hand side of any image
    if ( NULL != pw->pfont && NULL != pw->text && '\0' != pw->text[0] )
    {
        float x1, x2, y1, y2;

        // convert the virtual coordinates to screen coordinates
        ui_virtual_to_screen_abs( pw->vx, pw->vy, &x1, &y1 );
        ui_virtual_to_screen_abs( pw->vx + pw->vwidth, pw->vy + pw->vheight, &x2, &y2 );

        GL_DEBUG( glColor3f )( 1, 1, 1 );

        // find the (x2-x1) & (y2-y1) of the pw->text to be drawn, so that it can be centered inside
        // the button
        fnt_getTextSize( pw->pfont, pw->text, &text_w, &text_h );

        text_w = MIN( text_w, ( x2 - x1 ) );
        text_h = MIN( text_h, ( y2 - y1 ) );

        text_x = (( x2 - x1 ) - text_w ) / 2 + x1;
        text_y = (( y2 - y1 ) - text_h ) / 2 + y1;

        text_x = img_w + (( x2 - x1 ) - img_w - text_w ) / 2 + x1;
        text_y = (( y2 - y1 ) - text_h ) / 2                + y1;

        GL_DEBUG( glColor3f )( 1, 1, 1 );
        fnt_drawText( pw->pfont, &( pw->text_surf ), text_x, text_y, pw->text );
    }

    return result;
}

//--------------------------------------------------------------------------------------------
bool_t ui_copyWidget( ui_Widget_t * pw2, ui_Widget_t * pw1 )
{
    if ( NULL == pw2 || NULL == pw1 ) return bfalse;
    return NULL != memcpy( pw2, pw1, sizeof( ui_Widget_t ) );
}

//--------------------------------------------------------------------------------------------
bool_t ui_shrinkWidget( ui_Widget_t * pw2, ui_Widget_t * pw1, float pixels )
{
    if ( NULL == pw2 || NULL == pw1 ) return bfalse;

    if ( !ui_copyWidget( pw2, pw1 ) ) return bfalse;

    pw2->vx += pixels;
    pw2->vy += pixels;
    pw2->vwidth  -= 2 * pixels;
    pw2->vheight -= 2 * pixels;

    if ( pw2->vwidth < 0 )  pw2->vwidth   = 0;
    if ( pw2->vheight < 0 ) pw2->vheight = 0;

    return pw2->vwidth > 0 && pw2->vheight > 0;
}

//--------------------------------------------------------------------------------------------
bool_t ui_initWidget( ui_Widget_t * pw, ui_id_t id, Font * pfont, const char *text, oglx_texture_t *img, float vx, float vy, float vwidth, float vheight )
{
    if ( NULL == pw ) return bfalse;

    if ( NULL == pfont ) pfont = ui_getFont();

    pw->id      = id;
    pw->pfont   = pfont;
    pw->text    = text;
    pw->img     = img;
    pw->vx      = vx;
    pw->vy      = vy;
    pw->vwidth  = vwidth;
    pw->vheight = vheight;
    pw->state   = 0;
    pw->mask    = 0;
    pw->timeout = 0;

    return btrue;
}

//--------------------------------------------------------------------------------------------
bool_t ui_widgetAddMask( ui_Widget_t * pw, BIT_FIELD mbits )
{
    if ( NULL == pw ) return bfalse;

    ADD_BITS( pw->mask, mbits );
    REMOVE_BITS( pw->state, mbits );

    return btrue;
}

//--------------------------------------------------------------------------------------------
bool_t ui_widgetRemoveMask( ui_Widget_t * pw, BIT_FIELD mbits )
{
    if ( NULL == pw ) return bfalse;

    REMOVE_BITS( pw->mask, mbits );
    REMOVE_BITS( pw->state, mbits );

    return btrue;
}

//--------------------------------------------------------------------------------------------
bool_t ui_widgetSetMask( ui_Widget_t * pw, BIT_FIELD mbits )
{
    if ( NULL == pw ) return bfalse;

    pw->mask  = mbits;
    REMOVE_BITS( pw->state, mbits );

    return btrue;
}

//--------------------------------------------------------------------------------------------
void ui_virtual_to_screen_abs( float vx, float vy, float * rx, float * ry )
{
    /// @details BB@> convert "virtual" screen positions into "real" space

    *rx = ui_context.aw * vx + ui_context.bw;
    *ry = ui_context.ah * vy + ui_context.bh;
}

//--------------------------------------------------------------------------------------------
void ui_screen_to_virtual_abs( float rx, float ry, float *vx, float *vy )
{
    /// @details BB@> convert "real" mouse positions into "virtual" space

    *vx = ui_context.iaw * rx + ui_context.ibw;
    *vy = ui_context.iah * ry + ui_context.ibh;
}

//--------------------------------------------------------------------------------------------
void ui_virtual_to_screen_rel( float vx, float vy, float * rx, float * ry )
{
    /// @details BB@> convert "virtual" screen positions into "real" space

    *rx = ui_context.aw * vx;
    *ry = ui_context.ah * vy;
}

//--------------------------------------------------------------------------------------------
void ui_screen_to_virtual_rel( float rx, float ry, float *vx, float *vy )
{
    /// @details BB@> convert "real" mouse positions into "virtual" space

    *vx = ui_context.iaw * rx;
    *vy = ui_context.iah * ry;
}


//--------------------------------------------------------------------------------------------
void ui_set_virtual_screen( float vw, float vh, float ww, float wh )
{
    /// @details BB@> set up the ui's virtual screen

    float k;
    Font * old_defaultFont;

    // define the virtual screen
    ui_context.vw = vw;
    ui_context.vh = vh;
    ui_context.ww = ww;
    ui_context.wh = wh;

    // define the forward transform
    k = MIN( sdl_scr.x / ww, sdl_scr.y / wh );
    ui_context.aw = k;
    ui_context.ah = k;
    ui_context.bw = ( sdl_scr.x - k * ww ) * 0.5f;
    ui_context.bh = ( sdl_scr.y - k * wh ) * 0.5f;

    // define the inverse transform
    ui_context.iaw = 1.0f / ui_context.aw;
    ui_context.iah = 1.0f / ui_context.ah;
    ui_context.ibw = -ui_context.bw * ui_context.iaw;
    ui_context.ibh = -ui_context.bh * ui_context.iah;

    // make sure the font is sized right for the virtual screen
    old_defaultFont = ui_context.defaultFont;
    if ( NULL != ui_context.defaultFont )
    {
        fnt_freeFont( ui_context.defaultFont );
    }
    ui_context.defaultFont = ui_loadFont( ui_context.defaultFontName, ui_context.defaultFontSize );

    // fix the active font. in general, we do not own it, so do not delete
    if ( NULL == ui_context.activeFont || old_defaultFont == ui_context.activeFont )
    {
        ui_context.activeFont = ui_context.defaultFont;
    }
}

//--------------------------------------------------------------------------------------------
Font * ui_loadFont( const char * font_name, float vpointSize )
{
    float pointSize;

    pointSize = vpointSize * ui_context.aw;

    return fnt_loadFont( font_name, pointSize );
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void ui_joy_init()
{
    int cnt;

    for( cnt = 0; cnt < UI_MAX_JOYSTICKS; cnt++ )
    {
        memset( ui_context.joys + cnt, 0, sizeof(ui_control_info_t) );
    }

    memset( &(ui_context.joy), 0, sizeof(ui_control_info_t) );

    memset( &(ui_context.mouse), 0, sizeof(ui_control_info_t) );
}

//--------------------------------------------------------------------------------------------
void ui_cursor_update()
{
    int   cnt;

    ui_control_info_t * pctrl = NULL;

    // assume no one is in control
    pctrl = NULL;

    // find the best most controlling joystick
    for( cnt = 0; cnt < UI_MAX_JOYSTICKS; cnt++ )
    {
        ui_control_info_t * pinfo = ui_context.joys + cnt;

        if( pinfo->timer <= 0 ) continue;

        if( (NULL == pctrl) || (pinfo->timer > pctrl->timer) )
        {
            pctrl = pinfo;
        }
    }

    // update the ui_context.joy device
    if( NULL != pctrl )
    {
        ui_context.joy.timer   = pctrl->timer;

        ui_context.joy.vrt[0] += pctrl->vrt[0];
        ui_context.joy.vrt[1] += pctrl->vrt[1];
        ui_virtual_to_screen_abs( ui_context.joy.vrt[0], ui_context.joy.vrt[1], &(ui_context.joy.scr[0]), &(ui_context.joy.scr[1]) );

        pctrl = &(ui_context.joy);
    }

    // find out whether the mouse or the joystick is the better controller
    if( NULL == pctrl )
    {
        pctrl = &(ui_context.mouse);
    }
    else if( ui_context.mouse.timer >  pctrl->timer )
    {
        pctrl = &(ui_context.mouse);
    }

    if( NULL != pctrl )
    {
        ui_context.cursor_X = 0.5f * ui_context.cursor_X + 0.5f * pctrl->vrt[0];
        ui_context.cursor_Y = 0.5f * ui_context.cursor_Y + 0.5f * pctrl->vrt[1];
    }

    // decrement the joy timers
    for( cnt = 0; cnt < UI_MAX_JOYSTICKS; cnt++ )
    {
        ui_control_info_t * pinfo = ui_context.joys + cnt;

        if( pinfo->timer > 0 )
        {
            pinfo->timer--;
        }
    }

    // decrement the mouse timer
    if( ui_context.mouse.timer > 0 )
    {
        ui_context.mouse.timer--;
    }

    // decrement the joy timer
    if( ui_context.joy.timer > 0 )
    {
        ui_context.joy.timer--;
    }

}

//--------------------------------------------------------------------------------------------
bool_t ui_joy_set(SDL_JoyAxisEvent * evt_ptr )
{
    const int   dead_zone = 0x8000 >> 4;
    const float sensitivity = 10.0f;

    ui_control_info_t * pctrl   = NULL;
    bool_t          updated = bfalse;
    int             value   = 0;

    if( NULL == evt_ptr || SDL_JOYAXISMOTION != evt_ptr->type ) return bfalse;
    value   = evt_ptr->value;

    // check the correct range of the events
    if( evt_ptr->which >= UI_MAX_JOYSTICKS ) return btrue;
    pctrl = ui_context.joys + evt_ptr->which;

    updated = bfalse;
    if( evt_ptr->axis < 2 )
    {
        float old_diff, new_diff;

        // make a dead zone
        if ( value > dead_zone ) value -= dead_zone;
        else if ( value < -dead_zone ) value += dead_zone;
        else value = 0;

        // update the info
        old_diff = pctrl->scr[evt_ptr->axis];
        new_diff = (float)value / ( float )( 0x8000 - dead_zone ) * sensitivity;
        pctrl->scr[evt_ptr->axis] = new_diff;

        updated = (old_diff != new_diff);
    }

    if( updated )
    {
        ui_screen_to_virtual_rel( pctrl->scr[0], pctrl->scr[1], &(pctrl->vrt[0]), &(pctrl->vrt[1]) );

        pctrl->timer = UI_CONTROL_TIMER;
    }

    return (NULL != pctrl) && (pctrl->timer > 0);
}
