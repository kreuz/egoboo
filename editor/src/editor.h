/*******************************************************************************
*  EDITOR.H                                                                    *
*	- General definitions for the editor                	                   *
*									                                           *
*   Copyright (C) 2010  Paul Mueller <pmtech@swissonline.ch>                   *
*                                                                              *
*   This program is free software; you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License as published by       *
*   the Free Software Foundation; either version 2 of the License, or          *
*   (at your option) any later version.                                        *
*                                                                              *
*   This program is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*   GNU Library General Public License for more details.                       *
*                                                                              *
*   You should have received a copy of the GNU General Public License          *
*   along with this program; if not, write to the Free Software                *
*   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. *
*                                                                              *
*                                                                              *
*******************************************************************************/

#ifndef _EDITOR_H_
#define _EDITOR_H_

/*******************************************************************************
* DEFINES								                                       *
*******************************************************************************/

#define MAXLEVEL 255
#define MINLEVEL 50

#define MAXTILE 256             //

#define MAXSELECT 2560          // Max points that can be select_vertsed

#define FADEBORDER 64           // Darkness at the edge of map
#define ONSIZE 264              // Max size of raise mesh

#define MAXMESHTYPE                     64          // Number of mesh types
#define MAXMESHLINE                     64          // Number of lines in a fan schematic
#define MAXMESHVERTICES                 16          // Max number of vertices in a fan
#define MAXMESHFAN                      (128*128)   // Size of map in fans
#define MAXTOTALMESHVERTICES            ((MAXMESHFAN*(MAXMESHVERTICES / 2)) - 10)
#define MAXMESHCOMMAND                  4             // Draw up to 4 fans
#define MAXMESHCOMMANDENTRIES           32            // Fansquare command list size
#define MAXMESHCOMMANDSIZE              32            // Max trigs in each command

#define MPDFX_REF       0x00    /* MeshFX   */
#define MPDFX_SHA       0x01    
#define MPDFX_DRAWREF   0x02    
#define MPDFX_ANIM      0x04    
#define MPDFX_WATER     0x08    
#define MPDFX_WALL      0x10    
#define MPDFX_IMPASS    0x20    
#define MPDFX_DAMAGE    0x40    
#define MPDFX_SLIPPY    0x80    

#define COMMAND_TEXTUREHI_FLAG 0x20

// Editor modes: How to draw the map
#define EDIT_DRAWMODE_SOLID     0x01        /* Draw solid, yes/no       */
#define EDIT_DRAWMODE_TEXTURED  0x02        /* Draw textured, yes/no    */
#define EDIT_DRAWMODE_LIGHTMAX  0x04        /* Is drawn all white       */    

/* ---- For MAP_INFO_T: Kind of additional info ---- */
#define MAP_INFO_NONE       0x00
#define MAP_INFO_SPAWN      0x01
#define MAP_INFO_PASSAGE    0x02
#define MAP_INFO_CHOSEN     0x40        /* TODO: These fans are chosen      */ 

/* --- different edit modes --- */
#define EDITOR_TOOL_OFF     ((char)1)   /* 'View' map                       */
#define EDITOR_TOOL_MAP     ((char)2)   /* 'Carve' out map with mouse       */
                                        /* EDITMAIN_EDIT_SIMPLE             */
#define EDITOR_TOOL_FAN     ((char)3)   /* Edit fan geometry (base -types)  */
                                        /* EDITMAIN_EDIT_FREE               */
#define EDITOR_TOOL_FAN_FX  ((char)4)   /* Edit the flags of a fan          */
                                        /* EDITMAIN_EDIT_FX                 */   
#define EDITOR_TOOL_FAN_TEX ((char)5)   /* Texture of a fan                 */ 
                                        /* EDITMAIN_EDIT_TEXTURE            */
#define EDITOR_TOOL_PASSAGE ((char)6)   
#define EDITOR_TOOL_OBJECT  ((char)7)
#define EDITOR_TOOL_MODULE  ((char)8)   /* Change info in module description */
#define EDITOR_TOOL_VERTEX  ((char)9)   /* Edit vertices of a fan            */

/*******************************************************************************
* TYPEDEFS							                                           *
*******************************************************************************/

typedef struct {

    unsigned char ref;      /* Light reference      */
    float u, v;             /* Texture mapping info: Has to be multiplied by 'sub-uv' */    
    float x, y, z;          /* Default position of vertex at 0, 0, 0    */

} EDITOR_VTX;

typedef struct {

    char *name;                             // Name of this fan type
    char default_fx;                        // Default flags to set, if new fan (for walls)
    unsigned char default_tx_no;            // Default texture to set
    char  numvertices;			            // meshcommandnumvertices
    int   count;			                // meshcommands
    int   size[MAXMESHCOMMAND];             // meshcommandsize
    int   vertexno[MAXMESHCOMMANDENTRIES];  // meshcommandvrt, number of vertex
    float uv[MAXMESHVERTICES * 2];          // meshcommandu, meshcommandv
    float biguv[MAXMESHVERTICES * 2];       // meshcommandu, meshcommandv
                                            // For big texture images
    EDITOR_VTX vtx[MAXMESHVERTICES];        // Holds it's u/v position and default extent x/y/z
    
} COMMAND_T;

typedef struct {
  
    unsigned char tx_no;    /* Number of texture:                           */
                            /* (tx_no >> 6) & 3: Number of wall texture     */
                            /* tx_no & 0x3F:     Number of part of texture  */ 
    unsigned char tx_flags; /* Special flags                                */
    unsigned char fx;		/* Tile special effects flags                   */
    char          type;     /* Tile fan type (index into COMMAND_T)         */
    unsigned char twist;    /* Surface normal for slopes (?)                */
    
} FANDATA_T;

typedef struct {

    float x, y, z;          /* Vertex x / y / z                     */           
    unsigned char a;        /* Ambient lighting                     */

} MESH_VTX_T;              /* Planned for later adjustement how to store vertices */

typedef struct {

    unsigned char map_loaded;   // A map is loaded  into this struct
    unsigned char draw_mode;    // Flags for display of map
    
    int numvert;                // Number of vertices in map
    int numfreevert;            // Number of free vertices for edit
    int tiles_x, tiles_y;       // Size of mesh in tiles          
    int numfan;                 // Size of map in 'fans' (tiles_x * tiles_y)
    int watershift;             // Depends on size of map
    int minimap_w,              // For drawing in 2D on minimap         
        minimap_h;
    int minimap_tile_w;         // Size of tile in minimap
    
    float edgex;                // Borders of mesh
    float edgey;                
    float edgez;                

    FANDATA_T fan[MAXMESHFAN];                      // Fan desription                
    
    float vrtx[MAXTOTALMESHVERTICES + 10];          // Vertex position
    float vrty[MAXTOTALMESHVERTICES + 10];          //
    float vrtz[MAXTOTALMESHVERTICES + 10];          // Vertex elevation
    
    unsigned char vrta[MAXTOTALMESHVERTICES + 10];  // Vertex base light, 0=unused
    
    int  vrtstart[MAXMESHFAN];                      // First vertex of given fan  
    
    /* Prepare usage of dynamically allocated memory */
    char *data;                 // Allcated memory for map in one chunk (filesize)
    MESH_VTX_T *vrt;            // Pointer on list of map vertices

} MESH_T;

typedef struct {

    char type;          /* Type of map_info: MAP_INFO_NONE, MAP_INFO_SPAWN  MAP_INFO_PASSAGE */
    short int number;   /* Number of spawn point / passage chosen           */    

} MAP_INFO_T;   /* Information about spawn points an passages for chossing and display */

/*******************************************************************************
* CODE 								                                           *
*******************************************************************************/

#endif /* _EDITOR_H_	*/

