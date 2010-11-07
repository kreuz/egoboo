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

/// \file IDSZ_map.c
/// \brief

#include "IDSZ_map.h"
#include "log.h"

#include "egoboo_fileutil.h"
#include "egoboo_vfs.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
IDSZ_node_t * idsz_map_init( IDSZ_node_t * idsz_map )
{
    if ( NULL == idsz_map ) return idsz_map;

    idsz_map[0].id = IDSZ_NONE;
    idsz_map[0].level = IDSZ_NOT_FOUND;

    return idsz_map;
}

//--------------------------------------------------------------------------------------------
egoboo_rv idsz_map_add( IDSZ_node_t idsz_map[], const size_t idsz_map_len, const IDSZ idsz, const int level )
{
    /// \author ZF
    /// \details  Adds a single IDSZ with the specified level to the map. If it already exists
    ///              in the map, the higher of the two level values will be used.

    egoboo_rv rv = rv_error;
    size_t    i;

    if ( NULL == idsz_map ) return rv_error;

    // we must allow the function to add any level of IDSZ, otherwize we cannot
    // add beaten quests, skills with negative levels, etc.
    if ( IDSZ_NONE == idsz ) return rv_fail;

    for ( i = 0; i < idsz_map_len; i++ )
    {
        // the end of the list?
        if ( IDSZ_NONE == idsz_map[i].id ) break;

        // found a matching idsz?
        if ( idsz == idsz_map[i].id )
        {
            // But only if the new idsz level is "better" than the previous one
            if (( level > 0 && idsz_map[i].level >= level ) ||
                ( level < 0 && idsz_map[i].level <= level ) )
            {
                rv = rv_fail;
            }
            else
            {
                rv = rv_success;

                idsz_map[i].level = level;
            }

            break;
        }
    }

    // Trying to add a idsz to a full idsz list?
    if ( idsz_map_len == i )
    {
        log_warning( "%s - Failed to add [%s] to an IDSZ_map. Consider increasing idsz_map_len (currently %i)\n", __FUNCTION__, undo_idsz( idsz ), idsz_map_len );

        rv = rv_fail;
    }
    else if ( IDSZ_NONE == idsz_map[i].id )
    {
        // Reached the end of the list. Simply append the new idsz to idsz_map

        size_t tail = i + 1;

        // Set the termination down one step, if that step exists
        if ( tail < idsz_map_len )
        {
            idsz_map[tail].id    = IDSZ_NONE;
            idsz_map[tail].level = IDSZ_NOT_FOUND;
        }

        // Add the new idsz
        idsz_map[i].id    = idsz;
        idsz_map[i].level = level;

        rv = rv_success;
    }

    return rv;
}

//--------------------------------------------------------------------------------------------
IDSZ_node_t* idsz_map_get( const IDSZ_node_t idsz_map[], const size_t idsz_map_len, const IDSZ idsz )
{
    /// \author ZF
    /// \details  This function returns a pointer to the IDSZ_node_t from the IDSZ specified
    ///              or NULL if it wasn't found in the map.

    int iterator;
    IDSZ_node_t* pidsz;
    IDSZ_node_t* found_node = NULL;

    if ( NULL == idsz_map || IDSZ_NONE == idsz ) return NULL;

    // initialize the loop
    iterator = 0;
    pidsz = idsz_map_iterate( idsz_map, idsz_map_len, &iterator );

    // iterate the map
    while ( pidsz != NULL )
    {
        // Did we find the idsz?
        if ( pidsz->id == idsz )
        {
            found_node = pidsz;
            break;
        }

        // Get the next element
        pidsz = idsz_map_iterate( idsz_map, idsz_map_len, &iterator );
    }

    return found_node;
}

//--------------------------------------------------------------------------------------------
IDSZ_node_t* idsz_map_iterate( const IDSZ_node_t idsz_map[], const size_t idsz_map_len, int *iterator_ptr )
{
    /// \author ZF
    /// \details  This function iterates through a map containing any number of IDSZ_node_t
    ///              Returns NULL if there are no more elements to iterate.

    int          step = 0;
    IDSZ_node_t *node = NULL;

    if ( NULL == idsz_map || NULL == iterator_ptr ) return NULL;

    // alias the variable
    step = *iterator_ptr;

    // Reached the end of the list without finding a matching idsz
    if ( step < 0  || ( const size_t )step >= idsz_map_len ) return NULL;

    // Found the end of the list?
    if ( IDSZ_NONE == idsz_map[step].id )
    {
        // yes, set the return values to their terminal states
        node          = NULL;
        *iterator_ptr = -1;
    }
    else
    {
        // no, increment the iterator
        node          = ( IDSZ_node_t* )( idsz_map + step );
        *iterator_ptr = step     + 1;
    }

    return node;
}

//--------------------------------------------------------------------------------------------
egoboo_rv idsz_map_copy( const IDSZ_node_t map_src[], const size_t src_len, IDSZ_node_t map_dst[] )
{
    /// \author ZF
    /// \details This function copies one set of IDSZ map to another IDSZ map (exact)

    if ( map_src == NULL || map_dst == NULL || 0 == src_len ) return rv_error;

    // SDL_memcpy() is probably a lot more efficient than copying each element individually
    SDL_memmove( map_dst, map_src, sizeof( IDSZ_node_t ) * src_len );

    return rv_success;

    /*
    int iterator = 0;
    IDSZ_node_t *pidsz;

    // First clear the array we are copying to
    idsz_map_init( map_dst );

    // Iterate and copy each element exact
    pidsz = idsz_map_iterate( map_src, &iterator );
    while( pidsz != NULL )
    {
        idsz_map_add( map_dst, pidsz->id, pidsz->level );
        pidsz = idsz_map_iterate( map_src, &iterator );
    }
    */
}
