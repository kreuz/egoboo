#pragma once
//********************************************************************************************
//*
//*    This file is part of the opengl extensions library. This library is
//*    distributed with Egoboo.
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

/// \defgroup _ogl_extensions_ Extensions to OpenGL

/// \file extensions/ogl_texture.h
/// \ingroup _ogl_extensions_
/// \brief Definitions for OpenGL texture loading using SDL_image
/// \details

#include "ogl_include.h"
#include "ogl_debug.h"
#include "egoboo_typedef.h"

#include "ogl_extensions.h"

#include <SDL.h>

#if defined(__cplusplus)
extern "C"
{
#endif

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#define INVALID_KEY    ( (Uint32) (~0) )

#define VALID_BINDING( BIND ) ( (0 != (BIND)) && (INVALID_GL_ID != (BIND)) )
#define ERROR_IMAGE_BINDING( BIND ) ( ErrorImage_get_binding() == (BIND) )

/// OpenGL Texture filtering methods
    typedef enum e_tx_filters
    {
        TX_UNFILTERED,
        TX_LINEAR,
        TX_MIPMAP,
        TX_BILINEAR,
        TX_TRILINEAR_1,
        TX_TRILINEAR_2,
        TX_ANISOTROPIC,
        TX_FILTER_COUNT
    }
    TX_FILTERS;

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

    struct s_oglx_texture
    {
        GLboolean    base_valid;
        gl_texture_t base;

        GLuint        valid;           ///< whether or not the texture has been initialized
        char          name[256];       ///< the name of the original file
        SDL_Surface * surface;         ///< the original texture data
        int           imgW, imgH;      ///< the height & width of the texture data
        GLfloat       alpha;           ///< the alpha for the texture
    };
    typedef struct s_oglx_texture oglx_texture_t;

    GLuint  oglx_texture_Convert( oglx_texture_t *texture, SDL_Surface * pimage, const Uint32 key );
    GLuint  oglx_texture_Load( oglx_texture_t *texture, const char *filename, const Uint32 key );
    GLuint  oglx_texture_GetTextureID( const oglx_texture_t *texture );
    GLsizei oglx_texture_GetImageHeight( const oglx_texture_t *texture );
    GLsizei oglx_texture_GetImageWidth( const oglx_texture_t *texture );
    GLfloat oglx_texture_GetTextureWidth( const oglx_texture_t *texture );
    GLfloat oglx_texture_GetTextureHeight( const oglx_texture_t *texture );
    void    oglx_texture_SetAlpha( oglx_texture_t *texture, const GLfloat alpha );
    GLfloat oglx_texture_GetAlpha( const oglx_texture_t *texture );
    void    oglx_texture_Release( oglx_texture_t *texture );

    void    oglx_texture_Bind( oglx_texture_t * texture );

    oglx_texture_t * oglx_texture_ctor( oglx_texture_t * texture );
    void             oglx_texture_dtor( oglx_texture_t * texture );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

    struct s_oglx_texture_parameters
    {
        TX_FILTERS texturefilter;
        GLfloat    userAnisotropy;
    };
    typedef struct s_oglx_texture_parameters oglx_texture_parameters_t;

    extern oglx_texture_parameters_t tex_params;

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

    void      oglx_grab_texture_state( const GLenum target, const GLint level, oglx_texture_t * texture );
    GLboolean oglx_texture_valid( oglx_texture_t *ptex );

    GLuint    oglx_bind_default( const GLuint binding, const GLenum target, const GLint wrap_s, const GLint wrap_t );
    void      oglx_default_filtering( GLenum target );

    void      ErrorImage_bind( const GLenum target, const GLuint id );
    GLuint    ErrorImage_get_binding();

#if defined(__cplusplus)
}
#endif
