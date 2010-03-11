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

/// @file bsp.h
/// @details

#include "bbox.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
struct s_BSP_aabb
{
    size_t      dim;
    float_ary_t mins;
    float_ary_t mids;
    float_ary_t maxs;

#if defined(__cplusplus)
    s_BSP_aabb();
    s_BSP_aabb( size_t dim );
    ~s_BSP_aabb();
#endif
};
typedef struct s_BSP_aabb BSP_aabb_t;

BSP_aabb_t * BSP_aabb_ctor( BSP_aabb_t * pbb, size_t dim );
BSP_aabb_t * BSP_aabb_dtor( BSP_aabb_t * pbb );

bool_t       BSP_aabb_empty( BSP_aabb_t * psrc1 );
bool_t       BSP_aabb_clear( BSP_aabb_t * psrc1 );
bool_t       BSP_aabb_lhs_contains_rhs( BSP_aabb_t * psrc1, BSP_aabb_t * psrc2 );
bool_t       BSP_aabb_overlap( BSP_aabb_t * psrc1, BSP_aabb_t * psrc2 );

bool_t       BSP_aabb_from_oct_bb( BSP_aabb_t * pdst, oct_bb_t * psrc );

#define BSP_AABB_INIT_VALUES { 0, DYNAMIC_ARY_INIT_VALS, DYNAMIC_ARY_INIT_VALS }

//--------------------------------------------------------------------------------------------
struct s_BSP_leaf
{
    struct s_BSP_leaf * next;
    int                 data_type;
    void              * data;
    size_t              index;

    BSP_aabb_t          bbox;

#if defined(__cplusplus)
    s_BSP_leaf();
    ~s_BSP_leaf();
#endif
};
typedef struct s_BSP_leaf BSP_leaf_t;

BSP_leaf_t * BSP_leaf_create( int dim, void * data, int type );
bool_t       BSP_leaf_destroy( BSP_leaf_t ** ppleaf );
BSP_leaf_t * BSP_leaf_ctor( BSP_leaf_t * t, int dim, void * data, int type );
bool_t       BSP_leaf_dtor( BSP_leaf_t * t );

//--------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_ARY( BSP_leaf_ary, BSP_leaf_t )
DECLARE_DYNAMIC_ARY( BSP_leaf_pary, BSP_leaf_t * )

//--------------------------------------------------------------------------------------------
struct s_BSP_branch
{
    struct s_BSP_branch  * parent;

    size_t                 child_count;
    struct s_BSP_branch ** child_lst;

    size_t                 node_count;
    BSP_leaf_t           * node_lst;

    BSP_aabb_t             bbox;
    int                    depth;

#if defined(__cplusplus)
    s_BSP_branch();
    s_BSP_branch( size_t dim );
    ~s_BSP_branch();
#endif
};
typedef struct s_BSP_branch BSP_branch_t;

BSP_branch_t * BSP_branch_create( size_t dim );
bool_t         BSP_branch_destroy( BSP_branch_t ** ppbranch );
BSP_branch_t * BSP_branch_create_ary( size_t ary_size, size_t dim );
bool_t         BSP_branch_destroy_ary( size_t ary_size, BSP_branch_t ** ppbranch );
BSP_branch_t * BSP_branch_ctor( BSP_branch_t * B, size_t dim );
bool_t         BSP_branch_dtor( BSP_branch_t * B );
bool_t         BSP_branch_empty( BSP_branch_t * pbranch );

bool_t         BSP_branch_insert_leaf( BSP_branch_t * B, BSP_leaf_t * n );
bool_t         BSP_branch_insert_branch( BSP_branch_t * B, int index, BSP_branch_t * B2 );
bool_t         BSP_branch_clear_nodes( BSP_branch_t * B, bool_t recursive );
bool_t         BSP_branch_free_nodes( BSP_branch_t * B, bool_t recursive );
bool_t         BSP_branch_unlink( BSP_branch_t * B );
bool_t         BSP_branch_add_all_nodes( BSP_branch_t * pbranch, BSP_leaf_pary_t * colst );

//--------------------------------------------------------------------------------------------
DECLARE_DYNAMIC_ARY( BSP_branch_ary, BSP_branch_t )
DECLARE_DYNAMIC_ARY( BSP_branch_pary, BSP_branch_t * )

//--------------------------------------------------------------------------------------------
struct s_BSP_tree
{
    size_t dimensions;
    int    depth;

    BSP_branch_ary_t  branch_all;
    BSP_branch_pary_t branch_used;
    BSP_branch_pary_t branch_free;

    BSP_branch_t    * root;

    size_t            infinite_count;
    BSP_leaf_t      * infinite;

    BSP_aabb_t        bbox;

#if defined(__cplusplus)
    s_BSP_tree();
    s_BSP_tree( Sint32 dim, Sint32 depth );
    ~s_BSP_tree();
#endif
};
typedef struct s_BSP_tree BSP_tree_t;

#define BSP_TREE_INIT_VALS                                               \
    {                                                                    \
        0,                     /* size_t              dimensions     */  \
        0,                     /* int                 depth          */  \
        DYNAMIC_ARY_INIT_VALS, /* BSP_branch_ary_t    branch_all     */  \
        DYNAMIC_ARY_INIT_VALS, /* BSP_branch_pary_t * branch_all     */  \
        DYNAMIC_ARY_INIT_VALS, /* BSP_branch_pary_t * branch_free    */  \
        NULL,                  /* BSP_branch_t      * root           */  \
        0,                     /* size_t              infinite_count */  \
        NULL,                  /* BSP_leaf_t        * infinite       */  \
        BSP_AABB_INIT_VALUES   /* BSP_aabb_t bbox                    */  \
    }

BSP_tree_t * BSP_tree_create( size_t count );
bool_t       BSP_tree_destroy( BSP_tree_t ** ptree );

BSP_tree_t * BSP_tree_ctor( BSP_tree_t * t, Sint32 dim, Sint32 depth );
BSP_tree_t * BSP_tree_dtor( BSP_tree_t * t );
bool_t       BSP_tree_alloc( BSP_tree_t * t, size_t count, size_t dim );
bool_t       BSP_tree_free( BSP_tree_t * t );
bool_t       BSP_tree_init_0( BSP_tree_t * t );
//BSP_tree_t * BSP_tree_init_1( BSP_tree_t * t, Sint32 dim, Sint32 depth );

bool_t         BSP_tree_clear_nodes( BSP_tree_t * t, bool_t recursive );
bool_t         BSP_tree_free_nodes( BSP_tree_t * t, bool_t recursive );
bool_t         BSP_tree_free_all( BSP_tree_t * t );
bool_t         BSP_tree_prune( BSP_tree_t * t );
BSP_branch_t * BSP_tree_get_free( BSP_tree_t * t );
bool_t         BSP_tree_add_free( BSP_tree_t * t, BSP_branch_t * B );
BSP_branch_t * BSP_tree_ensure_root( BSP_tree_t * t );
BSP_branch_t * BSP_tree_ensure_branch( BSP_tree_t * t, BSP_branch_t * B, int index );
Sint32         BSP_tree_count_nodes( Sint32 dim, Sint32 depth );
bool_t         BSP_tree_insert( BSP_tree_t * t, BSP_branch_t * B, BSP_leaf_t * n, int index );
bool_t         BSP_tree_insert_leaf( BSP_tree_t * ptree, BSP_leaf_t * pleaf );
bool_t         BSP_tree_prune_branch( BSP_tree_t * t, int cnt );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
bool_t BSP_generate_aabb( BSP_aabb_t * psrc, int index, BSP_aabb_t * pdst );
int    BSP_tree_collide( BSP_tree_t * tree, BSP_aabb_t * paabb, BSP_leaf_pary_t * colst );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#define Egoboo_bsp_h

