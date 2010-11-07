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

/// \file script_functions.h
/// \details functions called by scr_run_function()

#include "egoboo_typedef_cpp.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

struct ego_ai_state;
struct ego_script_state;
struct ego_ai_bundle;
struct ego_chr;

//--------------------------------------------------------------------------------------------
// helper functions
//--------------------------------------------------------------------------------------------

Uint8  _display_message( const CHR_REF & ichr, const PRO_REF & iprofile, int message, ego_script_state * pstate );
bool_t _break_passage( int mesh_fx_or, int become, int frames, int starttile, const PASS_REF & passage, int *ptilex, int *ptiley );
Uint8  _find_grid_in_passage( const int x0, const int y0, const int tiletype, const PASS_REF & passage, int *px1, int *py1 );

Uint8   _append_end_text( ego_chr * pchr, const int message, ego_script_state * pstate );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

Uint8 scr_set_AlertBit( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ClearAlertBit( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TestAlertBit( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Alert( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ClearAlert( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TestAlert( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Bit( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ClearBit( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TestBit( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Bits( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ClearBits( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TestBits( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Spawned( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TimeOut( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AtWaypoint( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AtLastWaypoint( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Attacked( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Bumped( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Ordered( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CalledForHelp( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Content( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Killed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetKilled( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ClearWaypoints( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AddWaypoint( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_FindPath( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Compass( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_TargetArmorPrice( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Time( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_Content( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_JoinTargetTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToNearbyEnemy( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToTargetLeftHand( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToTargetRightHand( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToWhoeverAttacked( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToWhoeverBumped( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToWhoeverCalledForHelp( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToOldTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TurnModeToVelocity( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TurnModeToWatch( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TurnModeToSpin( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_BumpHeight( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHasID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHasItemID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHoldingItemID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHasSkillID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Else( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Run( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Walk( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Sneak( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DoAction( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_KeepAction( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_IssueOrder( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DropWeapons( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetDoAction( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_OpenPassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ClosePassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PassageOpen( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GoPoof( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CostTargetItemID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DoActionOverride( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Healed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SendPlayerMessage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CallForHelp( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AddIDSZ( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_State( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_State( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetCanOpenStuff( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Grabbed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Dropped( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToWhoeverIsHolding( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DamageTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_XIsLessThanY( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_WeatherTime( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_BumpHeight( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Reaffirmed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_UnkeepAction( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsOnOtherTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsOnHatedTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PressLatchButton( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToTargetOfLeader( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_LeaderKilled( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_BecomeLeader( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ChangeTargetArmor( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveMoneyToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DropKeys( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_LeaderIsAlive( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsOldTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToLeader( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnCharacter( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_RespawnCharacter( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ChangeTile( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Used( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DropMoney( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_OldTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DetachFromHolder( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHasVulnerabilityID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CleanUp( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CleanedUp( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Sitting( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsHurt( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsAPlayer( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PlaySound( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnParticle( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsAlive( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Stop( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DisaffirmCharacter( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ReaffirmCharacter( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsSelf( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsMale( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsFemale( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToSelf( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToRider( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_AttackTurn( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_DamageType( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_BecomeSpell( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_BecomeSpellbook( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ScoredAHit( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Disaffirmed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TranslateOrder( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToWhoeverWasHit( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToWideEnemy( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Changed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_InWater( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Bored( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TooMuchBaggage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Confused( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHasSpecialID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PressTargetLatchButton( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Invisible( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ArmorIs( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_TargetGrogTime( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_TargetDazeTime( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_DamageType( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_WaterLevel( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_EnchantTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_EnchantChild( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TeleportTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveExperienceToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_IncreaseAmmo( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_UnkurseTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveExperienceToTargetTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Unarmed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_RestockTargetAmmoIDAll( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_RestockTargetAmmoIDFirst( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_FlashTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_RedShift( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_GreenShift( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_BlueShift( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Light( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Alpha( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HitFromBehind( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HitFromFront( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HitFromLeft( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HitFromRight( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsOnSameTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_KillTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_UndoEnchant( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_WaterLevel( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CostTargetMana( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHasAnyID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_BumpSize( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_NotDropped( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_YIsLessThanX( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_FlyHeight( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Blocked( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsDefending( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsAttacking( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs0( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs1( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs2( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs3( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs4( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs5( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs6( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs7( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ContentIs( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TurnModeToWatchTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIsNot( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_XIsEqualToY( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DebugMessage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_BlackTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SendMessageNear( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HitGround( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_NameIsKnown( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_UsageIsKnown( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HoldingItemID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HoldingRangedWeapon( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HoldingMeleeWeapon( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HoldingShield( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Kursed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsKursed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsDressedUp( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_OverWater( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Thrown( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_MakeNameKnown( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_MakeUsageKnown( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StopTargetMovement( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_XY( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_XY( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AddXY( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_MakeAmmoKnown( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnAttachedParticle( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnExactParticle( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AccelerateTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_distanceIsMoreThanTurn( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Crushed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_MakeCrushValid( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToLowestTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_NotPutAway( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TakenOut( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AmmoOut( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PlaySoundLooped( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StopSound( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HealSelf( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Equip( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHasItemIDEquipped( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_OwnerToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToOwner( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Frame( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_BreakPassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_ReloadTime( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToWideBlahID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PoofTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ChildDoActionOverride( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnPoof( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_SpeedPercent( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_ChildState( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnAttachedSizedParticle( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ChangeArmor( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ShowTimer( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_FacingTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PlaySoundVolume( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnAttachedFacedParticle( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIsOdd( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToDistantEnemy( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Teleport( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveStrengthToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveWisdomToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveIntelligenceToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveDexterityToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveLifeToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveManaToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ShowMap( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ShowYouAreHere( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ShowBlipXY( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HealTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PumpTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CostAmmo( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_MakeSimilarNamesKnown( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnAttachedHolderParticle( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetReloadTime( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_FogLevel( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_FogLevel( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_FogTAD( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_FogBottomLevel( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_FogBottomLevel( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CorrectActionForHand( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsMounted( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SparkleIcon( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_UnsparkleIcon( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_TileXY( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TileXY( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_ShadowSize( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_OrderTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToWhoeverIsInPassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CharacterWasABook( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_EnchantBoostValues( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnCharacterXYZ( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnExactCharacterXYZ( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ChangeTargetClass( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PlayFullSound( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnExactChaseParticle( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_CreateOrder( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_OrderSpecialID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_UnkurseTargetInventory( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsSneaking( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DropItems( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_RespawnTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetDoActionSetFrame( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetCanSeeInvisible( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToNearestBlahID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToNearestEnemy( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToNearestFriend( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToNearestLifeform( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_FlashPassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_FindTileInPassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HeldInLeftHand( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_NotAnItem( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_ChildAmmo( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HitVulnerable( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsFlying( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_IdentifyTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_BeatModule( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_EndModule( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DisableExport( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_EnableExport( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_TargetState( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Equipped( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DropTargetMoney( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_TargetContent( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DropTargetKeys( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_JoinTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetJoinTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ClearMusicPassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ClearEndMessage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AddEndMessage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PlayMusic( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_MusicPassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_MakeCrushInvalid( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StopMusic( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_FlashVariable( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AccelerateUp( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_FlashVariableHeight( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_DamageTime( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs8( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs9( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs10( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs11( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs12( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs13( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs14( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_StateIs15( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsAMount( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsAPlatform( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AddStat( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DisenchantTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DisenchantAll( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_VolumeNearestTeammate( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AddShopPassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetPayForArmor( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_JoinEvilTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_JoinNullTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_JoinGoodTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PitsKill( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToPassageID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_MakeNameUnknown( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnExactParticleEndSpawn( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnPoofSpeedSpacingDamage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveExperienceToGoodTeam( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DoNothing( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GrogTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DazeTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_EnableRespawn( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DisableRespawn( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DispelTargetEnchantID( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_HolderBlocked( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHasNotFullMana( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_EnableListenSkill( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToLastItemUsed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_FollowLink( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_OperatorIsLinux( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsAWeapon( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SomeoneIsStealing( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsASpell( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_Backstabbed( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_get_TargetDamageType( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AddQuest( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_BeatQuestAllPlayers( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetHasQuest( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_QuestLevel( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AddQuestAllPlayers( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AddBlipAllEnemies( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_PitsFall( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsOwner( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_End( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TakePicture( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Speech( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_MoveSpeech( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_SecondMoveSpeech( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_AttackSpeech( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_AssistSpeech( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TerrainSpeech( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_SelectSpeech( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_OperatorIsMacintosh( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_ModuleHasIDSZ( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_MorphToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveManaFlowToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveManaReturnToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_Money( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetCanSeeKurses( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SpawnAttachedCharacter( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_KurseTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_ChildContent( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToChild( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_DamageThreshold( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_AccelerateTargetUp( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetAmmo( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_EnableInvictus( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DisableInvictus( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetDamageSelf( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_SetTargetSize( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_DrawBillboard( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_set_TargetToBlahInPassage( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_TargetIsFacingSelf( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_LevelUp( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
Uint8 scr_GiveSkillToTarget( ego_script_state * pstate, ego_ai_bundle * pbdl_self );
