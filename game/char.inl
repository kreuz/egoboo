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

/// @file char.inl
/// @note You will routinely include "char.inl" in all *.inl files or *.c/*.cpp files, instead of "char.h"

#include "char.h"

#include "enchant.inl"
#include "particle.inl"

/// @note include "profile.inl" here.
///  Do not include "char.inl" in "profile.inl", otherwise there is a bootstrapping problem.
#include "profile.inl"

#include "egoboo_math.inl"

//--------------------------------------------------------------------------------------------
// FORWARD DECLARARIONS
//--------------------------------------------------------------------------------------------
// cap_t accessor functions
INLINE bool_t cap_is_type_idsz( const CAP_REF by_reference icap, IDSZ test_idsz );
INLINE bool_t cap_has_idsz( const CAP_REF by_reference icap, IDSZ idsz );

//--------------------------------------------------------------------------------------------
// team_t accessor functions
INLINE CHR_REF  team_get_ileader( const TEAM_REF by_reference iteam );
INLINE chr_t  * team_get_pleader( const TEAM_REF by_reference iteam );

INLINE bool_t team_hates_team( const TEAM_REF by_reference ipredator_team, const TEAM_REF by_reference iprey_team );

//--------------------------------------------------------------------------------------------
// chr_t accessor functions
INLINE PRO_REF  chr_get_ipro( const CHR_REF by_reference ichr );
INLINE CAP_REF  chr_get_icap( const CHR_REF by_reference ichr );
INLINE EVE_REF  chr_get_ieve( const CHR_REF by_reference ichr );
INLINE PIP_REF  chr_get_ipip( const CHR_REF by_reference ichr, int ipip );
INLINE TEAM_REF chr_get_iteam( const CHR_REF by_reference ichr );
INLINE TEAM_REF chr_get_iteam_base( const CHR_REF by_reference ichr );

INLINE pro_t * chr_get_ppro( const CHR_REF by_reference ichr );
INLINE cap_t * chr_get_pcap( const CHR_REF by_reference ichr );
INLINE eve_t * chr_get_peve( const CHR_REF by_reference ichr );
INLINE pip_t * chr_get_ppip( const CHR_REF by_reference ichr, int ipip );

INLINE Mix_Chunk      * chr_get_chunk_ptr( chr_t * pchr, int index );
INLINE Mix_Chunk      * chr_get_chunk( const CHR_REF by_reference ichr, int index );
INLINE team_t         * chr_get_pteam( const CHR_REF by_reference ichr );
INLINE team_t         * chr_get_pteam_base( const CHR_REF by_reference ichr );
INLINE ai_state_t     * chr_get_pai( const CHR_REF by_reference ichr );
INLINE chr_instance_t * chr_get_pinstance( const CHR_REF by_reference ichr );

INLINE IDSZ chr_get_idsz( const CHR_REF by_reference ichr, int type );

INLINE void chr_update_size( chr_t * pchr );
INLINE void chr_init_size( chr_t * pchr, cap_t * pcap );
INLINE void chr_set_size( chr_t * pchr, float size );
INLINE void chr_set_width( chr_t * pchr, float width );
INLINE void chr_set_shadow( chr_t * pchr, float width );
INLINE void chr_set_height( chr_t * pchr, float height );
INLINE void chr_set_fat( chr_t * pchr, float fat );

INLINE bool_t chr_has_idsz( const CHR_REF by_reference ichr, IDSZ idsz );
INLINE bool_t chr_is_type_idsz( const CHR_REF by_reference ichr, IDSZ idsz );
INLINE bool_t chr_has_vulnie( const CHR_REF by_reference item, const PRO_REF by_reference weapon_profile );

INLINE bool_t chr_getMatUp( chr_t *pchr, fvec3_t   * pvec );
INLINE bool_t chr_getMatRight( chr_t *pchr, fvec3_t   * pvec );
INLINE bool_t chr_getMatForward( chr_t *pchr, fvec3_t   * pvec );
INLINE bool_t chr_getMatTranslate( chr_t *pchr, fvec3_t   * pvec );

//--------------------------------------------------------------------------------------------
// IMPLEMENTATION
//--------------------------------------------------------------------------------------------
INLINE bool_t cap_is_type_idsz( const CAP_REF by_reference icap, IDSZ test_idsz )
{
    /// @details BB@> check IDSZ_PARENT and IDSZ_TYPE to see if the test_idsz matches. If we are not
    ///     picky (i.e. IDSZ_NONE == test_idsz), then it matches any valid item.

    cap_t * pcap;

    if ( !LOADED_CAP( icap ) ) return bfalse;
    pcap = CapStack.lst + icap;

    if ( IDSZ_NONE == test_idsz ) return btrue;
    if ( test_idsz == pcap->idsz[IDSZ_TYPE  ] ) return btrue;
    if ( test_idsz == pcap->idsz[IDSZ_PARENT] ) return btrue;

    return bfalse;
}

//--------------------------------------------------------------------------------------------
INLINE bool_t cap_has_idsz( const CAP_REF by_reference icap, IDSZ idsz )
{
    /// @detalis BB@> does idsz match any of the stored values in pcap->idsz[]?
    ///               Matches anything if not picky (idsz == IDSZ_NONE)

    int     cnt;
    cap_t * pcap;
    bool_t  retval;

    if ( !LOADED_CAP( icap ) ) return bfalse;
    pcap = CapStack.lst + icap;

    if ( IDSZ_NONE == idsz ) return btrue;

    retval = bfalse;
    for ( cnt = 0; cnt < IDSZ_COUNT; cnt++ )
    {
        if ( pcap->idsz[cnt] == idsz )
        {
            retval = btrue;
            break;
        }
    }

    return retval;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE CHR_REF team_get_ileader( const TEAM_REF by_reference iteam )
{
    CHR_REF ichr;

    if ( iteam >= TEAM_MAX ) return ( CHR_REF )MAX_CHR;

    ichr = TeamStack.lst[iteam].leader;
    if ( !DEFINED_CHR( ichr ) ) return ( CHR_REF )MAX_CHR;

    return ichr;
}

//--------------------------------------------------------------------------------------------
INLINE chr_t  * team_get_pleader( const TEAM_REF by_reference iteam )
{
    CHR_REF ichr;

    if ( iteam >= TEAM_MAX ) return NULL;

    ichr = TeamStack.lst[iteam].leader;
    if ( !DEFINED_CHR( ichr ) ) return NULL;

    return ChrList.lst + ichr;
}

//--------------------------------------------------------------------------------------------
INLINE bool_t team_hates_team( const TEAM_REF by_reference ipredator_team, const TEAM_REF by_reference iprey_team )
{
    /// @details BB@> a wrapper function for access to the hatesteam data

    if ( ipredator_team >= TEAM_MAX || iprey_team >= TEAM_MAX ) return bfalse;

    return TeamStack.lst[ipredator_team].hatesteam[ REF_TO_INT( iprey_team )];
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE PRO_REF chr_get_ipro( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return ( PRO_REF )MAX_PROFILE;
    pchr = ChrList.lst + ichr;

    if ( !LOADED_PRO( pchr->iprofile ) ) return ( PRO_REF )MAX_PROFILE;

    return pchr->iprofile;
}

//--------------------------------------------------------------------------------------------
INLINE CAP_REF chr_get_icap( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return ( CAP_REF )MAX_CAP;
    pchr = ChrList.lst + ichr;

    return pro_get_icap( pchr->iprofile );
}

//--------------------------------------------------------------------------------------------
INLINE EVE_REF chr_get_ieve( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return ( EVE_REF )MAX_EVE;
    pchr = ChrList.lst + ichr;

    return pro_get_ieve( pchr->iprofile );
}

//--------------------------------------------------------------------------------------------
INLINE PIP_REF chr_get_ipip( const CHR_REF by_reference ichr, int ipip )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return ( PIP_REF )MAX_PIP;
    pchr = ChrList.lst + ichr;

    return pro_get_ipip( pchr->iprofile, ipip );
}

//--------------------------------------------------------------------------------------------
INLINE TEAM_REF chr_get_iteam( const CHR_REF by_reference ichr )
{
    chr_t * pchr;
    int iteam;

    if ( !DEFINED_CHR( ichr ) ) return ( TEAM_REF )TEAM_DAMAGE;
    pchr = ChrList.lst + ichr;

    iteam = REF_TO_INT( pchr->team );
    iteam = CLIP( iteam, 0, TEAM_MAX );

    return ( TEAM_REF )iteam;
}

//--------------------------------------------------------------------------------------------
INLINE TEAM_REF chr_get_iteam_base( const CHR_REF by_reference ichr )
{
    chr_t * pchr;
    int iteam;

    if ( !DEFINED_CHR( ichr ) ) return ( TEAM_REF )TEAM_MAX;
    pchr = ChrList.lst + ichr;

    iteam = REF_TO_INT( pchr->baseteam );
    iteam = CLIP( iteam, 0, TEAM_MAX );

    return ( TEAM_REF )iteam;
}

//--------------------------------------------------------------------------------------------
INLINE pro_t * chr_get_ppro( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return NULL;
    pchr = ChrList.lst + ichr;

    if ( !LOADED_PRO( pchr->iprofile ) ) return NULL;

    return ProList.lst + pchr->iprofile;
}

//--------------------------------------------------------------------------------------------
INLINE cap_t * chr_get_pcap( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return NULL;
    pchr = ChrList.lst + ichr;

    return pro_get_pcap( pchr->iprofile );
}

//--------------------------------------------------------------------------------------------
INLINE eve_t * chr_get_peve( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return NULL;
    pchr = ChrList.lst + ichr;

    return pro_get_peve( pchr->iprofile );
}

//--------------------------------------------------------------------------------------------
INLINE pip_t * chr_get_ppip( const CHR_REF by_reference ichr, int ipip )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return NULL;
    pchr = ChrList.lst + ichr;

    return pro_get_ppip( pchr->iprofile, ipip );
}

//--------------------------------------------------------------------------------------------
INLINE Mix_Chunk * chr_get_chunk( const CHR_REF by_reference ichr, int index )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return NULL;
    pchr = ChrList.lst + ichr;

    return pro_get_chunk( pchr->iprofile, index );
}

//--------------------------------------------------------------------------------------------
INLINE Mix_Chunk * chr_get_chunk_ptr( chr_t * pchr, int index )
{
    if ( !DEFINED_PCHR( pchr ) ) return NULL;

    return pro_get_chunk( pchr->iprofile, index );
}

//--------------------------------------------------------------------------------------------
INLINE team_t * chr_get_pteam( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return NULL;
    pchr = ChrList.lst + ichr;

    if ( pchr->team < 0 && pchr->team >= TEAM_MAX ) return NULL;

    return TeamStack.lst + pchr->team;
}

//--------------------------------------------------------------------------------------------
INLINE team_t * chr_get_pteam_base( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return NULL;
    pchr = ChrList.lst + ichr;

    if ( pchr->baseteam < 0 || pchr->baseteam >= TEAM_MAX ) return NULL;

    return TeamStack.lst + pchr->baseteam;
}

//--------------------------------------------------------------------------------------------
INLINE ai_state_t * chr_get_pai( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return NULL;
    pchr = ChrList.lst + ichr;

    return &( pchr->ai );
}

//--------------------------------------------------------------------------------------------
INLINE chr_instance_t * chr_get_pinstance( const CHR_REF by_reference ichr )
{
    chr_t * pchr;

    if ( !DEFINED_CHR( ichr ) ) return NULL;
    pchr = ChrList.lst + ichr;

    return &( pchr->inst );
}

//--------------------------------------------------------------------------------------------
INLINE IDSZ chr_get_idsz( const CHR_REF by_reference ichr, int type )
{
    cap_t * pcap;

    if ( type >= IDSZ_COUNT ) return IDSZ_NONE;

    pcap = chr_get_pcap( ichr );
    if ( NULL == pcap ) return IDSZ_NONE;

    return pcap->idsz[type];
}

//--------------------------------------------------------------------------------------------
INLINE bool_t chr_has_idsz( const CHR_REF by_reference ichr, IDSZ idsz )
{
    /// @detalis BB@> a wrapper for cap_has_idsz

    CAP_REF icap = chr_get_icap( ichr );

    return cap_has_idsz( icap, idsz );
}

//--------------------------------------------------------------------------------------------
INLINE bool_t chr_is_type_idsz( const CHR_REF by_reference item, IDSZ test_idsz )
{
    /// @details BB@> check IDSZ_PARENT and IDSZ_TYPE to see if the test_idsz matches. If we are not
    ///     picky (i.e. IDSZ_NONE == test_idsz), then it matches any valid item.

    CAP_REF icap;

    icap = chr_get_icap( item );

    return cap_is_type_idsz( icap, test_idsz );
}

//--------------------------------------------------------------------------------------------
INLINE bool_t chr_has_vulnie( const CHR_REF by_reference item, const PRO_REF by_reference test_profile )
{
    /// @detalis BB@> is item vulnerable to the type in profile test_profile?

    IDSZ vulnie;

    if ( !INGAME_CHR( item ) ) return bfalse;
    vulnie = chr_get_idsz( item, IDSZ_VULNERABILITY );

    // not vulnerable if there is no specific weakness
    if ( IDSZ_NONE == vulnie ) return bfalse;

    // check vs. every IDSZ that could have something to do with attacking
    if ( vulnie == pro_get_idsz( test_profile, IDSZ_TYPE ) ) return btrue;
    if ( vulnie == pro_get_idsz( test_profile, IDSZ_PARENT ) ) return btrue;

    return bfalse;
}

//--------------------------------------------------------------------------------------------
INLINE bool_t chr_getMatUp( chr_t *pchr, fvec3_t   *pvec )
{
    /// @details BB@> MAKE SURE the value it calculated relative to a valid matrix

    if ( !ALLOCATED_PCHR( pchr ) ) return bfalse;

    if ( NULL == pvec ) return bfalse;

    if ( !chr_matrix_valid( pchr ) )
    {
        chr_update_matrix( pchr, btrue );
    }

    if ( chr_matrix_valid( pchr ) )
    {
        ( *pvec ) = mat_getChrUp( pchr->inst.matrix );
    }
    else
    {
        ( *pvec ).x = ( *pvec ).y = 0.0f;
        ( *pvec ).z = 1.0f;
    }

    return btrue;
}

//--------------------------------------------------------------------------------------------
INLINE bool_t chr_getMatRight( chr_t *pchr, fvec3_t   *pvec )
{
    /// @details BB@> MAKE SURE the value it calculated relative to a valid matrix

    if ( !ALLOCATED_PCHR( pchr ) ) return bfalse;

    if ( NULL == pvec ) return bfalse;

    if ( !chr_matrix_valid( pchr ) )
    {
        chr_update_matrix( pchr, btrue );
    }

    if ( chr_matrix_valid( pchr ) )
    {
        ( *pvec ) = mat_getChrRight( pchr->inst.matrix );
    }
    else
    {
        // assume default Right is +y
        ( *pvec ).y = 1.0f;
        ( *pvec ).x = ( *pvec ).z = 0.0f;
    }

    return btrue;
}

//--------------------------------------------------------------------------------------------
INLINE bool_t chr_getMatForward( chr_t *pchr, fvec3_t   *pvec )
{
    /// @details BB@> MAKE SURE the value it calculated relative to a valid matrix

    if ( !ALLOCATED_PCHR( pchr ) ) return bfalse;

    if ( NULL == pvec ) return bfalse;

    if ( !chr_matrix_valid( pchr ) )
    {
        chr_update_matrix( pchr, btrue );
    }

    if ( chr_matrix_valid( pchr ) )
    {
        ( *pvec ) = mat_getChrForward( pchr->inst.matrix );
    }
    else
    {
        // assume default Forward is +x
        ( *pvec ).x = 1.0f;
        ( *pvec ).y = ( *pvec ).z = 0.0f;
    }

    return btrue;
}

//--------------------------------------------------------------------------------------------
INLINE bool_t chr_getMatTranslate( chr_t *pchr, fvec3_t   *pvec )
{
    /// @details BB@> MAKE SURE the value it calculated relative to a valid matrix

    if ( !ALLOCATED_PCHR( pchr ) ) return bfalse;

    if ( NULL == pvec ) return bfalse;

    if ( !chr_matrix_valid( pchr ) )
    {
        chr_update_matrix( pchr, btrue );
    }

    if ( chr_matrix_valid( pchr ) )
    {
        ( *pvec ) = mat_getTranslate( pchr->inst.matrix );
    }
    else
    {
        ( *pvec ) = pchr->pos;
    }

    return btrue;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE void chr_update_size( chr_t * pchr )
{
    /// @details BB@> Convert the base size values to the size values that are used in the game

    if ( !ACTIVE_PCHR( pchr ) ) return;

    pchr->shadow_size  = pchr->shadow_size_save  * pchr->fat;
    pchr->bump.size    = pchr->bump_save.size    * pchr->fat;
    pchr->bump.size_big = pchr->bump_save.size_big * pchr->fat;
    pchr->bump.height  = pchr->bump_save.height  * pchr->fat;

    chr_update_collision_size( pchr, btrue );
}

//--------------------------------------------------------------------------------------------
INLINE void chr_init_size( chr_t * pchr, cap_t * pcap )
{
    /// @details BB@> initalize the character size info

    if ( !ACTIVE_PCHR( pchr ) ) return;
    if ( NULL == pcap || !pcap->loaded ) return;

    pchr->fat               = pcap->size;
    pchr->shadow_size_save  = pcap->shadow_size;
    pchr->bump_save.size    = pcap->bump_size;
    pchr->bump_save.size_big = pcap->bump_sizebig;
    pchr->bump_save.height  = pcap->bump_height;

    chr_update_size( pchr );
}

//--------------------------------------------------------------------------------------------
INLINE void chr_set_size( chr_t * pchr, float size )
{
    /// @details BB@> scale the entire character so that the size matches the given value

    float ratio;

    if ( !DEFINED_PCHR( pchr ) ) return;

    ratio = size / pchr->bump.size;

    pchr->shadow_size_save  *= ratio;
    pchr->bump_save.size    *= ratio;
    pchr->bump_save.size_big *= ratio;
    pchr->bump_save.height  *= ratio;

    chr_update_size( pchr );
}

//--------------------------------------------------------------------------------------------
INLINE void chr_set_width( chr_t * pchr, float width )
{
    /// @details BB@> update the base character "width". This also modifies the shadow size

    float ratio;

    if ( !DEFINED_PCHR( pchr ) ) return;

    ratio = width / pchr->bump.size;

    pchr->shadow_size_save    *= ratio;
    pchr->bump_save.size    *= ratio;
    pchr->bump_save.size_big *= ratio;

    chr_update_size( pchr );
}

//--------------------------------------------------------------------------------------------
INLINE void chr_set_shadow( chr_t * pchr, float width )
{
    /// @details BB@> update the base shadow size

    if ( !DEFINED_PCHR( pchr ) ) return;

    pchr->shadow_size_save = width;

    chr_update_size( pchr );
}

//--------------------------------------------------------------------------------------------
INLINE void chr_set_fat( chr_t * pchr, float fat )
{
    /// @details BB@> update all the character size info by specifying the fat value

    if ( !DEFINED_PCHR( pchr ) ) return;

    pchr->fat = fat;

    chr_update_size( pchr );
}

//--------------------------------------------------------------------------------------------
INLINE void chr_set_height( chr_t * pchr, float height )
{
    /// @details BB@> update the base character height

    if ( !DEFINED_PCHR( pchr ) ) return;

    if ( height < 0 ) height = 0;

    pchr->bump_save.height = height;

    chr_update_size( pchr );
}