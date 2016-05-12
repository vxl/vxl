////////////////////////////////////////////////////////////////////////////////
// Cast Ray Bit
// Main cast ray function for fixed grid of shallow octree ray tracing.
//
// Note: This is a bare-bones skeleton for ray tracing.  It is necessary to
// define two preprocessor variables for this to do anything useful:
//    - AUX_ARGS : list of additional arguments to to this function (for instance data arrays)
//    - STEP_CELL : function and signature to be substituted in at the voxel level
//
// This function also depends on 'bit_tree_library_functions.cl'
////////////////////////////////////////////////////////////////////////////////

//example defines (passed in on C++ side)
//#define STEP_CELL step_cell_render(aux_args.mixture_array,aux_args.alpha_array,data_ptr,d,vis,aux_args.expected_int);
#define BLOCK_EPSILON .006125f
#define TREE_EPSILON  .005f
#define GRAZING_RAY_D 0.001f

////////////////////////////////////////////////////////////////////////////////
//Helper methods (that will hopefully, one day, become vector ops)
////////////////////////////////////////////////////////////////////////////////
inline float calc_pos(float t, float ray_o, float ray_d)
{
  return ray_o + (t + TREE_EPSILON)*ray_d;
}

inline float calc_tfar( float ray_ox, float ray_oy, float ray_oz,
                        float ray_dx, float ray_dy, float ray_dz,
                        float max_facex, float max_facey, float max_facez)
{
    // prevent ray from leaving cell along direction with very small delta value.
    // will cause exit t values slightly too large in grazing cases
    float tfar_x = fabs(ray_dx) > GRAZING_RAY_D? (max_facex - ray_ox) / ray_dx : MAXFLOAT;
    float tfar_y = fabs(ray_dy) > GRAZING_RAY_D? (max_facey - ray_oy) / ray_dy : MAXFLOAT;
    float tfar_z = fabs(ray_dz) > GRAZING_RAY_D? (max_facez - ray_oz) / ray_dz : MAXFLOAT;
    return min(min(tfar_x, tfar_y), tfar_z);
}

inline float calc_tnear(float ray_ox, float ray_oy, float ray_oz,
                        float ray_dx, float ray_dy, float ray_dz,
                        float min_facex, float min_facey, float min_facez)
{
    // prevent ray from entering cell along direction with very small delta value.
    // will cause entrance t values slightly too small in grazing cases
    float tnear_x = fabs(ray_dx) > GRAZING_RAY_D? (min_facex - ray_ox) / ray_dx : -MAXFLOAT;
    float tnear_y = fabs(ray_dy) > GRAZING_RAY_D? (min_facey - ray_oy) / ray_dy : -MAXFLOAT;
    float tnear_z = fabs(ray_dz) > GRAZING_RAY_D? (min_facez - ray_oz) / ray_dz : -MAXFLOAT;
    return max(max(tnear_x, tnear_y), tnear_z);
}

//requires float position
inline void calc_cell_min( float* cell_minx, float* cell_miny, float* cell_minz,
                           float posx, float posy, float posz, int4 dims)
{
  (*cell_minx) = clamp(floor(posx), 0.0f, dims.x-1.0f);
  (*cell_miny) = clamp(floor(posy), 0.0f, dims.y-1.0f);
  (*cell_minz) = clamp(floor(posz), 0.0f, dims.z-1.0f);
}

//requires cell_minx
inline int calc_blkI(float cell_minx, float cell_miny, float cell_minz, int4 dims)
{
  return convert_int(cell_minz + (cell_miny + cell_minx*dims.y)*dims.z);
}

//calc subblock exit point parameter (t) given cell_min and cell_length
inline float calc_cell_exit(float cell_minx, float cell_miny, float cell_minz, float cell_len,
                            float ray_ox, float ray_oy, float ray_oz,
                            float ray_dx, float ray_dy, float ray_dz)
{
  //calculate the exit along the sub block exit point (t_subblock_exit)
  cell_minx = (ray_dx > 0) ? cell_minx+cell_len : cell_minx;
  cell_miny = (ray_dy > 0) ? cell_miny+cell_len : cell_miny;
  cell_minz = (ray_dz > 0) ? cell_minz+cell_len : cell_minz;
  return calc_tfar(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, cell_minx, cell_miny, cell_minz);
}

////////////////////////////////////////////////////////////////////////////////
// END Helper methods
////////////////////////////////////////////////////////////////////////////////

void cast_ray(
          //---- RAY ARGUMENTS -------------------------------------------------
          int i, int j,                                     //pixel information
          float ray_ox, float ray_oy, float ray_oz,         //ray origin
          float ray_dx, float ray_dy, float ray_dz,         //ray direction

          //---- SCENE ARGUMENTS------------------------------------------------
          __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
          __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16

          //---- UTILITY ARGUMENTS----------------------------------------------
          __local     uchar16            * local_tree,      //local tree for traversing
          __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
          __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                      float              * vis,             //passed in as starting visibility

          //----aux arguments defined by host at compile time-------------------
          AuxArgs aux_args,
          float tnearf,
          float tfarf)
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->dims.x) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->dims.y) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->dims.z) : 0.0f;
  float tfar = calc_tfar(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, max_facex, max_facey, max_facez);
  float min_facex = (ray_dx < 0.0f) ? (linfo->dims.x) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->dims.y) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->dims.z) : 0.0f;
  float tblock = calc_tnear(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, min_facex, min_facey, min_facez);

  float tblockfixed = tblock;
  //tblock = (tblock > 0) ? tblock :0;    //make sure tnear is at least 0...

  tblock = (tblock > tnearf) ? tblock :tnearf;    //make sure tnear is at least 0...
  tfar = (tfar <  tfarf ) ? tfar : tfarf;
#ifdef REVERSE
  {
  //: code to reverse
  ray_ox = ray_ox + 2*tfar*ray_dx;
  ray_oy = ray_oy + 2*tfar*ray_dy;
  ray_oz = ray_oz + 2*tfar*ray_dz;

  ray_dx = -ray_dx;
  ray_dy = -ray_dy;
  ray_dz = -ray_dz;

  float temp = tblock;
  tblock = tfar;
  tfar = 2*tfar - temp;
  }
#endif
  tfar -= BLOCK_EPSILON;   //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)

  if (tfar <= tblock)
    return;

  //calculate entry point here, entry sub_block, and its index
  min_facex = calc_pos(tblock, ray_ox, ray_dx); //(ray_ox + (tblock + TREE_EPSILON)*ray_dx);
  min_facey = calc_pos(tblock, ray_oy, ray_dy); //(ray_oy + (tblock + TREE_EPSILON)*ray_dy);
  min_facez = calc_pos(tblock, ray_oz, ray_dz); //(ray_oz + (tblock + TREE_EPSILON)*ray_dz);

  // verify that position is within bounds of block
  if ( (min_facex < 0) || (min_facex >= linfo->dims.x) ) {
      return;
  }
  if ( (min_facey < 0) || (min_facey >= linfo->dims.y) ) {
      return;
  }
  if ( (min_facez < 0) || (min_facez >= linfo->dims.z) ) {
      return;
  }

  //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
  float cell_minx, cell_miny, cell_minz;
  calc_cell_min( &cell_minx, &cell_miny, &cell_minz, min_facex, min_facey, min_facez, linfo->dims);

  //load current block/tree
  int blkIndex = calc_blkI(cell_minx, cell_miny, cell_minz, linfo->dims);
  local_tree[llid] = as_uchar16(tree_array[blkIndex]);
  ushort buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));

  //initialize cumsum buffer and cumIndex
  cumsum[llid*10] = local_tree[llid].s0;
  int cumIndex = 1;

  //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
  float texit = calc_cell_exit(cell_minx, cell_miny, cell_minz, 1.0f, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz);

  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  while (tblock < tfar )
  {
    //-------------------------------------------------------------------------
    //find entry point (adjusted) and the current block index
    float posx = calc_pos(tblock, ray_ox, ray_dx);
    float posy = calc_pos(tblock, ray_oy, ray_dy);
    float posz = calc_pos(tblock, ray_oz, ray_dz);

    if (tblock >= texit)
    {
      //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
      calc_cell_min( &cell_minx, &cell_miny, &cell_minz, posx, posy, posz, linfo->dims);

      //load current block/tree
      blkIndex = calc_blkI(cell_minx, cell_miny, cell_minz, linfo->dims);
      local_tree[llid] = as_uchar16(tree_array[blkIndex]);
      buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));

      //initialize cumsum buffer and cumIndex
      cumsum[llid*10] = local_tree[llid].s0;
      cumIndex = 1;

      //get scene level t exit value.  check to make sure that the ray is progressing.
      //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
      texit = calc_cell_exit(cell_minx, cell_miny, cell_minz, 1.0f, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz);
    }

    // traverse to leaf cell that contains the entry point, set bounding box
    float vox_len=1.0f;
    float vox_minx, vox_miny, vox_minz;
    int bit_index = traverse_three(&local_tree[llid],
                                   posx-cell_minx, posy-cell_miny, posz-cell_minz,
                                   &vox_minx, &vox_miny, &vox_minz, &vox_len);
    //data index is relative data (data_index_cached) plus data_index_root
    int data_ptr =    data_index_cached(&local_tree[llid], bit_index, bit_lookup, &cumsum[llid*10], &cumIndex)
                    + data_index_root(&local_tree[llid]);

    // get texit along the voxel
    float t_vox_exit = calc_cell_exit(vox_minx+cell_minx, vox_miny+cell_miny, vox_minz+cell_minz, vox_len,
                                      ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz);

    //make sure ray goes through the cell with positive seg length (or push it along)
    if (t_vox_exit <= tblock) break;

    //// distance must be multiplied by the dimension of the bounding box
    float d = (t_vox_exit - tblock); // * linfo->block_len;
    ////////////////////////////////////////////////////////////////////////////////
    // Step Cell Functor
    ////////////////////////////////////////////////////////////////////////////////

    STEP_CELL;
    ////////////////////////////////////////////////////////////////////////////////
    // END Step Cell Functor
    ////////////////////////////////////////////////////////////////////////////////
	tblock = t_vox_exit;
  }
}
#ifdef RENDER_VISIBILITY
void cast_ray_render_vis(
          //---- RAY ARGUMENTS -------------------------------------------------
          int i, int j,                                     //pixel information
          float ray_ox, float ray_oy, float ray_oz,         //ray origin
          float ray_dx, float ray_dy, float ray_dz,         //ray direction

          //---- SCENE ARGUMENTS------------------------------------------------
          __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
          __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16

          //---- UTILITY ARGUMENTS----------------------------------------------
          __local     uchar16            * local_tree,      //local tree for traversing
          __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
          __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                      float              * tfar_max,             //passed in as starting visibility

          //----aux arguments defined by host at compile time-------------------
          AuxArgs aux_args )
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->dims.x) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->dims.y) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->dims.z) : 0.0f;
  float tfar = calc_tfar(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, max_facex, max_facey, max_facez);
  float min_facex = (ray_dx < 0.0f) ? (linfo->dims.x) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->dims.y) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->dims.z) : 0.0f;
  float tblock = calc_tnear(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, min_facex, min_facey, min_facez);
  tblock = (tblock > 0.0f) ? tblock : 0.0f;    //make sure tnear is at least 0...
  tfar = tfar > (*tfar_max) ? (*tfar_max) : tfar;
  tfar -= BLOCK_EPSILON;   //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)

  if (tfar <= tblock)
    return;

  //calculate entry point here, entry sub_block, and its index
  min_facex = calc_pos(tblock, ray_ox, ray_dx); //(ray_ox + (tblock + TREE_EPSILON)*ray_dx);
  min_facey = calc_pos(tblock, ray_oy, ray_dy); //(ray_oy + (tblock + TREE_EPSILON)*ray_dy);
  min_facez = calc_pos(tblock, ray_oz, ray_dz); //(ray_oz + (tblock + TREE_EPSILON)*ray_dz);

  // verify that position is within bounds of block
  if ( (min_facex < 0) || (min_facex >= linfo->dims.x) ) {
      return;
  }
  if ( (min_facey < 0) || (min_facey >= linfo->dims.y) ) {
      return;
  }
  if ( (min_facez < 0) || (min_facez >= linfo->dims.z) ) {
      return;
  }

  //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
  float cell_minx, cell_miny, cell_minz;
  calc_cell_min( &cell_minx, &cell_miny, &cell_minz, min_facex, min_facey, min_facez, linfo->dims);

  //load current block/tree
  int blkIndex = calc_blkI(cell_minx, cell_miny, cell_minz, linfo->dims);
  local_tree[llid] = as_uchar16(tree_array[blkIndex]);
  ushort buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));

  //initialize cumsum buffer and cumIndex
  cumsum[llid*10] = local_tree[llid].s0;
  int cumIndex = 1;

  //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
  float texit = calc_cell_exit(cell_minx, cell_miny, cell_minz, 1.0f, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz);

  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  while (tblock < tfar  )
  {
    //-------------------------------------------------------------------------
    //find entry point (adjusted) and the current block index
    float posx = calc_pos(tblock, ray_ox, ray_dx);
    float posy = calc_pos(tblock, ray_oy, ray_dy);
    float posz = calc_pos(tblock, ray_oz, ray_dz);
    if (tblock >= texit)
    {
      //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
      calc_cell_min( &cell_minx, &cell_miny, &cell_minz, posx, posy, posz, linfo->dims);

      //load current block/tree
      blkIndex = calc_blkI(cell_minx, cell_miny, cell_minz, linfo->dims);
      local_tree[llid] = as_uchar16(tree_array[blkIndex]);
      buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));

      //initialize cumsum buffer and cumIndex
      cumsum[llid*10] = local_tree[llid].s0;
      cumIndex = 1;

      //get scene level t exit value.  check to make sure that the ray is progressing.
      //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
      texit = calc_cell_exit(cell_minx, cell_miny, cell_minz, 1.0f, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz);
    }

    // traverse to leaf cell that contains the entry point, set bounding box
    float vox_len=1.0f;
    float vox_minx, vox_miny, vox_minz;
    int bit_index = traverse_three(&local_tree[llid],
                                   posx-cell_minx, posy-cell_miny, posz-cell_minz,
                                   &vox_minx, &vox_miny, &vox_minz, &vox_len);
    //data index is relative data (data_index_cached) plus data_index_root
    int data_ptr =    data_index_cached(&local_tree[llid], bit_index, bit_lookup, &cumsum[llid*10], &cumIndex)
                    + data_index_root(&local_tree[llid]);

    // get texit along the voxel
    float t_vox_exit = calc_cell_exit(vox_minx+cell_minx, vox_miny+cell_miny, vox_minz+cell_minz, vox_len,
                                      ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz);

    //make sure ray goes through the cell with positive seg length (or push it along)
    if (t_vox_exit <= tblock) break;

    //// distance must be multiplied by the dimension of the bounding box
    float d = (t_vox_exit - tblock); // * linfo->block_len;
    tblock = t_vox_exit;

    ////////////////////////////////////////////////////////////////////////////////
    // Step Cell Functor
    ////////////////////////////////////////////////////////////////////////////////
    STEP_CELL;
    ////////////////////////////////////////////////////////////////////////////////
    // END Step Cell Functor
    ////////////////////////////////////////////////////////////////////////////////
  }
}

#endif

#ifdef RENDER_VISIBILITY2

// the following version has vis as input too, some step cell methods are updating this vis value so it is needed for the step cell
void cast_ray_render_vis2(
          //---- RAY ARGUMENTS -------------------------------------------------
          int i, int j,                                     //pixel information
          float ray_ox, float ray_oy, float ray_oz,         //ray origin
          float ray_dx, float ray_dy, float ray_dz,         //ray direction

          //---- SCENE ARGUMENTS------------------------------------------------
          __constant  RenderSceneInfo    * linfo,           //scene info (origin, block size, etc)
          __global    int4               * tree_array,      //tree buffers (loaded as int4, but read as uchar16

          //---- UTILITY ARGUMENTS----------------------------------------------
          __local     uchar16            * local_tree,      //local tree for traversing
          __constant  uchar              * bit_lookup,      //0-255 num bits lookup table
          __local     uchar              * cumsum,          //cumulative sum helper for data pointer
                      float              * tfar_max,         // max distance for the ray tracing to go
                      float              * vis,     //passed in as starting visibility

          //----aux arguments defined by host at compile time-------------------
          AuxArgs aux_args )
{
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

  //determine the minimum face:
  //get parameters tnear and tfar for the scene
  float max_facex = (ray_dx > 0.0f) ? (linfo->dims.x) : 0.0f;
  float max_facey = (ray_dy > 0.0f) ? (linfo->dims.y) : 0.0f;
  float max_facez = (ray_dz > 0.0f) ? (linfo->dims.z) : 0.0f;
  float tfar = calc_tfar(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, max_facex, max_facey, max_facez);
  float min_facex = (ray_dx < 0.0f) ? (linfo->dims.x) : 0.0f;
  float min_facey = (ray_dy < 0.0f) ? (linfo->dims.y) : 0.0f;
  float min_facez = (ray_dz < 0.0f) ? (linfo->dims.z) : 0.0f;
  float tblock = calc_tnear(ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz, min_facex, min_facey, min_facez);
  tblock = (tblock > 0.0f) ? tblock : 0.0f;    //make sure tnear is at least 0...
  tfar = tfar > (*tfar_max) ? (*tfar_max) : tfar;
  tfar -= BLOCK_EPSILON;   //make sure tfar is within the last block so texit surpasses it (and breaks from the outer loop)

  if (tfar <= tblock)
    return;

  //calculate entry point here, entry sub_block, and its index
  min_facex = calc_pos(tblock, ray_ox, ray_dx); //(ray_ox + (tblock + TREE_EPSILON)*ray_dx);
  min_facey = calc_pos(tblock, ray_oy, ray_dy); //(ray_oy + (tblock + TREE_EPSILON)*ray_dy);
  min_facez = calc_pos(tblock, ray_oz, ray_dz); //(ray_oz + (tblock + TREE_EPSILON)*ray_dz);

  // verify that position is within bounds of block
  if ( (min_facex < 0) || (min_facex >= linfo->dims.x) ) {
      return;
  }
  if ( (min_facey < 0) || (min_facey >= linfo->dims.y) ) {
      return;
  }
  if ( (min_facez < 0) || (min_facez >= linfo->dims.z) ) {
      return;
  }

  //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
  float cell_minx, cell_miny, cell_minz;
  calc_cell_min( &cell_minx, &cell_miny, &cell_minz, min_facex, min_facey, min_facez, linfo->dims);

  //load current block/tree
  int blkIndex = calc_blkI(cell_minx, cell_miny, cell_minz, linfo->dims);
  local_tree[llid] = as_uchar16(tree_array[blkIndex]);
  ushort buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));

  //initialize cumsum buffer and cumIndex
  cumsum[llid*10] = local_tree[llid].s0;
  int cumIndex = 1;

  //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
  float texit = calc_cell_exit(cell_minx, cell_miny, cell_minz, 1.0f, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz);

  //----------------------------------------------------------------------------
  // Begin traversing the blocks, break when any curr_block_index value is
  // illegal (not between 0 and scenedims)
  //----------------------------------------------------------------------------
  while (tblock < tfar  )
  {
    //-------------------------------------------------------------------------
    //find entry point (adjusted) and the current block index
    float posx = calc_pos(tblock, ray_ox, ray_dx);
    float posy = calc_pos(tblock, ray_oy, ray_dy);
    float posz = calc_pos(tblock, ray_oz, ray_dz);
    if (tblock >= texit)
    {
      //curr block index (var later used as cell_min), check to make sure block index isn't 192 or -1
      calc_cell_min( &cell_minx, &cell_miny, &cell_minz, posx, posy, posz, linfo->dims);

      //load current block/tree
      blkIndex = calc_blkI(cell_minx, cell_miny, cell_minz, linfo->dims);
      local_tree[llid] = as_uchar16(tree_array[blkIndex]);
      buff_index = as_ushort((uchar2) (local_tree[llid].sd, local_tree[llid].sc));

      //initialize cumsum buffer and cumIndex
      cumsum[llid*10] = local_tree[llid].s0;
      cumIndex = 1;

      //get scene level t exit value.  check to make sure that the ray is progressing.
      //When rays are close to axis aligned, t values found for intersection become ill-defined, causing an infinite block loop
      texit = calc_cell_exit(cell_minx, cell_miny, cell_minz, 1.0f, ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz);
    }

    // traverse to leaf cell that contains the entry point, set bounding box
    float vox_len=1.0f;
    float vox_minx, vox_miny, vox_minz;
    int bit_index = traverse_three(&local_tree[llid],
                                   posx-cell_minx, posy-cell_miny, posz-cell_minz,
                                   &vox_minx, &vox_miny, &vox_minz, &vox_len);
    //data index is relative data (data_index_cached) plus data_index_root
    int data_ptr =    data_index_cached(&local_tree[llid], bit_index, bit_lookup, &cumsum[llid*10], &cumIndex)
                    + data_index_root(&local_tree[llid]);

    // get texit along the voxel
    float t_vox_exit = calc_cell_exit(vox_minx+cell_minx, vox_miny+cell_miny, vox_minz+cell_minz, vox_len,
                                      ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz);

    //make sure ray goes through the cell with positive seg length (or push it along)
    if (t_vox_exit <= tblock) break;

    //// distance must be multiplied by the dimension of the bounding box
    float d = (t_vox_exit - tblock); // * linfo->block_len;
    tblock = t_vox_exit;

    ////////////////////////////////////////////////////////////////////////////////
    // Step Cell Functor
    ////////////////////////////////////////////////////////////////////////////////
    STEP_CELL;
    ////////////////////////////////////////////////////////////////////////////////
    // END Step Cell Functor
    ////////////////////////////////////////////////////////////////////////////////
  }
}

#endif
