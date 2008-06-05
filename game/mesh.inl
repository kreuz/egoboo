#pragma once

#include "Mesh.h"
#include "char.h"
#include "particle.h"
#include "game.h"

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE const BUMPLIST_NODE * bumplist_node_new(BUMPLIST_NODE * n)
{
  if(NULL == n) return NULL;

  n->next = INVALID_BUMPLIST_NODE;
  n->ref  = INVALID_BUMPLIST_NODE;

  return n;
}

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_node_get_ref(BUMPLIST_NODE * n)
{
  if(NULL == n) return INVALID_BUMPLIST_NODE;

  return n->ref;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE const BUMPLIST * bumplist_new(BUMPLIST * b)
{
  if(NULL == b) return NULL;

  memset(b, 0, sizeof(BUMPLIST));

  return b;
};

//--------------------------------------------------------------------------------------------
INLINE const void bumplist_delete(BUMPLIST * b)
{
  if(NULL == b || !b->allocated) return;

  b->allocated = bfalse;

  if(0 == b->num_blocks) return;

  b->num_blocks = 0;

  b->free_count = 0;
  FREE(b->free_lst);
  FREE(b->node_lst);

  FREE(b->num_chr);
  FREE(b->chr_ref);

  FREE(b->num_prt);
  FREE(b->prt_ref);
}

//--------------------------------------------------------------------------------------------
INLINE const BUMPLIST * bumplist_renew(BUMPLIST * b)
{
  if(NULL == b) return NULL;

  bumplist_delete(b);
  return bumplist_new(b);
}

//--------------------------------------------------------------------------------------------
INLINE const bool_t bumplist_clear(BUMPLIST * b)
{
  Uint32 i;

  if(NULL == b || !b->allocated) return bfalse;

  // initialize the data
  for(i=0; i < b->num_blocks; i++)
  {
    b->num_chr[i] = 0;
    b->chr_ref[i].next = INVALID_BUMPLIST_NODE;
    b->chr_ref[i].ref  = MAXCHR;

    b->num_prt[i] = 0;
    b->prt_ref[i].next = INVALID_BUMPLIST_NODE;
    b->prt_ref[i].ref  = MAXPRT;
  }

  return btrue;
}

//--------------------------------------------------------------------------------------------
INLINE const bool_t bumplist_allocate(BUMPLIST * b, int size)
{
  if(NULL == b) return bfalse;

  if(size <= 0)
  {
    bumplist_renew(b);
  }
  else
  {
    b->num_chr = (Uint16       *)calloc(size, sizeof(Uint16));
    b->chr_ref = (BUMPLIST_NODE*)calloc(size, sizeof(BUMPLIST_NODE));

    b->num_prt = (Uint16       *)calloc(size, sizeof(Uint16));
    b->prt_ref = (BUMPLIST_NODE*)calloc(size, sizeof(BUMPLIST_NODE));

    if(NULL != b->num_chr && NULL != b->chr_ref && NULL != b->num_prt && NULL != b->prt_ref)
    {
      b->num_blocks = size;
      b->allocated  = btrue;
      bumplist_clear(b);
    }
  }

  return btrue;
};

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_get_free(BUMPLIST * b)
{
  if(NULL == b || b->free_count<=0) return INVALID_BUMPLIST_NODE;

  b->free_count--;
  return b->free_lst[b->free_count];
}

//--------------------------------------------------------------------------------------------
INLINE const bool_t bumplist_return_free(BUMPLIST * b, Uint32 ref)
{
  if(NULL == b || !b->initialized || ref >= b->free_max) return bfalse;

  b->free_lst[b->free_count] = ref;
  b->free_count++;

  return btrue;
}

//--------------------------------------------------------------------------------------------
INLINE const bool_t bumplist_insert_chr(BUMPLIST * b, Uint32 block, CHR_REF chr_ref)
{
  // BB > insert a character into the bumplist at fanblock.

  Uint32 ref;

  if(NULL == b || !b->initialized) return bfalse;

  ref = bumplist_get_free(&bumplist);
  if( INVALID_BUMPLIST_NODE == ref ) return bfalse;

  // place this as the first node in the list
  b->node_lst[ref].ref  = chr_ref;
  b->node_lst[ref].next = b->chr_ref[block].next;

  // make the list point to out new node
  b->chr_ref[block].next = ref;
  b->chr_ref[block].ref  = MAXCHR;

  // increase the count for this block
  b->num_chr[block]++;

  return btrue;
}

//--------------------------------------------------------------------------------------------
INLINE const bool_t bumplist_insert_prt(BUMPLIST * b, Uint32 block, PRT_REF prt_ref)
{
  // BB > insert a particle into the bumplist at fanblock.

  Uint32 ref;

  if(NULL == b || !b->initialized) return bfalse;

  ref = bumplist_get_free(&bumplist);
  if( INVALID_BUMPLIST_NODE == ref ) return bfalse;

  // place this as the first node in the list
  b->node_lst[ref].ref  = prt_ref;
  b->node_lst[ref].next = b->prt_ref[block].next;

  // make the list point to out new node
  b->prt_ref[block].next = ref;
  b->prt_ref[block].ref  = MAXPRT;

  // increase the count for this block
  b->num_chr[block]++;

  return btrue;
}

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_get_next( BUMPLIST * b, Uint32 node )
{
  if(NULL == b || !b->initialized || INVALID_BUMPLIST_NODE == node) return INVALID_BUMPLIST_NODE;

  return b->node_lst[node].next;
}

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_get_next_chr( CGame * gs, BUMPLIST * b, Uint32 node )
{
  Uint32  nodenext;
  CHR_REF bumpnext;

  if(NULL == b || !b->initialized || INVALID_BUMPLIST_NODE == node) return INVALID_BUMPLIST_NODE;

  nodenext = b->node_lst[node].next;
  bumpnext = b->node_lst[nodenext].ref;

  while( INVALID_BUMPLIST_NODE != nodenext && !VALID_CHR(gs->ChrList, bumpnext) )
  {
    nodenext = b->node_lst[node].next;
    bumpnext = b->node_lst[nodenext].ref;
  }

  return nodenext;
}

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_get_next_prt( CGame * gs, BUMPLIST * b, Uint32 node )
{
  Uint32  nodenext;
  CHR_REF bumpnext;

  if(NULL == b || !b->initialized || INVALID_BUMPLIST_NODE == node) return INVALID_BUMPLIST_NODE;

  nodenext = b->node_lst[node].next;
  bumpnext = b->node_lst[nodenext].ref;

  while( INVALID_BUMPLIST_NODE != nodenext && !VALID_PRT(gs->PrtList, bumpnext) )
  {
    nodenext = b->node_lst[node].next;
    bumpnext = b->node_lst[nodenext].ref;
  }

  return nodenext;
}

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_get_chr_head(BUMPLIST * b, Uint32 block)
{
  if(NULL == b || !b->initialized) return INVALID_BUMPLIST_NODE;
  if(block > b->num_blocks)  return INVALID_BUMPLIST_NODE;

  return b->chr_ref[block].next;
};

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_get_prt_head(BUMPLIST * b, Uint32 block)
{
  if(NULL == b || !b->initialized) return INVALID_BUMPLIST_NODE;
  if(block > b->num_blocks)  return INVALID_BUMPLIST_NODE;

  return b->prt_ref[block].next;
}

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_get_chr_count(BUMPLIST * b, Uint32 block)
{
  if(NULL == b || !b->initialized) return INVALID_BUMPLIST_NODE;
  if(block > b->num_blocks)  return INVALID_BUMPLIST_NODE;

  return b->num_chr[block];
};

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_get_prt_count(BUMPLIST * b, Uint32 block)
{
  if(NULL == b || !b->initialized) return INVALID_BUMPLIST_NODE;
  if(block > b->num_blocks)  return INVALID_BUMPLIST_NODE;

  return b->num_prt[block];
}

//--------------------------------------------------------------------------------------------
INLINE const Uint32 bumplist_get_ref(BUMPLIST * b, Uint32 node)
{
  if(NULL == b || !b->initialized) return INVALID_BUMPLIST_NODE;
  if(node > b->free_max)    return INVALID_BUMPLIST_NODE;

  return b->node_lst[node].ref;
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE const bool_t mesh_fan_is_in_renderlist( MESH_FAN * mf_list, int fan )
{
  if ( INVALID_FAN == fan ) return bfalse;

  return mf_list[fan].inrenderlist;
};

//--------------------------------------------------------------------------------------------
INLINE void mesh_fan_remove_renderlist( MESH_FAN * mf_list, int fan )
{
  mf_list[fan].inrenderlist = bfalse;
};

//--------------------------------------------------------------------------------------------
INLINE void mesh_fan_add_renderlist( MESH_FAN * mf_list, int fan )
{
  mf_list[fan].inrenderlist = btrue;
};


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE const float mesh_clip_x( MESH_INFO * mi, float x )
{
  if ( x <      0.0f )  x = 0.0f;
  if ( x > mi->edge_x )  x = mi->edge_x;

  return x;
}

//--------------------------------------------------------------------------------------------
INLINE const float mesh_clip_y( MESH_INFO * mi, float y )
{
  if ( y <      0.0f )  y = 0.0f;
  if ( y > mi->edge_y )  y = mi->edge_y;

  return y;
}

//--------------------------------------------------------------------------------------------
INLINE const int mesh_clip_fan_x( MESH_INFO * mi, int ix )
{
  if ( ix < 0 )  ix = 0;
  if ( ix > mi->size_x - 1 )  ix = mi->size_x - 1;

  return ix;
};

//--------------------------------------------------------------------------------------------
INLINE const int mesh_clip_fan_y( MESH_INFO * mi, int iy )
{
  if ( iy < 0 )  iy = 0;
  if ( iy > mi->size_y - 1 )  iy = mi->size_y - 1;

  return iy;
};

//--------------------------------------------------------------------------------------------
INLINE const int mesh_clip_block_x( MESH_INFO * mi, int ix )
{
  if ( ix < 0 )  ix = 0;
  if ( ix > ( mi->size_x >> 2 ) - 1 )  ix = ( mi->size_x >> 2 ) - 1;

  return ix;
};

//--------------------------------------------------------------------------------------------
INLINE const int mesh_clip_block_y( MESH_INFO * mi, int iy )
{
  if ( iy < 0 )  iy = 0;
  if ( iy > ( mi->size_y >> 2 ) - 1 )  iy = ( mi->size_y >> 2 ) - 1;

  return iy;
};

//--------------------------------------------------------------------------------------------
INLINE const bool_t mesh_check( MESH_INFO * mi, float x, float y )
{
  if ( x < 0 || x > mi->edge_x ) return bfalse;
  if ( y < 0 || y > mi->edge_x ) return bfalse;

  return btrue;
}


//--------------------------------------------------------------------------------------------
INLINE void mesh_set_colora( CGame * gs, int fan_x, int fan_y, int color )
{
  Uint32 cnt, fan, vert, numvert;

  MESH_INFO * mi      = &(gs->mesh);
  MESH_FAN  * mf_list = gs->Mesh_Mem.fanlst;

  fan = mesh_convert_fan( mi, fan_x, fan_y );
  if ( INVALID_FAN == fan ) return;

  vert = mf_list[fan].vrt_start;
  cnt = 0;
  numvert = Mesh_Cmd[mf_list[fan].type].vrt_count;
  while ( cnt < numvert )
  {
    gs->Mesh_Mem.vrt_ar_fp8[vert] = color;
    gs->Mesh_Mem.vrt_ag_fp8[vert] = color;
    gs->Mesh_Mem.vrt_ab_fp8[vert] = color;
    vert++;
    cnt++;
  }
}

//--------------------------------------------------------------------------------------------
INLINE const Uint32 mesh_get_fan( CGame * gs, vect3 pos )
{
  // BB > find the tile under <pos.x,pos.y>, but MAKE SURE we have the right tile.

  Uint32 ivert, testfan = INVALID_FAN;
  float minx, maxx, miny, maxy;
  int i, ix, iy;
  bool_t bfound = bfalse;

  MESH_FAN  * mf_list = gs->Mesh_Mem.fanlst;
  MESH_INFO * mi      = &(gs->mesh);

  if ( !mesh_check( mi, pos.x, pos.y ) )
    return testfan;

  ix = MESH_FLOAT_TO_FAN( pos.x );
  iy = MESH_FLOAT_TO_FAN( pos.y );

  testfan = mesh_convert_fan( &(gs->mesh), ix, iy );
  if(INVALID_FAN == testfan) return testfan;

  ivert = mf_list[testfan].vrt_start;
  minx = maxx = gs->Mesh_Mem.vrt_x[ivert];
  miny = maxy = gs->Mesh_Mem.vrt_y[ivert];
  for ( i = 1;i < 4;i++, ivert++ )
  {
    minx = MIN( minx, gs->Mesh_Mem.vrt_x[ivert] );
    maxx = MAX( maxx, gs->Mesh_Mem.vrt_x[ivert] );

    miny = MIN( miny, gs->Mesh_Mem.vrt_y[ivert] );
    maxy = MAX( maxy, gs->Mesh_Mem.vrt_y[ivert] );
  };

  if ( pos.x < minx ) { ix--; bfound = btrue; }
  else if ( pos.x > maxx ) { ix++; bfound = btrue; }

  if ( pos.y < miny ) { iy--; bfound = btrue; }
  else if ( pos.y > maxy ) { iy++; bfound = btrue; }

  testfan = mesh_convert_fan( &(gs->mesh), ix, iy );

  return testfan;
};

//--------------------------------------------------------------------------------------------
INLINE const Uint32 mesh_get_block( MESH_INFO * mi, vect3 pos )
{
  // BB > find the block under <x,y>

  return mesh_convert_block( mi, MESH_FLOAT_TO_BLOCK( pos.x ), MESH_FLOAT_TO_BLOCK( pos.y ) );
};



//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE const bool_t mesh_fan_clear_bits( CGame * gs, int fan_x, int fan_y, Uint32 bits )
{
  bool_t retval = bfalse;
  Uint32 fan;
  MESH_FAN  * mf_list = gs->Mesh_Mem.fanlst;
  MESH_INFO * mi      = &(gs->mesh);

  fan = mesh_convert_fan( mi, fan_x, fan_y );
  if ( INVALID_FAN == fan ) return retval;

  retval = mesh_has_some_bits( mf_list, fan, bits );

  mf_list[fan].fx &= ~bits;

  return retval;
};

//--------------------------------------------------------------------------------------------
INLINE const bool_t mesh_fan_add_bits( CGame * gs, int fan_x, int fan_y, Uint32 bits )
{
  bool_t retval = bfalse;
  Uint32 fan;

  MESH_FAN  * mf_list = gs->Mesh_Mem.fanlst;
  MESH_INFO * mi      = &(gs->mesh);

  fan = mesh_convert_fan( mi, fan_x, fan_y );
  if ( INVALID_FAN == fan ) return retval;

  retval = MISSING_BITS( mf_list[fan].fx, bits );

  mf_list[fan].fx |= bits;

  return retval;
};

//--------------------------------------------------------------------------------------------
INLINE const bool_t mesh_fan_set_bits( CGame * gs, int fan_x, int fan_y, Uint32 bits )
{
  bool_t retval = bfalse;
  Uint32 fan;

  MESH_FAN  * mf_list = gs->Mesh_Mem.fanlst;
  MESH_INFO * mi      = &(gs->mesh);

  fan = mesh_convert_fan( mi, fan_x, fan_y );
  if ( INVALID_FAN == fan ) return retval;

  retval = ( mf_list[fan].fx != bits );

  mf_list[fan].fx = bits;

  return retval;
};


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE const int mesh_bump_tile( CGame * gs, int fan_x, int fan_y )
{
  Uint32 fan;

  MESH_FAN  * mf_list = gs->Mesh_Mem.fanlst;
  MESH_INFO * mi      = &(gs->mesh);

  fan = mesh_convert_fan( mi, fan_x, fan_y );
  if ( INVALID_FAN == fan ) return 0;

  mf_list[fan].tile++;

  return mf_list[fan].tile;
}

//--------------------------------------------------------------------------------------------
INLINE const Uint16 mesh_get_tile( CGame * gs, int fan_x, int fan_y )
{
  Uint32 fan;

  MESH_FAN  * mf_list = gs->Mesh_Mem.fanlst;
  MESH_INFO * mi      = &(gs->mesh);

  fan = mesh_convert_fan( mi, fan_x, fan_y );
  if ( INVALID_FAN == fan ) return INVALID_TILE;

  return mf_list[fan].tile;
}

//--------------------------------------------------------------------------------------------
INLINE const bool_t mesh_set_tile( CGame * gs, int fan_x, int fan_y, Uint32 become )
{
  bool_t retval = bfalse;
  Uint32 fan;

  MESH_FAN  * mf_list = gs->Mesh_Mem.fanlst;
  MESH_INFO * mi      = &(gs->mesh);

  fan = mesh_convert_fan( mi, fan_x, fan_y );
  if ( INVALID_FAN == fan ) return retval;

  if ( become != 0 )
  {
    mf_list[fan].tile = become;
    retval = btrue;
  }

  return retval;
}

//--------------------------------------------------------------------------------------------
INLINE const Uint32 mesh_convert_fan( MESH_INFO * mi, int fan_x, int fan_y )
{
  // BB > convert <fan_x,fan_y> to a fanblock

  if ( fan_x < 0 || fan_x >= mi->size_x || fan_y < 0 || fan_y >= mi->size_y ) return INVALID_FAN;

  return fan_x + mi->Fan_X[fan_y];
};

//--------------------------------------------------------------------------------------------
INLINE const Uint32 mesh_convert_block( MESH_INFO * mi, int block_x, int block_y )
{
  // BB > convert <block_x,block_y> to a fanblock

  if ( block_x < 0 || block_x > ( mi->size_x >> 2 ) || block_y < 0 || block_y > ( mi->size_y >> 2 ) ) return INVALID_FAN;

  return block_x + mi->Block_X[block_y];
};


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
INLINE const Uint32 mesh_get_bits( MESH_FAN * mf_list, int fan, Uint32 bits )
{
  if ( INVALID_FAN == fan ) return 0;

  return mf_list[fan].fx & bits;
};

//--------------------------------------------------------------------------------------------
INLINE const bool_t mesh_has_some_bits( MESH_FAN * mf_list, int fan, Uint32 bits )
{
  if ( INVALID_FAN == fan ) return 0;

  return HAS_SOME_BITS( mf_list[fan].fx, bits );
};

//--------------------------------------------------------------------------------------------
INLINE const bool_t mesh_has_no_bits( MESH_FAN * mf_list, int fan, Uint32 bits )
{
  if ( INVALID_FAN == fan ) return 0;

  return HAS_NO_BITS( mf_list[fan].fx, bits );
};

//--------------------------------------------------------------------------------------------
INLINE const bool_t mesh_has_all_bits( MESH_FAN * mf_list, int fan, Uint32 bits )
{
  if ( INVALID_FAN == fan ) return 0;

  return HAS_ALL_BITS( mf_list[fan].fx, bits );
};

//--------------------------------------------------------------------------------------------
INLINE const float mesh_fraction_x( MESH_INFO * mi, float x )
{
  return x / mi->edge_x;
};

//--------------------------------------------------------------------------------------------
INLINE const float mesh_fraction_y( MESH_INFO * mi, float y )
{
  return y / mi->edge_y;
};


//--------------------------------------------------------------------------------------------
INLINE const Uint8 mesh_get_twist( MESH_FAN * mf_list, int fan )
{
  Uint8 retval = 0x77;

  if ( INVALID_FAN != fan )
  {
    retval = mf_list[fan].twist;
  }

  return retval;
};