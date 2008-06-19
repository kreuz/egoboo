#pragma once

#include "object.h"
#include "sound.h"
#include "Mesh.h"
#include "Mad.h"
#include "input.h"

struct CMad_t;

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#define GRIP_SIZE           4
#define GRIP_VERTICES      (2*GRIP_SIZE)   // Each model has 8 grip vertices


#define SPINRATE            200                     // How fast spinners spin
#define FLYDAMPEN           .001                    // Levelling rate for flyers

#define JUMPINFINITE        255                     // Flying character
#define DELAY_JUMP           20                      // Time between jumps
#define JUMPTOLERANCE       20                      // Distance above ground to be jumping
#define SLIDETOLERANCE      10                      // Stick to ground better
#define PLATTOLERANCE       50                      // Platform tolerance...
#define PLATADD             -10                     // Height add...
#define PLATASCEND          .10                     // Ascension rate
#define PLATKEEP            (1.0f-PLATASCEND)       // Retention rate

#define DONTFLASH 255                               //

#define MAXVERTICES                    1024         // Max number of points in a model

#define MAXSTOR             (1<<4)                  // Storage data
#define STORAND             (MAXSTOR-1)             //


#define DELAY_BORE                        (rand()&255)+120
#define DELAY_CAREFUL                     50
#define REEL                            7600.0      // Dampen for melee knock back
#define REELBASE                        .35         //

#define WATCHMIN            .01                     //
#define DELAY_PACK           25                      // Time before inventory rotate again
#define DELAY_GRAB           25                      // Time before grab again
#define NOSKINOVERRIDE      -1                      // For import
#define PITDEPTH            -30                     // Depth to kill character
#define DISMOUNTZVEL        16
#define DISMOUNTZVELFLY     4

#define THROWFIX            30.0                    // To correct thrown velocities
#define MINTHROWVELOCITY    15.0                    //
#define MAXTHROWVELOCITY    45.0                    //

#define RETURNAND           63                      // Return mana every so often
#define MANARETURNSHIFT     4                       //

#define RIPPLEAND           15                      // How often ripples spawn
#define RIPPLETOLERANCE     60                      // For deep water
#define SPLASHTOLERANCE     10                      //
#define CLOSETOLERANCE      2                       // For closing doors

#define DROPXYVEL           8                       //
#define DROPZVEL            7                       //
#define JUMPATTACKVEL       -2                      //
#define WATERJUMP           12                      //


#define CAPLST_COUNT              OBJLST_COUNT
#define CHRLST_COUNT              350         // Max number of characters
#define MAXSKIN             4
#define MAXCAPNAMESIZE      32                      // Character class names
#define BASELEVELS            6                       // Levels 0-5

#define MAXXP    ((1<<30)-1)                               // Maximum experience (Sint32 32)
#define MAXMONEY 9999                                      // Maximum money

#define SEEKURSEAND         31                      // Blacking flash
#define SEEINVISIBLE        128                     // Cutoff for invisible characters
#define INVISIBLE           20                      // The character can't be detected

typedef enum missle_handling_e
{
  MIS_NORMAL  = 0,                  //Treat missiles normally
  MIS_DEFLECT,                      //Deflect incoming missiles
  MIS_REFLECT                       //Reflect them back!
} MISSLE_TYPE;

typedef enum slot_e
{
  SLOT_LEFT,
  SLOT_RIGHT,
  SLOT_SADDLE,          // keep a slot open for a possible "saddle" for future use

  // other values
  SLOT_INVENTORY,       // this is a virtual "slot" that really means the inventory
  SLOT_NONE,

  // aliases
  SLOT_BEGIN = SLOT_LEFT,
  SLOT_COUNT = SLOT_INVENTORY
} SLOT;

extern SLOT _slot;

typedef enum grip_e
{
  GRIP_ORIGIN   = 0,                                  // Grip at mount's origin
  GRIP_LAST     = 1,                                  // Grip at mount's last vertex
  GRIP_RIGHT    = (( SLOT_RIGHT + 1 ) * GRIP_SIZE ),  // Grip at mount's right hand
  GRIP_LEFT     = (( SLOT_LEFT  + 1 ) * GRIP_SIZE ),  // Grip at mount's left hand

  // other values
  GRIP_NONE,

  // Aliases
  GRIP_SADDLE    = GRIP_LEFT,    // Grip at mount's "saddle" (== left hand for now)
  GRIP_INVENTORY = GRIP_ORIGIN   // "Grip" in the object's inventory
} GRIP;


//--------------------------------------------------------------------------------------------

struct Cap_t;
struct CChr_t;
struct CGame_t;

//--------------------------------------------------------------------------------------------


typedef enum gender_e
{
  GEN_FEMALE = 0,
  GEN_MALE,
  GEN_OTHER,
  GEN_RANDOM,
} GENDER;

typedef enum latch_button_e
{
  LATCHBUTTON_NONE      =      0,
  LATCHBUTTON_LEFT      = 1 << 0,                    // Character button presses
  LATCHBUTTON_RIGHT     = 1 << 1,                    //
  LATCHBUTTON_JUMP      = 1 << 2,                    //
  LATCHBUTTON_ALTLEFT   = 1 << 3,                    // ( Alts are for grab/drop )
  LATCHBUTTON_ALTRIGHT  = 1 << 4,                    //
  LATCHBUTTON_PACKLEFT  = 1 << 5,                    // ( Packs are for inventory cycle )
  LATCHBUTTON_PACKRIGHT = 1 << 6,                    //
  LATCHBUTTON_RESPAWN   = 1 << 7                     //
} LATCHBUTTON_BITS;

typedef enum alert_bits_e
{
  ALERT_NONE                       =       0,
  ALERT_SPAWNED                    = 1 <<  0,
  ALERT_HITVULNERABLE              = 1 <<  1,
  ALERT_ATWAYPOINT                 = 1 <<  2,
  ALERT_ATLASTWAYPOINT             = 1 <<  3,
  ALERT_ATTACKED                   = 1 <<  4,
  ALERT_BUMPED                     = 1 <<  5,
  ALERT_SIGNALED                   = 1 <<  6,
  ALERT_CALLEDFORHELP              = 1 <<  7,
  ALERT_KILLED                     = 1 <<  8,
  ALERT_TARGETKILLED               = 1 <<  9,
  ALERT_DROPPED                    = 1 << 10,
  ALERT_GRABBED                    = 1 << 11,
  ALERT_REAFFIRMED                 = 1 << 12,
  ALERT_LEADERKILLED               = 1 << 13,
  ALERT_USED                       = 1 << 14,
  ALERT_CLEANEDUP                  = 1 << 15,
  ALERT_SCOREDAHIT                 = 1 << 16,
  ALERT_HEALED                     = 1 << 17,
  ALERT_DISAFFIRMED                = 1 << 18,
  ALERT_CHANGED                    = 1 << 19,
  ALERT_INWATER                    = 1 << 20,
  ALERT_BORED                      = 1 << 21,
  ALERT_TOOMUCHBAGGAGE             = 1 << 22,
  ALERT_GROGGED                    = 1 << 23,
  ALERT_DAZED                      = 1 << 24,
  ALERT_HITGROUND                  = 1 << 25,
  ALERT_NOTDROPPED                 = 1 << 26,
  ALERT_BLOCKED                    = 1 << 27,
  ALERT_THROWN                     = 1 << 28,
  ALERT_CRUSHED                    = 1 << 29,
  ALERT_NOTPUTAWAY                 = 1 << 30,
  ALERT_TAKENOUT                   = 1 << 31
} ALERT_BITS;

typedef enum turnmode_e
{
  TURNMODE_NONE     = 0,
  TURNMODE_VELOCITY,                      // Character gets rotation from velocity
  TURNMODE_WATCH,                             // For watch towers
  TURNMODE_SPIN,                              // For spinning objects
  TURNMODE_WATCHTARGET                        // For combat intensive AI
} TURNMODE;

typedef enum Experience_e
{
  XP_FINDSECRET = 0,                           // Finding a secret
  XP_WINQUEST,                                 // Beating a module or a subquest
  XP_USEDUNKOWN,                               // Used an unknown item
  XP_KILLENEMY,                                // Killed an enemy
  XP_KILLSLEEPY,                               // Killed a sleeping enemy
  XP_KILLHATED,                                // Killed a hated enemy
  XP_TEAMKILL,                                 // Team has killed an enemy
  XP_TALKGOOD,                                 // Talk good, er...  I mean well
  XP_COUNT,                                    // Number of ways to get experience
  XP_DIRECT     = 255                          // No modification
} EXPERIENCE;

typedef enum idsz_index_e
{
  IDSZ_PARENT = 0,                             // Parent index
  IDSZ_TYPE,                                   // Self index
  IDSZ_SKILL,                                  // Skill index
  IDSZ_SPECIAL,                                // Special index
  IDSZ_HATE,                                   // Hate index
  IDSZ_VULNERABILITY,                          // Vulnerability index
  IDSZ_COUNT                                   // ID strings per character
} IDSZ_INDEX;

#define MAXNUMINPACK        6                       // Max number of items to carry in pack
#define LOWSTAT             INT_TO_FP8(  1)      // Worst...
#define PERFECTSTAT         INT_TO_FP8( 75)      // Perfect...
#define HIGHSTAT            INT_TO_FP8( 99)      // Absolute MAX strength...
#define PERFECTBIG          INT_TO_FP8(127)      // Perfect life or mana...
#define DAMAGE_MIN          INT_TO_FP8(  1)      // Minimum damage for hurt animation
#define DAMAGE_HURT         INT_TO_FP8(  1)      // 1 point of damage == hurt


//--------------------------------------------------------------------------------------------
extern Uint16          numdolist;                  // How many in the list
extern CHR_REF         dolist[CHRLST_COUNT];             // List of which characters to draw

//--------------------------------------------------------------------------------------------
typedef struct skin_t
{
  Uint8         defense_fp8;                // Defense for each skin
  char          name[MAXCAPNAMESIZE];   // Skin name
  Uint16        cost;                   // Store prices
  Uint8         damagemodifier_fp8[MAXDAMAGETYPE];
  float         maxaccel;                   // Acceleration for each skin
} SKIN;

//--------------------------------------------------------------------------------------------
typedef struct CProperties_t
{
  // [BEGIN] Character template parameters that are like Skill Expansions
  bool_t        istoobig;                      // Can't be put in pack
  bool_t        reflect;                       // Draw the reflection
  bool_t        alwaysdraw;                    // Always render
  bool_t        isranged;                      // Flag for ranged weapon
  bool_t        isequipment;                   // Behave in silly ways
  bool_t        ridercanattack;                // Rider attack?
  bool_t        canbedazed;                    // Can it be dazed?
  bool_t        canbegrogged;                  // Can it be grogged?
  bool_t        resistbumpspawn;               // Don't catch fire
  bool_t        waterwalk;                     // Walk on water?
  bool_t        invictus;                      // Is it invincible?
  bool_t        canseeinvisible;               // Can it see invisible?
  bool_t        iskursed;                      // Can't be dropped?  Could this also mean damage debuff? Spell fizzle rate? etc.
  bool_t        canchannel;                    //
  bool_t        canopenstuff;                  // Can it open chests/doors?
  bool_t        isitem;                        // Is it an item?
  bool_t        ismount;                       // Can you ride it?
  bool_t        isstackable;                   // Is it arrowlike?
  bool_t        isplatform;                    // Can be stood on?
  bool_t        canuseplatforms;               // Can use platforms?
  bool_t        cangrabmoney;                  // Collect money?
  bool_t        icon;                          // Draw icon
  bool_t        forceshadow;                   // Draw a shadow?
  bool_t        ripple;                        // Spawn ripples?
  bool_t        stickybutt;                    // Stick to the ground?
  bool_t        nameknown;                     // Is the class name known?
  bool_t        usageknown;                    // Is its usage known
  bool_t        canbecrushed;                  // Crush in a door?
  // [END] Character template parameters that are like Skill Expansions


  // [BEGIN] Skill Expansions
  bool_t     canseekurse;             // Can it see kurses?
  bool_t     canusearcane;            // Can use [WMAG] spells?
  bool_t     canjoust;                // Can it use a lance to joust?
  bool_t     canusetech;              // Can it use [TECH]items?
  bool_t     canusedivine;            // Can it use [HMAG] runes?
  bool_t     candisarm;               // Disarm and find traps [DISA]
  bool_t     canbackstab;             // Backstab and murder [STAB]
  bool_t     canuseadvancedweapons;   // Advanced weapons usage [AWEP]
  bool_t     canusepoison;            // Use poison without err [POIS]
  bool_t     canread;                 // Can read books and scrolls [READ]
  // [END] Skill Expansions

} CProperties;

//--------------------------------------------------------------------------------------------
typedef struct CStatData_t
{
  // life
  PAIR          life_fp8;                      // Life
  PAIR          lifeperlevel_fp8;              //
  Sint16        lifereturn_fp8;                //
  Uint16        lifeheal_fp8;                  //

  // mana
  PAIR          mana_fp8;                      // Mana
  PAIR          manaperlevel_fp8;              //
  PAIR          manareturn_fp8;                //
  PAIR          manareturnperlevel_fp8;        //
  PAIR          manaflow_fp8;                  //
  PAIR          manaflowperlevel_fp8;          //

  // SWID
  PAIR          strength_fp8;                  // Strength
  PAIR          strengthperlevel_fp8;          //
  PAIR          wisdom_fp8;                    // Wisdom
  PAIR          wisdomperlevel_fp8;            //
  PAIR          intelligence_fp8;              // Intlligence
  PAIR          intelligenceperlevel_fp8;      //
  PAIR          dexterity_fp8;                 // Dexterity
  PAIR          dexterityperlevel_fp8;         //

} CStatData;

//--------------------------------------------------------------------------------------------
typedef struct CCap_t
{
  bool_t        used;

  char          classname[MAXCAPNAMESIZE];     // Class name
  Sint8         skinoverride;                  // -1 or 0-3.. For import
  Uint8         leveloverride;                 // 0 for normal
  int           stateoverride;                 // 0 for normal
  int           contentoverride;               // 0 for normal
  Uint16        skindressy;                    // bits for dressy skins
  float         strengthdampen;                // Strength damage factor
  Uint8         stoppedby;                     // Collision Mask
  bool_t        uniformlit;                    // Bad lighting?
  Uint8         lifecolor;                     // Bar colors
  Uint8         manacolor;                     //
  Uint8         ammomax;                       // Ammo stuff
  Uint8         ammo;                          //
  GENDER        gender;                        // Gender

  CStatData     statdata;

  Sint16        manacost_fp8;                  // Mana cost to use
  Sint16        money;                         // Money
  float         size;                          // Scale of model
  float         sizeperlevel;                  // Scale increases
  float         dampen;                        // Bounciness
  float         bumpstrength;                  // ghostlike interaction with objects?
  Uint8         shadowsize;                    // Shadow size
  float         bumpsize;                      // Bounding octagon
  float         bumpsizebig;                   // For octagonal bumpers
  float         bumpheight;                    //
  float         bumpdampen;                    // Mass
  float         weight;                        // Weight
  float         jump;                          // Jump power
  Uint8         jumpnumber;                    // Number of jumps ( Ninja )
  Uint8         spd_sneak;                      // Sneak threshold
  Uint8         spd_walk;                       // Walk threshold
  Uint8         spd_run;                        // Run threshold
  Uint8         flyheight;                     // Fly height
  Uint8         flashand;                      // Flashing rate
  Uint16        alpha_fp8;                     // Transparency
  Uint16        light_fp8;                     // Light blending
  bool_t        transferblend;                 // Transfer blending to rider/weapons
  Uint8         sheen_fp8;                     // How shiny it is ( 0-15 )
  bool_t        enviro;                        // Phong map this baby?
  Uint16        uoffvel;                       // Texture movement rates
  Uint16        voffvel;                       //
  Uint16        iframefacing;                  // Invincibility frame
  Uint16        iframeangle;                   //
  Uint16        nframefacing;                  // Normal frame
  Uint16        nframeangle;                   //
  int           experienceforlevel[BASELEVELS];  // Experience needed for next level
  float         experienceconst;
  float         experiencecoeff;
  PAIR          experience;                    // Starting experience
  int           experienceworth;               // Amount given to killer/user
  float         experienceexchange;            // Adds to worth
  float         experiencerate[XP_COUNT];
  IDSZ          idsz[IDSZ_COUNT];                 // ID strings

  bool_t        cancarrytonextmodule;          // Take it with you?
  bool_t        needskillidtouse;              // Check IDSZ first?
  DAMAGE        damagetargettype;              // For AI DamageTarget
  ACTION        weaponaction;                  // Animation needed to swing
  bool_t        slotvalid[SLOT_COUNT];         // Left/Right hands valid
  bool_t        attackattached;                //
  PIP_REF       attackprttype;                 //
  Uint8         attachedprtamount;             // Sticky particles
  DAMAGE        attachedprtreaffirmdamagetype; // Relight that torch...
  PIP_REF       attachedprttype;               //
  Uint8         gopoofprtamount;               // Poof effect
  Sint16        gopoofprtfacingadd;            //
  PIP_REF       gopoofprttype;                 //
  BLUD_LEVEL    bludlevel;                     // Blud ( yuck )
  PIP_REF       bludprttype;                   //
  Sint8         footfallsound;                 // Footfall sound, -1
  Sint8         jumpsound;                     // Jump sound, -1
  Uint8         kursechance;                   // Chance of being kursed
  Sint8         hidestate;                       // Don't draw when...

  SKIN          skin[MAXSKIN];

  CProperties   prop;

} CCap;

#ifdef __cplusplus
  typedef TList<CCap_t, CAPLST_COUNT> CapList_t;
  typedef TPList<CCap_t, CAPLST_COUNT> PCap;
#else
  typedef CCap CapList_t[CAPLST_COUNT];
  typedef CCap * PCap;
#endif

CCap *  Cap_new(CCap *pcap);
bool_t Cap_delete(CCap * pcap);
CCap *  Cap_renew(CCap *pcap);

#define CAP_INHERIT_IDSZ(PGS, MODEL, ID) (PGS->CapList[MODEL].idsz[IDSZ_PARENT] == (IDSZ)(ID) || PGS->CapList[MODEL].idsz[IDSZ_TYPE] == (IDSZ)(ID))
#define CAP_INHERIT_IDSZ_RANGE(PGS, MODEL,IDMIN,IDMAX) ((PGS->CapList[MODEL].idsz[IDSZ_PARENT] >= (IDSZ)(IDMIN) && PGS->CapList[MODEL].idsz[IDSZ_PARENT] <= (IDSZ)(IDMAX)) || (PGS->CapList[MODEL].idsz[IDSZ_TYPE] >= (IDSZ)(IDMIN) && PGS->CapList[MODEL].idsz[IDSZ_TYPE] <= (IDSZ)(IDMAX)) )

#define VALID_CAP_RANGE(XX) (((XX)>=0) && ((XX)<CAPLST_COUNT))
#define VALID_CAP(LST, XX)    ( VALID_CAP_RANGE(XX) && LST[XX].used )
#define VALIDATE_CAP(LST, XX) ( VALID_CAP(LST, XX) ? (XX) : (INVALID_CAP) )

//--------------------------------------------------------------------------------------------
typedef struct lighting_data_t
{
  vect4   emission, diffuse, specular;
  GLfloat shininess[1];
} LData;


//--------------------------------------------------------------------------------------------
typedef struct action_info_t
{
  bool_t  keep;      // Keep the action playing?
  bool_t  loop;      // Loop it too?
  bool_t  ready;     // Ready to play a new one?
  ACTION  now;       // Character's current action
  ACTION  next;      // Character's next    action
} ACTION_INFO;

INLINE ACTION_INFO * action_info_new( ACTION_INFO * a);

//--------------------------------------------------------------------------------------------
typedef struct animation_info_t
{
  Uint16          next;       // Character's frame
  Uint16          last;       // Character's last frame
  float           flip;
  Uint8           ilip;    // Character's low-res frame in betweening
} ANIM_INFO;

INLINE ANIM_INFO * anim_info_new( ANIM_INFO * a );

//--------------------------------------------------------------------------------------------
#define MAXWAY              8                       // Waypoints
#define WAYTHRESH           128                     // Threshold for reaching waypoint

typedef vect2 WAYPOINT;

typedef struct waypoint_list_t
{
  int      head, tail;
  WAYPOINT pos[MAXWAY];
} WP_LIST;

INLINE WP_LIST * wp_list_new(WP_LIST * w, vect3 * pos);
INLINE bool_t    wp_list_clear(WP_LIST * w);
INLINE bool_t    wp_list_advance(WP_LIST * wl);
INLINE bool_t    wp_list_add(WP_LIST * wl, float x, float y);

INLINE bool_t wp_list_empty( WP_LIST * wl );
INLINE float  wp_list_x( WP_LIST * wl );
INLINE float  wp_list_y( WP_LIST * wl );

bool_t wp_list_prune(WP_LIST * wl);

//--------------------------------------------------------------------------------------------
typedef struct ai_state_t
{
  Uint16          type;          // The AI script to run

  // some ai state variables 
  int             state;         // Short term memory for AI
  Uint32          alert;         // Alerts for AI script
  float           time;          // AI Timer
  bool_t          morphed;       // Some various other stuff

  // internal variables - "short term" or temporary variables
  int             content;       // A variable set by "setup.txt"
  WP_LIST         wp;
  int             x[MAXSTOR];    // Temporary values...  SetXY
  int             y[MAXSTOR];    //

  // "pointers" to various external data
  CHR_REF         target;          // Who the AI is after
  CHR_REF         oldtarget;       // The target the last time the script was run
  CHR_REF         owner;           // The character's owner
  CHR_REF         child;           // The character's child
  CHR_REF         bumplast;        // Last character it was bumped by
  CHR_REF         attacklast;      // Last character it was attacked by
  CHR_REF         hitlast;         // Last character it hit

  // other random stuff
  Uint16          directionlast;   // Direction of last attack/healing
  DAMAGE          damagetypelast;  // Last damage type
  TURNMODE        turnmode;        // Turning mode
  vect3           trgvel;          // target's velocity
  CLatch           latch;           // latches

  // "global" variables. Any character to character messages should be sent another way
  // there is no guarantee that scripts will be processed in any specific order!
  Uint32 offset;
  Uint32 indent;
  Uint32 lastindent;
  Sint32 operationsum;

  Sint32 tmpx;
  Sint32 tmpy;
  Uint32 tmpturn;
  Sint32 tmpdistance;
  Sint32 tmpargument;
} AI_STATE;

INLINE AI_STATE * ai_state_new(struct CGame_t * gs, AI_STATE * a, CHR_REF ichr);
INLINE AI_STATE * ai_state_renew(AI_STATE * a, CHR_REF ichr);


//--------------------------------------------------------------------------------------------
typedef struct chr_terrain_light_t
{
  vect3_ui08      ambi_fp8; // 0-255, terrain light
  vect3_ui08      spek_fp8; // 0-255, terrain light
  vect3_ui16      turn_lr;  // Character's light rotation 0 to 65535
} CHR_TLIGHT;


//--------------------------------------------------------------------------------------------
typedef struct CStats_t
{
  // life
  Sint16  life_fp8;            // Life stuff
  Sint16  lifemax_fp8;         //
  Uint16  lifeheal_fp8;        //
  Sint16  lifereturn_fp8;      // Regeneration/poison

  // mana
  Sint16  mana_fp8;            // Mana stuff
  Sint16  manamax_fp8;         //
  Sint16  manaflow_fp8;        //
  Sint16  manareturn_fp8;      //

  // SWID
  Sint16  strength_fp8;        // Strength
  Sint16  wisdom_fp8;          // Wisdom
  Sint16  intelligence_fp8;    // Intelligence
  Sint16  dexterity_fp8;       // Dexterity
} CStats;

//--------------------------------------------------------------------------------------------
typedef struct CChr_t
{
  bool_t          on;              // Does it exist?
  char            name[MAXCAPNAMESIZE];  // Character name
  bool_t          gopoof;          // is poof requested?
  bool_t          freeme;          // is ChrList_free_one() requested?


  matrix_4x4      matrix;          // Character's matrix
  bool_t          matrix_valid;    // Did we make one yet?

  OBJ_REF         model;           // Character's current model
  OBJ_REF         model_base;      // The true form

  bool_t          alive;           // Is it alive?

  // pack stuff
  CHR_REF         inwhichpack;     // Is it in whose inventory?
  CHR_REF         nextinpack;      // Link to the next item
  Uint8           numinpack;       // How many

  // stats
  CStats          stats;
  int             experience;          // Experience
  int             experiencelevel;     // Experience Level
  Sint32          money;               // Money
  Uint8           ammomax;             // Ammo stuff
  Uint8           ammo;                //
  GENDER          gender;              // Gender

  // stat graphic info
  Uint8           sparkle;         // Sparkle color or 0 for off
  Uint8           lifecolor;       // Bar color
  Uint8           manacolor;       // Bar color
  bool_t          staton;          // Display stats?

  // position info
  vect3           stt;             // Starting position
  vect3           pos;             // Character's position
  Uint16          turn_lr;   // Character's rotation 0 to 65535
  vect3           vel;             // Character's velocity
  vect3           pos_old;         // Character's last position
  Uint16          turn_lr_old;     //
  Uint16          mapturn_lr;       //
  Uint16          mapturn_ud;       //

  // physics info
  CPhysAccum      accum;
  float           flyheight;       // Height to stabilize at
  bool_t          inwater;         //
  float           dampen;          // Bounciness
  float           bumpstrength;    // ghost-like interaction with objects?
  float           level;           // Height under character
  bool_t          levelvalid;      // Has height been stored?

  AI_STATE        aistate;           // ai-specific into


  float           jump;            // Jump power
  float           jumptime;        // Delay until next jump
  float           jumpnumber;      // Number of jumps remaining
  Uint8           jumpnumberreset; // Number of jumps total, 255=Flying
  bool_t          jumpready;       // For standing on a platform character

  Uint32          onwhichfan;      // Where the char is
  bool_t          indolist;        // Has it been added yet?

  Uint16          uoffset_fp8;     // For moving textures
  Uint16          voffset_fp8;     //
  Uint16          uoffvel;         // Moving texture speed
  Uint16          voffvel;         //

  ACTION_INFO     action;
  ANIM_INFO       anim;

  TEAM_REF        team;            // Character's team
  TEAM_REF        baseteam;        // Character's starting team

  // lighting info
  vect3_ui08      vrta_fp8[MAXVERTICES];  // Lighting hack ( Ooze )
  CHR_TLIGHT      tlight;                 // terrain lighting info
  VData_Blended   vdata;                  // pre-processed per-vertex lighting data
  LData           ldata;                  // pre-processed matrial parameters
  Uint16          alpha_fp8;                 // 255 = Solid, 0 = Invisible
  Uint16          light_fp8;                 // 1 = Light, 0 = Normal
  Uint8           flashand;                  // 1,3,7,15,31 = Flash, 255 = Don't
  Uint8           sheen_fp8;           // 0-15, how shiny it is
  bool_t          transferblend;       // Give transparency to weapons?
  Uint8           redshift;            // Color channel shifting
  Uint8           grnshift;            //
  Uint8           blushift;            //
  bool_t          enviro;              // Environment map?

  CHR_REF         holdingwhich[SLOT_COUNT];  // !=INVALID_CHR if character is holding something
  CHR_REF         attachedto;                // !=INVALID_CHR if character is a held weapon
  Uint16          attachedgrip[GRIP_SIZE];   // Vertices which describe the weapon grip


  // bumber info
  BData           bmpdata;           // character bump size data
  BData           bmpdata_save;
  float           bumpdampen;      // Character bump mass

  Uint8           spd_sneak;        // Sneaking if above this speed
  Uint8           spd_walk;         // Walking if above this speed
  Uint8           spd_run;          // Running if above this speed

  DAMAGE          damagetargettype;   // Type of damage for AI DamageTarget
  DAMAGE          reaffirmdamagetype; // For relighting torches
  float           damagetime;         // Invincibility timer

  Uint16          skin_ref;           // which skin
  SKIN            skin;               // skin data

  float           weight;          // Weight ( for pressure plates )

  Uint8           passage;         // The passage associated with this character

  Uint32          message;           // The last order given the character
  Uint8           messagedata;       // The rank of the character on the order chain

  CHR_REF         onwhichplatform; // What are we standing on?
  Uint16          holdingweight;   // For weighted buttons
  SLOT            inwhichslot;     // SLOT_LEFT or SLOT_RIGHT or SLOT_SADDLE
  bool_t          isequipped;      // For boots and rings and stuff

  Sint16          manacost_fp8;    // Mana cost to use
  Uint32          stoppedby;       // Collision mask

  // timers
  Sint16          grogtime;        // Grog timer
  Sint16          dazetime;        // Daze timer
  float           boretime;        // Boredom timer
  float           carefultime;     // "You hurt me!" timer
  float           reloadtime;      // Time before another shot

  bool_t          ammoknown;       // Is the ammo known?
  bool_t          hitready;        // Was it just dropped?
  PLA_REF         whichplayer;     // not PLALST_COUNT for true

  // enchant info
  ENC_REF         firstenchant;    // Linked list for enchants
  ENC_REF         undoenchant;     // Last enchantment spawned

  // missle info
  MISSLE_TYPE     missiletreatment;// For deflection, etc.
  Uint8           missilecost;     // Mana cost for each one
  CHR_REF         missilehandler;  // Who pays the bill for each one...

  Uint16          damageboost;     // Add to swipe damage
  bool_t          overlay;         // Is this an overlay?  Track aitarget...


  // matrix mods
  float           scale;           // Character's size (useful)
  float           fat;             // Character's size (legible)
  float           sizegoto;        // Character's size goto ( legible )
  float           sizegototime;    // Time left in siez change
  vect3           pancakepos;
  vect3           pancakevel;

  Sint8           loopingchannel;    // Channel number of the loop so
  float           loopingvolume;     // Sound volume of the channel

  
  CProperties   prop;                // all character properties
} CChr;

#ifdef __cplusplus
  typedef TList<CChr_t, CHRLST_COUNT> ChrList_t;
  typedef TPList<CChr_t, CHRLST_COUNT> PChr;
#else
  typedef CChr ChrList_t[CHRLST_COUNT];
  typedef CChr * PChr;
#endif

CChr * Chr_new(CChr * pchr);
bool_t Chr_delete(CChr * pchr);
CChr * Chr_renew(CChr * pchr);

struct CChr_t * ChrList_getPChr(struct CGame_t * gs, CHR_REF ichr);
struct CProfile_t * ChrList_getPObj(struct CGame_t * gs, CHR_REF ichr);
struct CCap_t * ChrList_getPCap(struct CGame_t * gs, CHR_REF ichr);
struct CMad_t * ChrList_getPMad(struct CGame_t * gs, CHR_REF ichr);
struct CPip_t * ChrList_getPPip(struct CGame_t * gs, CHR_REF ichr, int i);

OBJ_REF ChrList_getRObj(struct CGame_t * gs, CHR_REF ichr);
CAP_REF ChrList_getRCap(struct CGame_t * gs, CHR_REF ichr);
MAD_REF ChrList_getRMad(struct CGame_t * gs, CHR_REF ichr);
PIP_REF ChrList_getRPip(struct CGame_t * gs, CHR_REF ichr, int i);



#define VALID_CHR_RANGE(XX) (((XX)>=0) && ((XX)<CHRLST_COUNT))
#define VALID_CHR(LST, XX)    ( VALID_CHR_RANGE(XX) && LST[XX].on )
#define VALIDATE_CHR(LST, XX) ( VALID_CHR(LST, XX) ? (XX) : (INVALID_CHR) )

INLINE bool_t chr_in_pack( PChr lst, size_t lst_size, CHR_REF character );
INLINE bool_t chr_attached( PChr lst, size_t lst_size, CHR_REF character );
INLINE bool_t chr_has_inventory( PChr lst, size_t lst_size, CHR_REF character );
INLINE bool_t chr_is_invisible( PChr lst, size_t lst_size, CHR_REF character );
INLINE bool_t chr_using_slot( PChr lst, size_t lst_size, CHR_REF character, SLOT slot );

INLINE CHR_REF chr_get_nextinpack( PChr lst, size_t lst_size, CHR_REF ichr );
INLINE CHR_REF chr_get_onwhichplatform( PChr lst, size_t lst_size, CHR_REF ichr );
INLINE CHR_REF chr_get_inwhichpack( PChr lst, size_t lst_size, CHR_REF ichr );
INLINE CHR_REF chr_get_attachedto( PChr lst, size_t lst_size, CHR_REF ichr );
INLINE CHR_REF chr_get_holdingwhich( PChr lst, size_t lst_size, CHR_REF ichr, SLOT slot );

INLINE CHR_REF chr_get_aitarget( PChr lst, size_t lst_size, CChr * pchr );
INLINE CHR_REF chr_get_aiowner( PChr lst, size_t lst_size, CChr * pchr );
INLINE CHR_REF chr_get_aichild( PChr lst, size_t lst_size, CChr * pchr );
INLINE CHR_REF chr_get_aiattacklast( PChr lst, size_t lst_size, CChr * pchr );
INLINE CHR_REF chr_get_aibumplast( PChr lst, size_t lst_size, CChr * pchr );
INLINE CHR_REF chr_get_aihitlast( PChr lst, size_t lst_size, CChr * pchr );

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

INLINE const SLOT   grip_to_slot( GRIP g );
INLINE const GRIP   slot_to_grip( SLOT s );
INLINE const Uint16 slot_to_latch( PChr lst, size_t count, Uint16 object, SLOT s );
INLINE const Uint16 slot_to_offset( SLOT s );


// Character data

extern Uint16          chrcollisionlevel;

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
CHR_REF ChrList_get_free( struct CGame_t * gs, CHR_REF irequest );
void ChrList_free_one( struct CGame_t * gs, CHR_REF character );

bool_t make_one_character_matrix( ChrList_t chrlst, size_t chrlst_size, CChr * cnt );
void chr_free_inventory( PChr lst, size_t lst_size, CHR_REF character );
bool_t make_one_weapon_matrix( ChrList_t chrlst, size_t chrlst_size, Uint16 cnt );
void make_character_matrices( struct CGame_t * gs );

Uint32 chr_hitawall( struct CGame_t * gs, CChr * pchr, vect3 * norm );
void play_action( struct CGame_t * gs, CHR_REF character, ACTION action, bool_t ready );
void set_frame( struct CGame_t * gs, CHR_REF character, Uint16 frame, Uint8 lip );
bool_t detach_character_from_mount( struct CGame_t * gs, CHR_REF character, bool_t ignorekurse, bool_t doshop );
void drop_money( struct CGame_t * gs, CHR_REF character, Uint16 money );

void damage_character( struct CGame_t * gs, CHR_REF character, Uint16 direction,
                       PAIR * ppair, DAMAGE damagetype, TEAM_REF team,
                       CHR_REF attacker, Uint16 effects );
void kill_character( struct CGame_t * gs, CHR_REF character, CHR_REF killer );
void spawn_poof( struct CGame_t * gs, CHR_REF character, OBJ_REF profile );


void tilt_characters_to_terrain(struct CGame_t * gs);
CHR_REF spawn_one_character( struct CGame_t * gs, vect3 pos, OBJ_REF iobj, TEAM_REF team,
                            Uint8 skin, Uint16 facing, char *name, CHR_REF override );

void respawn_character( struct CGame_t * gs, CHR_REF character );
Uint16 change_armor( struct CGame_t * gs, CHR_REF character, Uint16 skin );
void change_character( struct CGame_t * gs, CHR_REF cnt, OBJ_REF profile, Uint8 skin,
                       Uint8 leavewhich );
bool_t cost_mana( struct CGame_t * gs, CHR_REF character, int amount, CHR_REF killer );
bool_t attach_character_to_mount( struct CGame_t * gs, CHR_REF character, CHR_REF mount, SLOT slot );
CHR_REF stack_in_pack( struct CGame_t * gs, CHR_REF item, CHR_REF character );
bool_t pack_add_item( struct CGame_t * gs, CHR_REF item, CHR_REF character );
CHR_REF pack_get_item( struct CGame_t * gs,CHR_REF character, SLOT slot, bool_t ignorekurse );
void drop_keys( struct CGame_t * gs,CHR_REF character );
void drop_all_items( struct CGame_t * gs,CHR_REF character );
bool_t chr_grab_stuff( struct CGame_t * gs,CHR_REF chara, SLOT slot, bool_t people );
void chr_swipe( struct CGame_t * gs,Uint16 cnt, SLOT slot );
void move_characters( struct CGame_t * gs,float dUpdate );

OBJ_REF object_generate_index( struct CGame_t * gs,char *szLoadName );

CAP_REF CapList_load_one( struct CGame_t * gs, const char * szModpath, const char *szObjectname, CAP_REF irequest );

bool_t chr_bdata_reinit(CChr * pchr, BData * pbd);

int fget_skin( char * szModpath, const char * szObjectname );


void calc_cap_experience( struct CGame_t * gs, CHR_REF object );
int calc_chr_experience( struct CGame_t * gs, CHR_REF object, float level );
float calc_chr_level( struct CGame_t * gs, CHR_REF object );


bool_t chr_calculate_bumpers( struct CGame_t * gs, CChr * pchr, int level);

void flash_character_height( struct CGame_t * gs, CHR_REF character, Uint8 valuelow, Sint16 low,
                             Uint8 valuehigh, Sint16 high );

void flash_character( struct CGame_t * gs, CHR_REF character, Uint8 value );

void signal_target( struct CGame_t * gs, CHR_REF target, Uint16 upper, Uint16 lower );
void signal_team( struct CGame_t * gs, CHR_REF character, Uint32 order );
void signal_idsz_index( struct CGame_t * gs, Uint32 order, IDSZ idsz, IDSZ_INDEX index );

bool_t ai_state_advance_wp(AI_STATE * a, bool_t do_atlastwaypoint);