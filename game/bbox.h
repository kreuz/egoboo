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

///
/// @file bbox.h
/// @brief A small "library" for dealing with various bounding boxes

#include "egoboo_math.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

/// axis aligned bounding box
struct ego_aabb
{
    float mins[3];
    float maxs[3];
};

//--------------------------------------------------------------------------------------------

/// Level 0 character "bumper"
/// The simplest collision volume, equivalent to the old-style collision data
/// stored in data.txt
struct ego_bumper
{
    float  size;        ///< Size of bumpers
    float  size_big;     ///< For octagonal bumpers
    float  height;      ///< Distance from head to toe
};

//--------------------------------------------------------------------------------------------

/// The various axes for the octagonal bounding box
enum e_octagonal_axes
{
    OCT_X, OCT_Y, OCT_XY, OCT_YX, OCT_Z, OCT_COUNT
};

/// a "vector" that measures distances based on the axes of an octagonal bounding box
typedef float oct_vec_t[OCT_COUNT];

bool_t oct_vec_ctor( oct_vec_t ovec, fvec3_t pos );

#define OCT_VEC_INIT_VALS { 0,0,0,0,0 }

//--------------------------------------------------------------------------------------------

/// generic octagonal bounding box
/// to be used for the Level 1 character "bumper"
/// The best possible octagonal bounding volume. A generalization of the old octagonal bounding box
/// values in data.txt. Computed on the fly.

struct ego_oct_bb
{
    oct_vec_t mins,  maxs;

    ego_oct_bb() { memset( this, 0, sizeof( *this ) ); }

    static ego_oct_bb * ctor( ego_oct_bb   * pobb );
    static bool_t       do_union( ego_oct_bb   src1, ego_oct_bb   src2, ego_oct_bb   * pdst );
    static bool_t       do_intersection( ego_oct_bb   src1, ego_oct_bb   src2, ego_oct_bb   * pdst );
    static bool_t       empty( ego_oct_bb   src1 );

    static void         downgrade( ego_oct_bb * psrc, ego_bumper bump_stt, ego_bumper bump_base, ego_bumper * p_bump, ego_oct_bb   * pdst );
    static bool_t       add_vector( const ego_oct_bb src, const fvec3_base_t vec, ego_oct_bb   * pdst );
};

#define OCT_BB_INIT_VALS { OCT_VEC_INIT_VALS, OCT_VEC_INIT_VALS }

//--------------------------------------------------------------------------------------------
struct ego_lod_aabb
{
    int    sub_used;
    float  weight;

    bool_t used;
    int    level;
    int    address;

    ego_aabb  bb;

    ego_lod_aabb() { memset( this, 0, sizeof( *this ) ); }
};

//--------------------------------------------------------------------------------------------
struct ego_aabb_lst
{
    int       count;
    ego_lod_aabb * list;

    ego_aabb_lst();
    ~ego_aabb_lst();

    static EGO_CONST ego_aabb_lst   * ctor( ego_aabb_lst   * lst );
    static EGO_CONST ego_aabb_lst   * dtor( ego_aabb_lst   * lst );
    static EGO_CONST ego_aabb_lst   * renew( ego_aabb_lst   * lst );
    static EGO_CONST ego_aabb_lst   * alloc( ego_aabb_lst   * lst, int count );
};

//--------------------------------------------------------------------------------------------
struct ego_aabb_ary
{
    int         count;
    ego_aabb_lst   * list;

    static EGO_CONST ego_aabb_ary * ctor( ego_aabb_ary * ary );
    static EGO_CONST ego_aabb_ary * dtor( ego_aabb_ary * ary );
    static EGO_CONST ego_aabb_ary * renew( ego_aabb_ary * ary );
    static EGO_CONST ego_aabb_ary * alloc( ego_aabb_ary * ary, int count );
};

//--------------------------------------------------------------------------------------------

/// @details A convex poly representation of an object volume
struct ego_OVolume
{
    int      lod;             ///< the level of detail (LOD) of this volume
    bool_t   needs_shape;     ///< is the shape data valid?
    bool_t   needs_position;  ///< Is the position data valid?

    ego_oct_bb   oct;

    static ego_OVolume do_merge( ego_OVolume * pv1, ego_OVolume * pv2 );
    static ego_OVolume do_intersect( ego_OVolume * pv1, ego_OVolume * pv2 );
    static bool_t      draw( ego_OVolume * cv, bool_t draw_square, bool_t draw_diamond );
    static bool_t      shift( ego_OVolume * cv_src, fvec3_t * pos_src, ego_OVolume *cv_dst );
    static bool_t      unshift( ego_OVolume * cv_src, fvec3_t * pos_src, ego_OVolume *cv_dst );
    static bool_t      refine( ego_OVolume * pov, fvec3_t * pcenter, float * pvolume );
};

//--------------------------------------------------------------------------------------------
struct ego_OTree 
{ 
    ego_OVolume leaf[8]; 
};

//--------------------------------------------------------------------------------------------

/// @details A convex polygon representation of the collision of two objects
struct ego_CVolume
{
    float          volume;
    fvec3_t        center;
    ego_OVolume    ov;
    ego_OTree    * tree;

    static bool_t ctor( ego_CVolume * pcv, ego_OVolume * pva, ego_OVolume * pvb );
    static bool_t refine( ego_CVolume * pcv );
};

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// type conversion routines

bool_t bumper_to_oct_bb_0( ego_bumper src, ego_oct_bb   * pdst );
bool_t bumper_to_oct_bb_1( ego_bumper src, fvec3_t vel, ego_oct_bb   * pdst );


int    oct_bb_to_points( ego_oct_bb   * pbmp, fvec4_t pos[], size_t pos_count );
void   points_to_oct_bb( ego_oct_bb   * pbmp, fvec4_t pos[], size_t pos_count );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#define Egoboo_bbox_h