#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void generalized_volm_obj_based_matching_no_sky_with_orient(
              __global unsigned*                 n_cam, // query -- number of cameras (single unsigned)
              __global unsigned*                 n_obj, // query -- number of objects (single unsigned)
              __global unsigned*                grd_id, // query -- ground index id
              __global unsigned*            grd_offset, // query -- ground array offset indicator
              __global unsigned char*         grd_dist, // query -- ground query distance   (single float)
              __global unsigned char*         grd_land, // query -- ground query land fallback category (4 uchar per ray)
              __global float*             grd_land_wgt, // query -- ground query land fallback category weight (4 floats per ray)
              __global float*               grd_weight, // query -- ground weight parameter (single float)
              __global float*            grd_wgt_attri, // query -- ground weight parameters for different attributes (3 floats)
              __global unsigned*                obj_id, // query -- object index id
              __global unsigned*            obj_offset, // query -- object array offset indicator
              __global unsigned char*     obj_min_dist, // query -- object query minimium distance
              __global unsigned char*       obj_orient, // query -- object query orientation
              __global unsigned char*         obj_land, // query -- object query land fallback category (4 uchar per ray)
              __global float*             obj_land_wgt, // query -- object query land fallback category weight (4 float per ray)
              __global float*               obj_weight, // query -- object weight parameter array (n_obj floats)
              __global float*            obj_wgt_attri, // query -- object wieght parameter array (4*n_obj floats)
              __global unsigned*                 n_ind, // index -- number of indices passed into device (single unsigned)
              __global unsigned*            layer_size, // index -- size of spherical shell container (single unsigned)
              __global unsigned char*    fallback_size, // index -- number of the possible land type store in fallback land category
              __global unsigned char*            index, // index -- index depth array
              __global unsigned char*     index_orient, // index -- index orientation array
              __global unsigned char*       index_land, // index -- index land array
              __global float*                    score, // score array (score per index per camera)
              __global float*                       mu, // average depth array for index
              __global float*           depth_interval, // depth_interval
              __global unsigned*          depth_length, // length of depth_interval table
              __global float*                    debug, // debug array
              __local unsigned char*    local_min_dist, // query -- object minimimu distance on local memory
              __local unsigned char*  local_obj_orient, // query -- object orientation on local memory
              __local unsigned char*    local_obj_land, // query -- object land fallback category on local memory
              __local float*        local_obj_land_wgt, // query -- object land fallback category weight on local memory
              __local float*          local_obj_weight, // query -- object weight parameters on local memory
              __local float*       local_obj_wgt_attri, // query -- object weight parameters (for attributes) on local memory
              __local float*       local_grd_wgt_attri, // query -- object wieght parameters (for attributes) on local memory
              __local float*      local_depth_interval) // depth_interval on local memory
{
  // get the cam_id and ind_id
  unsigned cam_id = 0, ind_id = 0;
  ind_id = get_global_id(0);
  cam_id = get_global_id(1);
  unsigned llid = (get_local_id(0) + get_local_size(0)*get_local_id(1));

  //bool debug_bool = (ind_id == 143) && (cam_id == 3);

  // passing necessary values from global to the local memory
  __local unsigned ln_cam, ln_obj, ln_ind, ln_layer_size, ln_depth_size;
  __local unsigned char l_fs;
  __local float l_grd_weight;

  if (llid == 0) {
    ln_cam = *n_cam;
    ln_obj = *n_obj;
    ln_ind = *n_ind;
    ln_layer_size = *layer_size;
    l_fs = *fallback_size;
    l_grd_weight = *grd_weight;
    ln_depth_size = *depth_length;
  }
  if (llid < *n_obj) {
    local_min_dist[llid] = obj_min_dist[llid];
    local_obj_orient[llid] = obj_orient[llid];
    local_obj_weight[llid] = obj_weight[llid];
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  // doing this copy using only one work item because the size of depth_interval can be larger than than the work group size...
  if (llid == 0) {
    for (unsigned di = 0; di < ln_depth_size; di++)
      local_depth_interval[di] = depth_interval[di];
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  if (llid == 0) {
    for (unsigned di = 0; di < ln_obj*4; di++) {
      local_obj_wgt_attri[di] = obj_wgt_attri[di];
      local_obj_land[di] = obj_land[di];
      local_obj_land_wgt[di] = obj_land_wgt[di];
    }
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  if (llid < 3) {
    local_grd_wgt_attri[llid] = grd_wgt_attri[llid];
  }
  barrier(CLK_LOCAL_MEM_FENCE);

  // Start the matcher
  if ( cam_id < ln_cam && ind_id < ln_ind )
  {
    // locate index offset
    unsigned start_ind = ind_id * (ln_layer_size);

    // calculate ground score
    // define the altitude ratio, suppose the altitude in index could ba up to 3 meter
    // assuming the read-in alt values in query is normally ~1m, the altiutide ratio would be (2-1)/1 ~2
    // the altitude ratio defined the tolerance for ground distance d as delta_d = alt_ratio * d
    unsigned char alt_ratio = 2;
    unsigned start_grd = grd_offset[cam_id];
    unsigned end_grd = grd_offset[cam_id+1];
    unsigned score_grd_dst = 0;
    unsigned score_grd_ori = 0;
    float    score_grd_lnd = 0.0;
    for (unsigned k = start_grd; k < end_grd; ++k) {
      unsigned id = start_ind + grd_id[k];
      // ground distance score
      if (index[id] < ln_depth_size && grd_dist[k] < ln_depth_size ) {
        float ind_d = local_depth_interval[index[id]];
        float grd_d = local_depth_interval[grd_dist[k]];
        float delta_d = alt_ratio * grd_d;
        if ( ind_d >= (grd_d - delta_d) && ind_d <= (grd_d+delta_d) )
          score_grd_dst += 1;
      }
      // ground orientation and ground land type
      unsigned ind_ori = index_orient[id];
      unsigned ind_lnd = index_land[id];
      if (ind_ori == 1)
        score_grd_ori += 1;
      if (ind_lnd != 0) {
        unsigned lnd_start = k * l_fs;
        for (unsigned ii = 0; ii < l_fs; ii++) {
          unsigned l_id = lnd_start + ii;
          if (ind_lnd == grd_land[l_id]) {
            score_grd_lnd += grd_land_wgt[l_id];
            break;
          }
        }
      }
    }
    float score_grd;
    score_grd = local_grd_wgt_attri[0]*score_grd_ori +
                local_grd_wgt_attri[1]*score_grd_lnd +
                local_grd_wgt_attri[2]*score_grd_dst;

    score_grd = (end_grd != start_grd) ? score_grd / (end_grd-start_grd) : 0;
    score_grd = score_grd * l_grd_weight;

    // calculate object score
    // calculate average mean depth value first
    // locate the mu index to store the mean value
    unsigned mu_start_id = cam_id*ln_obj + ind_id*ln_cam*ln_obj;
    for (unsigned k = 0; k < ln_obj; ++k) {              // loop over each object for cam_id and ind_id
      unsigned offset_id = k + ln_obj * cam_id;
      unsigned start_obj = obj_offset[offset_id];
      unsigned end_obj = obj_offset[offset_id+1];
      float mu_obj = 0;
      unsigned count = 0;

      for (unsigned i = start_obj; i < end_obj; ++i) {   // loop over each voxel in object k
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
    for (unsigned k = 0; k < ln_obj; ++k)
    {
      unsigned offset_id = k + ln_obj * cam_id;
      unsigned start_obj = obj_offset[offset_id];
      unsigned end_obj = obj_offset[offset_id+1];
      unsigned lnd_start = k*l_fs;
      unsigned lnd_end = (k+1)*l_fs;
      float score_k_ord = 0.0f;
      float score_k_min = 0.0f;
      float score_k_ori = 0.0f;
      float score_k_lnd = 0.0f;
      for (unsigned i = start_obj; i < end_obj; ++i) {
        unsigned id = start_ind + obj_id[i];
        unsigned d = index[id];
        unsigned s_vox_ord = 0;
        unsigned s_vox_min = 0;
        unsigned s_vox_ori = 0;

        // calculate order and distance score
        if (d < 253 && d < ln_depth_size) {
          // calculate order score for voxel i
          s_vox_ord = 1;
          for (unsigned mu_id = 0; (s_vox_ord && mu_id < k); ++mu_id)
            if (mu[mu_id+mu_start_id]*mu[mu_id+mu_start_id] > 1E-7)
              s_vox_ord = s_vox_ord * (local_depth_interval[d] - mu[mu_id + mu_start_id] > -1E-5);
          for (unsigned mu_id = k+1; (s_vox_ord && mu_id < ln_obj); ++mu_id)
            if (mu[mu_id+mu_start_id]*mu[mu_id+mu_start_id] > 1E-7)
              s_vox_ord = s_vox_ord * (local_depth_interval[d] - mu[mu_id + mu_start_id] < 1E-5);
          // calculate min_distance score for voxel i
          s_vox_min = (d > local_min_dist[k]) ? 1 : 0;
        }

        // calculate score for orientation and land type
        unsigned char ind_ori = index_orient[id];
        unsigned char ind_lnd = index_land[id];
        if (local_obj_orient[k] == 1)  // query obj is horizontal
          s_vox_ori = (ind_ori == 1) ? 1 : 0;
        else                           // query obj is vertical
          s_vox_ori = (ind_ori > 1 && ind_ori < 10 && local_obj_orient[k] != 0) ? 1: 0;
        //if (ind_ori < 253)
        //  s_vox_ori = (ind_ori != 0 && ind_ori == local_obj_orient[k]) ? 1 : 0;
        if (ind_lnd != 0) {
          for (unsigned ii = lnd_start; ii < lnd_end; ii++) {
            if (ind_lnd == local_obj_land[ii]) {
              score_k_lnd += local_obj_land_wgt[ii];
              break;
            }
          }
        }
        score_k_ord += (float)s_vox_ord;
        score_k_min += (float)s_vox_min;
        score_k_ori += (float)s_vox_ori;
      }  // finish object k

      // normalized the score for object k
      unsigned att_s = k*4;
      float score_k = local_obj_wgt_attri[att_s] * score_k_ori +
                      local_obj_wgt_attri[att_s+1] * score_k_lnd +
                      local_obj_wgt_attri[att_s+2] * score_k_min +
                      local_obj_wgt_attri[att_s+3] * score_k_ord;

      score_k = (end_obj != start_obj) ? score_k/(end_obj-start_obj) : 0;
      score_k *= local_obj_weight[k];
      // summarize the object score
      score_obj += score_k;
    }

#if 0
    if ( cam_id == 0 && ind_id == 90 ) {
      debug[0] = cam_id;
      debug[1] = ind_id;
      debug[2] = score_grd;
      debug[3] = score_obj;
    }
#endif
    // summarize the scores
    unsigned score_id = cam_id + ind_id*ln_cam;
    score[score_id] = score_grd + score_obj;
  }  // end of the calculation of index ind_id and camera cam_id
}
