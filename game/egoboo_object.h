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

/// @file egoboo_object.h
/// @details Definitions of data that all Egoboo objects should "inherit"

#include "egoboo_typedef.h"
#include "egoboo_state_machine.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// some basic data that all Egoboo objects should have

/// The possible actions that an ego_object_base_t object can perform
enum e_ego_object_actions
{
    ego_object_nothing        = ego_action_invalid,
    ego_object_constructing   = ego_action_beginning,   ///< The object has been allocated and had its critical variables filled with safe values
    ego_object_initializing   = ego_action_entering,    ///< The object is being initialized/re-initialized
    ego_object_processing     = ego_action_running,     ///< The object is fully activated
    ego_object_deinitializing = ego_action_leaving,     ///< The object is being de-initialized
    ego_object_destructing    = ego_action_finishing,   ///< The object is being destructed

    // the states that are specific to objects
    ego_object_waiting                                  ///< The object has been fully destructed and is awaiting final "deletion"
};
typedef enum e_ego_object_actions ego_object_actions_t;

//--------------------------------------------------------------------------------------------

/// The state of an ego_object_base_t object. The object is essentially
/// a state machine in the same way that the "egoboo process" is
struct s_ego_object_state
{
    // basic flags for where the object is in the creation/desctuction process
    bool_t               valid;       ///< The object is a valid object
    bool_t               constructed; ///< The object has been initialized
    bool_t               initialized; ///< The object has been initialized
    bool_t               active;      ///< The object is fully ready to go
    bool_t               killed;      ///< The object is fully "deleted"

    // other state flags
    bool_t               spawning;    ///< The object is being created
    bool_t               on;          ///< The object is being used
    bool_t               paused;      ///< The object's action will be overridden to bo ego_object_nothing

    ego_object_actions_t action;      ///< What action is it performing?
};
typedef struct s_ego_object_state ego_object_state_t;

ego_object_state_t * ego_object_state_ctor( ego_object_state_t * );
ego_object_state_t * ego_object_state_dtor( ego_object_state_t * );
ego_object_state_t * ego_object_state_clear( ego_object_state_t * ptr );

ego_object_state_t * ego_object_state_set_valid( ego_object_state_t *, bool_t val );

ego_object_state_t * ego_object_state_end_constructing( ego_object_state_t * );
ego_object_state_t * ego_object_state_end_initialization( ego_object_state_t * );
ego_object_state_t * ego_object_state_end_processing( ego_object_state_t * );
ego_object_state_t * ego_object_state_end_deinitializing( ego_object_state_t * );
ego_object_state_t * ego_object_state_end_destructing( ego_object_state_t * );
ego_object_state_t * ego_object_state_end_killing( ego_object_state_t * );
ego_object_state_t * ego_object_state_invalidate( ego_object_state_t * );

ego_object_state_t * ego_object_state_begin_waiting( ego_object_state_t * );

//--------------------------------------------------------------------------------------------

/// Structure for handling user requests to the ego_object_base_t
struct s_ego_object_req
{
    bool_t  unpause_me;  ///< request to un-pause the object
    bool_t  pause_me;    ///< request to pause the object
    bool_t  turn_me_on;  ///< request to turn on the object
    bool_t  turn_me_off; ///< request to turn on the object
    bool_t  kill_me;     ///< request to destroy the object
};
typedef struct s_ego_object_req ego_object_req_t;

ego_object_req_t * ego_object_req_ctor( ego_object_req_t * ptr );
ego_object_req_t * ego_object_req_dtor( ego_object_req_t * ptr );
ego_object_req_t * ego_object_req_clear( ego_object_req_t* ptr );

//--------------------------------------------------------------------------------------------

/// The data that is "inherited" by every Egoboo object.
struct s_ego_object_base
{
    // basic object definitions
    STRING               base_name; ///< what is its name at creation. Probably related to the filename that generated it.
    Uint32               guid;      ///< a globally unique identifier

    // "process" control control
    ego_object_state_t   state;     ///< The state of the object_base "process"
    ego_object_req_t     req;       ///< place for making requests to change the state

    // allocation info
    list_object_state_t lst_state;

    // things related to the updating of objects
    size_t         update_count;  ///< How many updates have been made to this object?
    size_t         frame_count;   ///< How many frames have been rendered?

    unsigned       update_guid;   ///< a value that lets you know if an reference in synch with its object list
};

typedef struct s_ego_object_base ego_object_base_t;

ego_object_base_t * ego_object_ctor( ego_object_base_t * pbase, size_t index );
ego_object_base_t * ego_object_dtor( ego_object_base_t * pbase );

ego_object_base_t * ego_object_allocate( ego_object_base_t * pbase, size_t index );
ego_object_base_t * ego_object_deallocate( ego_object_base_t * pbase );
ego_object_base_t * ego_object_invalidate( ego_object_base_t * );
ego_object_base_t * ego_object_end_constructing( ego_object_base_t * );
ego_object_base_t * ego_object_end_initializing( ego_object_base_t * );
ego_object_base_t * ego_object_end_processing( ego_object_base_t * );
ego_object_base_t * ego_object_end_deinitializing( ego_object_base_t * );
ego_object_base_t * ego_object_end_destructing( ego_object_base_t * );
ego_object_base_t * ego_object_end_killing( ego_object_base_t * );

ego_object_base_t * ego_object_begin_processing( ego_object_base_t *, const char * name );
ego_object_base_t * ego_object_validate( ego_object_base_t * pbase );
ego_object_base_t * ego_object_begin_waiting( ego_object_base_t * pbase );

ego_object_base_t * ego_object_req_terminate( ego_object_base_t * pbase );
ego_object_base_t * ego_object_grant_terminate( ego_object_base_t * pbase );
ego_object_base_t * ego_object_set_spawning( ego_object_base_t * pbase, bool_t val );

ego_object_base_t * ego_object_grant_on( ego_object_base_t * pbase );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

/// Mark a ego_object_base_t object as being allocated
#define POBJ_ALLOCATE( PDATA, INDEX ) ( (NULL == (PDATA)) ? NULL : ego_object_allocate( POBJ_GET_PBASE(PDATA), INDEX ) )

/// Mark a ego_object_base_t object as being deallocated
#define POBJ_INVALIDATE( PDATA ) ( (NULL == (PDATA)) ? NULL : ego_object_invalidate( POBJ_GET_PBASE(PDATA) ) )

/// Turn on an ego_object_base_t object
#define POBJ_ACTIVATE( PDATA, NAME ) ( (NULL == (PDATA)) ? NULL : ego_object_begin_processing( POBJ_GET_PBASE(PDATA), NAME ) )

/// Begin turning off an ego_object_base_t object
#define POBJ_REQUEST_TERMINATE( PDATA ) ( (NULL == (PDATA)) ? NULL : ego_object_req_terminate( POBJ_GET_PBASE(PDATA) ) )

/// Completely turn off an ego_object_base_t object and mark it as no longer allocated
#define POBJ_TERMINATE( PDATA ) ( (NULL == (PDATA)) ? NULL : ego_object_grant_terminate( POBJ_GET_PBASE(PDATA) ) )

#define POBJ_BEGIN_SPAWN( PDATA ) \
    if( (NULL != (PDATA)) && FLAG_VALID_PBASE(POBJ_GET_PBASE(PDATA))  ) \
    {\
        if( !(PDATA)->obj_base.state.spawning )\
        {\
            ego_object_set_spawning( POBJ_GET_PBASE(PDATA), btrue );\
            ego_object_spawn_depth++;\
        }\
    }\
     
#define POBJ_END_SPAWN( PDATA ) \
    if( (NULL != (PDATA)) && FLAG_VALID_PBASE(POBJ_GET_PBASE(PDATA)) ) \
    {\
        if( (PDATA)->obj_base.state.spawning )\
        {\
            ego_object_set_spawning( POBJ_GET_PBASE(PDATA), bfalse );\
            ego_object_spawn_depth--;\
        }\
    }\
     
/// Is the object flagged as allocated?
#define FLAG_ALLOCATED_PBASE( PBASE ) ( (PBASE)->lst_state.allocated )
/// Is the object allocated?
#define ALLOCATED_PBASE( PBASE )       ( (NULL != (PBASE)) && FLAG_ALLOCATED_PBASE(PBASE) )

/// Is the object flagged as valid?
#define FLAG_VALID_PBASE( PBASE ) ( (PBASE)->state.valid )
/// Is the object valid?
#define VALID_PBASE( PBASE )       ( (NULL != (PBASE))  && FLAG_ALLOCATED_PBASE( PBASE ) && FLAG_VALID_PBASE(PBASE) )

/// Is the object flagged as constructed?
#define FLAG_CONSTRUCTED_PBASE( PBASE ) ( (PBASE)->state.constructed )
/// Is the object constructed?
#define CONSTRUCTED_PBASE( PBASE )       ( VALID_PBASE( PBASE ) && FLAG_CONSTRUCTED_PBASE(PBASE) )

/// Is the object flagged as initialized?
#define FLAG_INITIALIZED_PBASE( PBASE ) ( (PBASE)->state.initialized )
/// Is the object initialized?
#define INITIALIZED_PBASE( PBASE )      ( CONSTRUCTED_PBASE( PBASE ) && FLAG_INITIALIZED_PBASE(PBASE) )

/// Is the object flagged as killed?
#define FLAG_KILLED_PBASE( PBASE ) ( (PBASE)->state.killed )
/// Is the object killed?
#define KILLED_PBASE( PBASE )       ( VALID_PBASE( PBASE ) && FLAG_KILLED_PBASE(PBASE) )

/// Is the object flagged as requesting termination?
#define FLAG_ON_PBASE( PBASE )  ( (PBASE)->state.on )
/// Is the object on?
#define ON_PBASE( PBASE )       ( VALID_PBASE( PBASE ) && FLAG_ON_PBASE(PBASE) && !FLAG_KILLED_PBASE( PBASE ) )

/// Is the object flagged as kill_me?
#define FLAG_REQ_TERMINATION_PBASE( PBASE ) ( (PBASE)->req.kill_me )
/// Is the object kill_me?
#define REQ_TERMINATION_PBASE( PBASE )      ( VALID_PBASE( PBASE ) && FLAG_REQ_TERMINATION_PBASE(PBASE) )

/// Has the object been created yet?
#define STATE_CONSTRUCTING_PBASE( PBASE ) ( (ego_object_constructing == (PBASE)->state.action) )
/// Has the object been created yet?
#define CONSTRUCTING_PBASE( PBASE )       ( VALID_PBASE( PBASE ) && STATE_CONSTRUCTING_PBASE(PBASE) )

/// Is the object in the initializing state?
#define STATE_INITIALIZING_PBASE( PBASE ) ( (ego_object_initializing == (PBASE)->state.action) )
/// Is the object being initialized right now?
#define INITIALIZING_PBASE( PBASE )       ( CONSTRUCTED_PBASE( PBASE ) && STATE_INITIALIZING_PBASE(PBASE) )

/// Is the object in the active state?
#define STATE_PROCESSING_PBASE( PBASE ) ( ego_object_processing == (PBASE)->state.action )
/// Is the object active?
#define ACTIVE_PBASE( PBASE )           ( INITIALIZED_PBASE( PBASE ) && STATE_PROCESSING_PBASE(PBASE) && FLAG_ON_PBASE(PBASE) && !FLAG_KILLED_PBASE( PBASE ) )

/// Is the object in the deinitializing state?
#define STATE_DEINITIALIZING_PBASE( PBASE ) ( (ego_object_deinitializing == (PBASE)->state.action) )
/// Is the object being deinitialized right now?
#define DEINITIALIZING_PBASE( PBASE )       ( CONSTRUCTED_PBASE( PBASE ) && STATE_DEINITIALIZING_PBASE(PBASE) )

/// Is the object in the destructing state?
#define STATE_DESTRUCTING_PBASE( PBASE ) ( (ego_object_destructing == (PBASE)->state.action) )
/// Is the object being deinitialized right now?
#define DESTRUCTING_PBASE( PBASE )       ( VALID_PBASE( PBASE ) && STATE_DESTRUCTING_PBASE(PBASE) )

/// Is the object "waiting to die" state?
#define STATE_WAITING_PBASE( PBASE ) ( ego_object_waiting == (PBASE)->state.action )
/// Is the object "waiting to die"?
#define WAITING_PBASE( PBASE )       ( VALID_PBASE( PBASE ) && STATE_WAITING_PBASE(PBASE) )

/// Has the object been marked as terminated? (alias for KILLED_PBASE)
#define STATE_TERMINATED_PBASE( PBASE ) FLAG_KILLED_PBASE(PBASE)
#define TERMINATED_PBASE( PBASE )       KILLED_PBASE( PBASE )

/// Grab a pointer to the ego_object_base_t of an object that "inherits" this data
#define POBJ_GET_PBASE( POBJ )   ( (NULL == (POBJ)) ? NULL : &((POBJ)->obj_base) )

/// Grab the index value of object that "inherits" from ego_object_base_t
#define GET_INDEX_POBJ( POBJ, FAIL_VALUE )  ( (NULL == (POBJ) || !VALID_PBASE( POBJ_GET_PBASE( (POBJ) ) ) ) ? FAIL_VALUE : (POBJ)->obj_base.lst_state.index )
#define GET_REF_POBJ( POBJ, FAIL_VALUE )    ((REF_T)GET_INDEX_POBJ( POBJ, FAIL_VALUE ))

/// Grab the state of object that "inherits" from ego_object_base_t
#define GET_STATE_POBJ( POBJ )  ( (NULL == (POBJ) || !VALID_PBASE( POBJ_GET_PBASE( (POBJ) ) ) ) ? ego_object_nothing : (POBJ)->obj_base.lst_state.index )

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

/// A variable to hold the object guid counter
extern Uint32 ego_object_guid;

extern Uint32 ego_object_spawn_depth;

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#define egoboo_object_h
