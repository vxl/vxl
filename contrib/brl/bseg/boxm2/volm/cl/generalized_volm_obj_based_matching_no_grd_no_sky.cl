#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void generalized_volm_obj_based_matching_no_grd_no_sky(__global unsigned*                 n_cam,         // query -- number of cameras (single unsigned)
                                                                __global unsigned*                 n_obj,         // query -- number of objects (single unsigned)
                                                                __global unsigned*                obj_id,         // query -- object index id
                                                                __global unsigned*            obj_offset,         // query -- object array offset indicator
                                                                __global unsigned char*     obj_min_dist,         // query -- object query minimium distance
                                                                __global float*               obj_weight,         // query -- object weight parameter array
                                                                __global unsigned*                 n_ind,         // index -- number of indices passed into device (single unsigned)
                                                                __global unsigned*            layer_size,         // index -- size of spherical shell container (single unsigned)
                                                                __global unsigned char*            index,         // index -- index depth array
                                                                __global float*                    score,         // score array (score per index per camera)
                                                                __global float*                       mu,         // average depth array for index
                                                                __global float*           depth_interval,         // depth_interval
                                                                __global unsigned*          depth_length,         // length of depth_interval table
                                                                __global float*                    debug,         // debug array
                                                                __local unsigned char*    local_min_dist,         // query -- object minimimu distance on local memory
                                                                __local float*          local_obj_weight,         // query -- object order list on local memory
                                                                __local float*      local_depth_interval)         // depth_interval on local memory
{
  // get the cam_id and ind_id
  unsigned cam_id = 0, ind_id = 0;
  ind_id = get_global_id(0);
  cam_id = get_global_id(1);
  unsigned llid = (get_local_id(0) + get_local_size(0)*get_local_id(1));
  
  //bool debug_bool = (ind_id == 143) && (cam_id == 3);
  
  // passing necessary values from global to the local memory
  __local unsigned ln_cam, ln_obj, ln_ind, ln_layer_size, ln_depth_size;
  if (llid == 0) {
    ln_cam = *n_cam;
    ln_obj = *n_obj;
    ln_ind = *n_ind;
    ln_layer_size = *layer_size;
    ln_depth_size = *depth_length;
  }
  if (llid < *n_obj) {
    local_min_dist[llid] = obj_min_dist[llid];
    local_obj_weight[llid] = obj_weight[llid];
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  if (llid == 0) {
    for(unsigned i = 0; i < (*depth_length); i++) {
      local_depth_interval[i] = depth_interval[i];
    }
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  //if( cam_id == 0 && ind_id == 1) {
  //  debug[0] = *depth_length;
  //  for(unsigned i =0 ; i < (*depth_length); i++) {
  //    debug[i+1] = depth_interval[i];
  //  }
  //}
//  if( debug_bool ){
//    debug[0] = (float)get_global_id(0);
//    debug[1] = (float)get_global_id(1);
//    debug[2] = (float)get_local_size(0);
//    debug[3] = (float)get_local_size(1);
//    debug[4] = (float)llid;
//    debug[5] = (float)ln_ind;
//    debug[6] = (float)ln_cam;
//    debug[7] = (float)ln_obj;
//    debug[8] = (float)ind_id;
//    debug[9] = (float)cam_id;
//  }


  // locate the index
  
  if ( cam_id < ln_cam && ind_id < ln_ind ) {
    // locate index offset
    unsigned start_ind = ind_id * (ln_layer_size);
        
    // calculate object score
    // calcualte average mean depth value first
    // locate the mu index to store the mean value
    unsigned mu_start_id = cam_id*ln_obj + ind_id*ln_cam*ln_obj;
    for (unsigned k = 0; k < ln_obj; k++) {              // loop over each object for cam_id and ind_id
      unsigned offset_id = k + ln_obj * cam_id;
      unsigned start_obj = obj_offset[offset_id];
      unsigned end_obj = obj_offset[offset_id+1];
         float mu_obj = 0;
      unsigned count = 0;
      
      for (unsigned i = start_obj; i < end_obj; i++) {   // loop over each voxel in object k
        unsigned id = start_ind + obj_id[i];
        unsigned d = index[id];
        if (d < 253 && d < ln_depth_size) {
          mu_obj += local_depth_interval[d];
          count += 1;
        }
      }
      mu_obj = (count > 0) ? mu_obj/count : 0;
      unsigned mu_id = k + mu_start_id;
      mu[mu_id] =  mu_obj;
    }
    // calculate object score
    // note that the two neighboring objects may have same order 
    // therefore voxel depth could be less or eqaul the meaning depth of the objects with lower order
    // and could be greater or eqaul the meaning depth of the object with higher order
    float score_obj = 0.0f;
    for (unsigned k = 0; k < ln_obj; k++) {
      unsigned offset_id = k + ln_obj * cam_id;
      unsigned start_obj = obj_offset[offset_id];
      unsigned end_obj = obj_offset[offset_id+1];
      float score_k_ord = 0.0f;
      float score_k_min = 0.0f;
      for (unsigned i = start_obj; i < end_obj; i++) {
        unsigned id = start_ind + obj_id[i];
        unsigned d = index[id];
        unsigned s_vox_ord = 1;
        unsigned s_vox_min = 0;
        if (d < 253 && d < ln_depth_size) {
          // calculate order score for voxel i
          for (unsigned mu_id = 0; (s_vox_ord && mu_id < k); mu_id++)
            if(mu[mu_id+mu_start_id] != 0)
              s_vox_ord = s_vox_ord * (local_depth_interval[d] >= mu[mu_id + mu_start_id]);
          for (unsigned mu_id = k+1; (s_vox_ord && mu_id < ln_obj); mu_id++)
            if(mu[mu_id+mu_start_id] != 0)
              s_vox_ord = s_vox_ord * (local_depth_interval[d] <= mu[mu_id + mu_start_id]);
          // calculate min_distance socre for voxel i
          if( d > local_min_dist[k] )
            s_vox_min = 1;
        }
        else {
          s_vox_ord = 0;
        }
        score_k_ord += (float)s_vox_ord;
        score_k_min += (float)s_vox_min;
      }
      // normalized the score for object k 
      float score_k = score_k_ord + score_k_min;
      score_k = (end_obj != start_obj) ? score_k/(end_obj-start_obj) : 0;
      score_k = score_k * local_obj_weight[k];
      // summerize the object score
      score_obj += score_k;
    }
    // normalized the order score
    score_obj = score_obj / ln_obj;
    
    // summerize the scores
    unsigned score_id = cam_id + ind_id*ln_cam;
    score[score_id] = score_obj;
/*    
    if (debug_bool) {
      debug[10] = (float)score_sky;
      debug[11] = (float)score_grd;
      debug[12] = (float)score_obj;
      debug[13] = (float)score[score_id];
      debug[14] = score_id;
    }
*/
  }  // end of the calculation of index ind_id and camera cam_id

}