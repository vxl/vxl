__kernel void map_orbit_to_target(                   __constant  float           * centerX,//0
                                                     __constant  float           * centerY,//1
                                                     __constant  float           * centerZ,//2
                                                     __constant  uchar           * bit_lookup,//3             //0-255 num bits lookup table
                                                     __global  RenderSceneInfo   * target_scene_linfo,//4
                                                     __global  RenderSceneInfo   * source_scene_linfo,//5
                                                     __global    int4            * target_scene_tree_array,//6       // tree structure for each block
                                                     __global    int4            * source_scene_tree_array,//9       // tree structure for each block
                                                     __global    float           * target_scene_alpha_array,//7      // alpha for each block
                                                     __global    MOG_TYPE        * target_scene_mog_array,//8        // appearance for each block
                                                     __global    uchar8          * target_rgb_array,
                                                     __global    float           * target_vis_array,


                                                     __global    float           * source_scene_alpha_array,//10      // alpha for each block
                                                     __global    MOG_TYPE        * source_scene_mog_array,//11        // appearance for each block
                                                     __global    uchar8          * source_rgb_array,
                                                     __global    float           * source_vis_array,
                                                     //                                          __global    ushort4         * source_nobs_array,
                                                     __global    float           * rotation,//13
                                                     __global    float           * translation,//12
                                                     __global    uchar           * sphere,
                                                     __global    uchar           * iris,
                                                     __global    uchar           * pupil,
                                                     __global    uchar           * eyelid,
                                                     __global    uchar           * eyelid_crease,
                                                     __global    uchar           * lower_lid,
                                                     __global    float           * eyelid_geo,//16
                                                     __global    float           * dt,//16
                                                     //at which voxels should be matched.
#ifdef DEBUG_CL
                                                     __global    float           * output,//16
                                                     __global    float16         * nb_top,//16
                                                     __global    float16         * nb_bot,//16
                                                     __global    float8          * output_mog,//16
                                                     __global    float4          * output_color,//16
#endif
                                                     __local     uchar           * cumsum_wkgp,//17
                                                     __local     uchar16         * local_trees_target,//18
                                                     __local     uchar16         * local_trees_source,
                                                     __local     uchar16         * reflected_local_trees_source,//18
                                                     __local     uchar16         * neighbor_trees)//20
{
  int gid = get_global_id(0);
  int lid = get_local_id(0);
  //default values for empty cells
  MOG_INIT(mog_init);

  //-log(1.0f - init_prob)/side_length  init_prob = 0.001f
  float alpha_init = 0.001f/source_scene_linfo->block_len;
  int is_right = 0;
#ifdef RIGHT_EYE
  is_right = 1;
#endif
  //
  int numTrees = target_scene_linfo->dims.x * target_scene_linfo->dims.y * target_scene_linfo->dims.z;
  float4 source_scene_origin = source_scene_linfo->origin;
  float4 source_scene_blk_dims = convert_float4(source_scene_linfo->dims) ;
  float4 source_scene_maxpoint = source_scene_origin + convert_float4(source_scene_linfo->dims) * source_scene_linfo->block_len ;
  //: each thread will work on a sub_block(tree) of target to match it to locations in the source block
  if (gid < numTrees) {
    local_trees_target[lid] = as_uchar16(target_scene_tree_array[gid]);
    int index_x =(int)( (float)gid/(target_scene_linfo->dims.y * target_scene_linfo->dims.z));
    int rem_x   =( gid- (float)index_x*(target_scene_linfo->dims.y * target_scene_linfo->dims.z));
    int index_y = rem_x/target_scene_linfo->dims.z;
    int rem_y =  rem_x - index_y*target_scene_linfo->dims.z;
    int index_z =rem_y;
    if((index_x >= 0 &&  index_x <= target_scene_linfo->dims.x -1 &&
        index_y >= 0 &&  index_y <= target_scene_linfo->dims.y -1 &&
        index_z >= 0 &&  index_z <= target_scene_linfo->dims.z -1  )) {
      __local uchar16* local_tree = &local_trees_target[lid];
      __local uchar16* neighbor_tree = &neighbor_trees[lid];
      __local uchar * cumsum = &cumsum_wkgp[lid*10];
      __local Eyelid eyelid_param;
      eyelid_from_float_arr(&eyelid_param,eyelid_geo);
      __local OrbitLabels orbit;
      orbit.sphere = sphere; orbit.iris = iris; orbit.pupil = pupil;
      orbit.lower_lid = lower_lid; orbit.eyelid = eyelid; orbit.eyelid_crease = eyelid_crease;


      // iterate through leaves
      cumsum[0] = (*local_tree).s0;
      int cumIndex = 1;
      int MAX_INNER_CELLS, MAX_CELLS;
      get_max_inner_outer(&MAX_INNER_CELLS, &MAX_CELLS, target_scene_linfo->root_level);
      MAX_CELLS = 585;
      for (int i=0; i<MAX_CELLS; i++) {
        //if current bit is 0 and parent bit is 1, you're at a leaf
        int pi = (i-1)>>3;           //Bit_index of parent bit
        bool validParent = tree_bit_at(local_tree, pi) || (i==0); // special case for root
        int currDepth = get_depth(i);
        if (validParent && ( tree_bit_at(local_tree, i)==0 )) {

          //: for each leaf node xform the cell and find the correspondence in another block.
          //get the index into this cell data
          int target_data_index = data_index_cached(local_tree, i, bit_lookup, cumsum, &cumIndex) + data_index_root(local_tree); //gets absolute position
          // set intial values in case source is not accessed
          /* target_scene_mog_array[target_data_index] = mog_init; */
          /* target_scene_alpha_array[target_data_index] = alpha_init; */

          // transform coordinates to source scene
          float xg = target_scene_linfo->origin.x + ((float)index_x+centerX[i])*target_scene_linfo->block_len ;
          float yg = target_scene_linfo->origin.y + ((float)index_y+centerY[i])*target_scene_linfo->block_len ;
          float zg = target_scene_linfo->origin.z + ((float)index_z+centerZ[i])*target_scene_linfo->block_len ;

          float txg = xg - translation[0];
          float tyg = yg - translation[1];
          float tzg = zg - translation[2];
          float4 source_p           = (float4)( txg,tyg,tzg,0);
          float4 source_p_refl      = (float4)(-txg,tyg,tzg,0);
          float4 cell_center, cell_center_refl;
          unsigned source_data_index, bit_index, source_data_index_refl, bit_index_refl;
          unsigned point_ok = data_index_world_point(source_scene_linfo, source_scene_tree_array,
                                                     local_trees_source,lid,source_p,
                                                     &cell_center,
                                                     &source_data_index,
                                                     &bit_index,bit_lookup);
         unsigned r_point_ok = data_index_world_point(source_scene_linfo, source_scene_tree_array,
                                                     reflected_local_trees_source,lid,source_p_refl,
                                                     &cell_center_refl,
                                                     &source_data_index_refl,
                                                     &bit_index_refl,bit_lookup);

          if(point_ok && r_point_ok)
          {

            if(bit_index >=0 && bit_index < MAX_CELLS ){
              cell_center.s3=0;
              {
                //reflect the non sphere voxels ; make sure there is anatomy at the reflected data index
                bool lidvoxel = false; bool has_anatomy = false;
                float4 source_p_original  = source_p;
                float4 left_p             = is_right ? source_p_refl : source_p;
                int bit_index_original = bit_index ;
                int source_data_index_original = source_data_index;
                int source_data_index_probe = is_right ? source_data_index_refl : source_data_index;
                float4 cell_center_original = cell_center;
                int anatomy_index = 6;


              if( sphere[source_data_index_original] ){
                  /* //we are on a sphere voxel so time to undo the rotation */
                  source_p = rotate_point(source_p_original,rotation);
                  has_anatomy = true;
                  unsigned point_ok = data_index_world_point(source_scene_linfo, source_scene_tree_array,
                                                     local_trees_source,lid,source_p,
                                                     &cell_center,
                                                     &source_data_index,
                                                     &bit_index,bit_lookup);

              }
                if( eyelid[source_data_index_probe]){
                  source_p          = is_right ? source_p_refl          : source_p_original ;
                  source_data_index = is_right ? source_data_index_refl : source_data_index_original;
                  bit_index         = is_right ? bit_index_refl         : bit_index_original;
                  cell_center       = is_right ? cell_center_refl       : cell_center_original;
                  float t           = compute_t(source_p.x,source_p.y,&eyelid_param);
                  float tolerance = 0.0;
                  has_anatomy = is_valid_t(t,&eyelid_param,tolerance) ? true : false;
                  if(!is_valid_t( t - dt[0], &eyelid_param,tolerance)){
                    lidvoxel = sphere[source_data_index] ? false : true;
                  }else{
                    source_p += lid_vf(source_p.x, t, -dt[0],&eyelid_param);
                    //source_p = is_right? source_p_left_refl : source_p_left;

                    unsigned point_ok     = data_index_world_point(source_scene_linfo, source_scene_tree_array,
                                                                   local_trees_source,lid,source_p,
                                                                   &cell_center,
                                                                   &source_data_index,
                                                                   &bit_index,bit_lookup);
                    if(!point_ok)
                      has_anatomy = false;
                  }
              }

              if( eyelid_crease[source_data_index_probe] || lower_lid[source_data_index_probe] ){
                  has_anatomy = true;
                  source_p          = is_right ? source_p_refl          : source_p_original ;
                  source_data_index = is_right ? source_data_index_refl : source_data_index_original;
                  bit_index         = is_right ? bit_index_refl         : bit_index_original;
                  cell_center       = is_right ? cell_center_refl       : cell_center_original;

                }
                if (!has_anatomy)
                  continue;

                int current_anatomy = get_ranked_anatomy(source_data_index,&orbit);

                if (current_anatomy == 6)
                  continue;
                int currDepth = get_depth(bit_index);
                float cell_len = 1.0f/((float)(1<<currDepth));
                MOG_TYPE mog_original = source_scene_mog_array  [source_data_index];
                float  alpha_original = source_scene_alpha_array[source_data_index];
                uchar8 rgb_original   = source_rgb_array[source_data_index];
                target_scene_alpha_array[target_data_index] = lidvoxel ? 0 : alpha_original;
                target_scene_mog_array[target_data_index]   = mog_original;
                target_rgb_array[target_data_index] = rgb_original;
                target_vis_array[target_data_index] = 1.0; // bump up vis score

                // here is where the interp magic happens
#define DO_INTERP
#ifdef DO_INTERP
                float  alphas[8];
                float  params[8];
                float weights[8];
                float4 abs_neighbors[8];
                float4 rgb_params[8];

                float cell_len_rw  = cell_len * source_scene_linfo->block_len;
                int nbs_ok = collect_neighbors_and_weights(abs_neighbors,weights,source_p,cell_center, cell_len_rw);

                if(!nbs_ok)
                  continue;
#ifdef DEBUG_CL
                float16 group_b = (float16)(abs_neighbors[0],abs_neighbors[1],abs_neighbors[2],abs_neighbors[3]);
                float16 group_t = (float16)(abs_neighbors[4],abs_neighbors[5],abs_neighbors[6],abs_neighbors[7]);
                nb_bot[source_data_index] = group_b;
                nb_top[source_data_index] = group_t;
                nb_bot[source_data_index].s3 = source_p.x; nb_bot[source_data_index].s7 =source_p.y ; nb_bot[source_data_index].sB =source_p.z;
#endif
                int nb_count = 0; float sum = 0;
                float4 blank_val = (float4)(255,128,255,0);
                //                output[source_data_index] = nb_bot[source_data_index].s0;
                for (unsigned i=0; i<8; i++){
                  int neighborBitIndex, nb_data_index;
                  float4 nb_cell_center;
                  unsigned nb_point_ok = data_index_world_point(source_scene_linfo, source_scene_tree_array,
                                                                neighbor_trees,lid,abs_neighbors[i],
                                                                &nb_cell_center,
                                                                &nb_data_index,
                                                                &neighborBitIndex,bit_lookup);

                  bool nb_anatomy = nb_point_ok ? is_anatomy(current_anatomy,nb_data_index,&orbit) : false;
                  if ( nb_anatomy ){
                    alphas[i] = source_scene_alpha_array[nb_data_index] ;
                    nb_count++;
#ifdef MOG_TYPE_8
                    CONVERT_FUNC_FLOAT8(mog,source_scene_mog_array[nb_data_index]);
                    mog /= NORM ;
                    EXPECTED_INT(params[i],mog);
#endif

                    CONVERT_FUNC_FLOAT8(rgb_tuple,source_rgb_array[nb_data_index]);
                    rgb_tuple /= NORM;
                    rgb_params[i].s0123 = rgb_tuple.s0123;
                  }else{
                    alphas [i] = 0.0; params[i]=0; rgb_params[i] = (float4)(255,255,0,0);
                    weights[i] = 0.0;
                  }
                }

                if (nb_count< 8 && nb_count > 0){ //renormalize if we had a nan value
                  float sum_w = 0;
                  for(unsigned i=0 ; i<8 ; sum_w += weights[i++] );
                  if( sum_w == 0)
                    continue;
                  for(unsigned i = 0; i<8 ; weights[i++] /= sum_w );
                } else if ( nb_count == 0 )
                   continue;

                CONVERT_FUNC_FLOAT8(mog_float,mog_original);
                mog_float/=NORM; float expected_int_orig;
                EXPECTED_INT(expected_int_orig,mog_float); //for debug
                MOG_TYPE mog_interped;
                float alpha_interped = interp_float_weights(alphas,weights);

                uchar8 curr_rgb_tuple = source_rgb_array[source_data_index];
                float4 float_rgb_tuple_interped  = interp_float4_weights(rgb_params,weights); //use the flow interp for float4s

                uchar4 uchar_rgb_tuple_interped  = convert_uchar4_sat_rte(float_rgb_tuple_interped * NORM); // hack-city

#ifdef MOG_TYPE_8

                float expected_int   = interp_float_weights(params,weights);
#ifdef DEBUG_CL
                output[source_data_index] = alpha_interped;
                __global    float* mog_nb = (__global float*) &output_mog[source_data_index];
                for(unsigned i =0;i<8; i++){
                  mog_nb[i] = alphas[i];
                }

                output_color[source_data_index] = float_rgb_tuple_interped  * 255 ;
#endif
                uchar8 expected_int_interped  = (uchar8)((uchar)(expected_int * NORM), 32, 255, 0, 0, 0, 0, 0); // hack-city
                CONVERT_FUNC_SAT_RTE(mog_interped, expected_int_interped);
#endif

                curr_rgb_tuple.s0123 = uchar_rgb_tuple_interped;
                target_rgb_array[target_data_index] = curr_rgb_tuple;
                target_scene_alpha_array[target_data_index] = lidvoxel  ? 0 : alpha_interped;
                target_scene_mog_array[target_data_index]   = mog_interped;


#endif //interp

              }
            }
          }
        }
      }
    }
  }
}
