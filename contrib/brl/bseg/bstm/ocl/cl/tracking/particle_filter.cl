#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable

#define VIEW_DIR_NUM 4
#define BLOCK_EPSILON .006125f
#define TIME_EPSILON .006125f
#define P_EPSILON .006125f

inline void AtomicAdd(volatile __global float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;
    do {
        prevVal.floatVal = *source;
        newVal.floatVal = prevVal.floatVal + operand;
    } while (atomic_cmpxchg((volatile __global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}

inline void AtomicMax(volatile __global float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;
    do {
        prevVal.floatVal = *source;
        newVal.floatVal = max(prevVal.floatVal, operand);
    } while (atomic_cmpxchg((volatile __global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}

inline void AtomicMin(volatile __global float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;
    do {
        prevVal.floatVal = *source;
        newVal.floatVal = min(prevVal.floatVal, operand);
    } while (atomic_cmpxchg((volatile __global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}

inline void AtomicAddLocal(volatile __local float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;
    do {
        prevVal.floatVal = *source;
        newVal.floatVal = prevVal.floatVal + operand;
    } while (atomic_cmpxchg((volatile __local unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}

bool intersect_two_boxes(box3d box1, box3d box2  )
{
  if(box1.x_min > box2.x_max || box1.x_max < box2.x_min) return false;
  if(box1.y_min > box2.y_max || box1.y_max < box2.y_min) return false;
  if(box1.z_min > box2.z_max || box1.z_max < box2.z_min) return false;
  return true;
}


bool contains(box3d box, point3d p)
{
  if(p.x >= box.x_min && p.x < box.x_max &&
     p.y >= box.y_min && p.y < box.y_max &&
     p.z >= box.z_min && p.z < box.z_max )
    return true;
  else
    return false;
}


__kernel void estimate_mi    (__global  RenderSceneInfo * linfo,
                              __global  float             * centerX,
                              __global  float             * centerY,
                              __global  float             * centerZ,
                              __constant  uchar           * bit_lookup,             //0-255 num bits lookup table
                              //template data
                              __global    int4            * tree_array,             // tree structure for each block
                              __global    int2            * time_tree_array,        // time tree structure for each block
                              __global    float           * alpha_array,            // alpha for each block
                              __global    MOG_TYPE        * app_array,              // appearance model for each block
                              //target data
                              __global    int4            * target_tree_array,      // tree structure for each block
                              __global    int2            * target_time_tree_array, // time tree structure for each block
                              __global    float           * target_alpha_array,     // alpha for each block
                              __global    MOG_TYPE        * target_app_array,       // appearance model for each block
                              //transformation param
                              __global    float           * translation,            //4 vector
                              __global    float           * rotation,               //9 vector
                              //bb
                              __global    float           * bounding_box,           //6 vector
                              //scene param
                              __global    float           * target_blk_origin,      //4 vector
                              __global    int             * target_subblk_num,      //4 vector
                              __global    float           * target_subblk_len,      //1 vector
                              __global    float           * times,                  //2 vector
                               //histograms
                              __global    uint            * bin_num,                //1 vector
                              __global    uint            * global_app_hist,        //8* nbin * nbin
                              __global    uint            * particle_no,            //1 vector

                              //output
                              __global    float           * output,
                              //local param
                              __local     uchar16         * local_trees,
                              __local     uchar16         * target_local_trees,
                              __local     uchar8          * local_time_trees,
                              __local     uint            * app_joint_hist
                              )
{
  int gid = get_global_id(0);
  int lid = get_local_id(0);

  int numTrees = linfo->dims.x * linfo->dims.y * linfo->dims.z;

  //let thread 0 initialize the joint histograms
  uint app_hist_nbins = bin_num[0];

  if (lid == 0)
  {
    for (unsigned int i = 0; i <  app_hist_nbins * app_hist_nbins * VIEW_DIR_NUM; i++)
      app_joint_hist[i] = 0;
  }

  barrier(CLK_LOCAL_MEM_FENCE);

  if (gid < numTrees)
  {
    //load bounding box
    box3d bb;
    bb.x_min = bounding_box[6* (*particle_no) + 0];  bb.y_min = bounding_box[6* (*particle_no) + 1]; bb.z_min = bounding_box[6* (*particle_no) + 2];
    bb.x_max = bounding_box[6* (*particle_no) + 3];  bb.y_max = bounding_box[6* (*particle_no) + 4]; bb.z_max = bounding_box[6* (*particle_no) + 5];
    point3d bb_center;
    bb_center.x = (bb.x_min + bb.x_max) / 2;
    bb_center.y = (bb.y_min + bb.y_max) / 2;
    bb_center.z = (bb.z_min + bb.z_max) / 2;

    //before loading the tree, figure out its x,y,z box
    int index_x = gid/(linfo->dims.y * linfo->dims.z);
    int rem_x= gid- index_x*(linfo->dims.y * linfo->dims.z);
    int index_y = rem_x/linfo->dims.z;
    int rem_y =  rem_x - index_y*linfo->dims.z;
    int index_z =rem_y;
    float tree_origin_x = linfo->origin.x + (float)index_x* linfo->block_len;
    float tree_origin_y = linfo->origin.y + (float)index_y* linfo->block_len;
    float tree_origin_z = linfo->origin.z + (float)index_z* linfo->block_len;

    box3d tree_box;
    tree_box.x_min = tree_origin_x; tree_box.x_max = tree_origin_x + linfo->block_len;
    tree_box.y_min = tree_origin_y; tree_box.y_max = tree_origin_y + linfo->block_len;
    tree_box.z_min = tree_origin_z; tree_box.z_max = tree_origin_z + linfo->block_len;

    //if the bb and tree don't intersect, nothing to do here...
    if(intersect_two_boxes(bb, tree_box) )
    {
      //load the tree in local mem
      local_trees[lid] = as_uchar16(tree_array[gid]);
      __local uchar16* local_tree = &local_trees[lid];

      // iterate through leaves
      for (int i=0; i<585; i++) {
        //if current bit is 0 and parent bit is 1, you're at a leaf
        int pi = (i-1)>>3;           //Bit_index of parent bit
        bool validParent = tree_bit_at(local_tree, pi) || (i==0); // special case for root
        if (validParent && tree_bit_at(local_tree, i)==0) {
          //////////////////////////////////////////////////
          //LEAF CODE HERE
          //////////////////////////////////////////////////
          //find transformed point
          float orig_x = linfo->origin.x + ((float)index_x+centerX[i])*linfo->block_len - bb_center.x;
          float orig_y = linfo->origin.y + ((float)index_y+centerY[i])*linfo->block_len - bb_center.y;
          float orig_z = linfo->origin.z + ((float)index_z+centerZ[i])*linfo->block_len - bb_center.z;
          float xformed_x = rotation[9* (*particle_no) + 0]*orig_x + rotation[9* (*particle_no) + 1]*orig_y + rotation[9* (*particle_no) + 2]*orig_z + translation[4* (*particle_no) + 0] + bb_center.x;
          float xformed_y = rotation[9* (*particle_no) + 3]*orig_x + rotation[9* (*particle_no) + 4]*orig_y + rotation[9* (*particle_no) + 5]*orig_z + translation[4* (*particle_no) + 1] + bb_center.y;
          float xformed_z = rotation[9* (*particle_no) + 6]*orig_x + rotation[9* (*particle_no) + 7]*orig_y + rotation[9* (*particle_no) + 8]*orig_z + translation[4* (*particle_no) + 2] + bb_center.z;

          //the transformed point might not be in the current block,
          //note a transformed block may be mapped to multiple blks
          box3d target_blk;
          target_blk.x_min = target_blk_origin[0] + BLOCK_EPSILON*target_subblk_len[0];
          target_blk.x_max = target_blk_origin[0] + target_subblk_num[0]* target_subblk_len[0] - BLOCK_EPSILON*target_subblk_len[0];
          target_blk.y_min = target_blk_origin[1] + BLOCK_EPSILON*target_subblk_len[0];
          target_blk.y_max = target_blk_origin[1] + target_subblk_num[1]* target_subblk_len[0] - BLOCK_EPSILON*target_subblk_len[0];
          target_blk.z_min = target_blk_origin[2] + BLOCK_EPSILON*target_subblk_len[0];
          target_blk.z_max = target_blk_origin[2] + target_subblk_num[2]* target_subblk_len[0] - BLOCK_EPSILON*target_subblk_len[0];

          point3d p; p.x = xformed_x; p.y = xformed_y; p.z = xformed_z;

          if(contains(target_blk,p ) )
          {
            //compute tree index given local time [0,linfo->dims.w)
            int time_tree_index1 = floor(times[0]);
            //get the data index
            float side_len = linfo->block_len/(float) (1<<get_depth(i));
            //get time tree ptr
            int dataIndex = data_index_relative(local_tree,i,bit_lookup) + data_index_root(local_tree);

            //load the time tree
            local_time_trees[lid] = as_uchar8( time_tree_array[dataIndex * linfo->dims.w + time_tree_index1 ]);
            //traverse it
            int bit_index_t = traverse_tt(&local_time_trees[lid], times[0] - time_tree_index1);
            int data_ptr_tt = data_index_root_tt(&local_time_trees[lid])+ get_relative_index_tt(&local_time_trees[lid],bit_index_t);
            //fetch data
            float alpha   = alpha_array[data_ptr_tt];
            float prob = 1 - exp(-alpha * side_len);
            MOG_TYPE app_model = app_array[data_ptr_tt];

            ////

            //now figure out the exact tree the transformed point lies in
            int target_tree_index_x = (int) floor( (xformed_x - target_blk_origin[0]) / target_subblk_len[0] );
            int target_tree_index_y = (int) floor( (xformed_y - target_blk_origin[1]) / target_subblk_len[0] );
            int target_tree_index_z = (int) floor( (xformed_z - target_blk_origin[2]) / target_subblk_len[0] );

            //load up the target tree in local mem
            int target_idx = target_tree_index_z + (target_tree_index_y + target_tree_index_x*target_subblk_num[1])*target_subblk_num[2];
            target_local_trees[lid] = as_uchar16(target_tree_array[target_idx]);
            __local uchar * target_tree_ptr = &target_local_trees[lid];

            //compute the local loc of pt in cube [0,1]^3
            float local_tree_x = (xformed_x - target_blk_origin[0]) / (target_subblk_len[0]) - target_tree_index_x;
            float local_tree_y = (xformed_y - target_blk_origin[1]) / (target_subblk_len[0]) - target_tree_index_y;
            float local_tree_z = (xformed_z - target_blk_origin[2]) / (target_subblk_len[0]) - target_tree_index_z;

            //now traverse to the transformed pt
            float cell_minx,cell_miny,cell_minz,cell_len;
            ushort target_bit_index =  traverse_three(target_tree_ptr,local_tree_x,local_tree_y,local_tree_z, &cell_minx,&cell_miny,&cell_minz, &cell_len);
            //figure out sidelen
            float target_side_len = (target_subblk_len[0])/(float) (1<<get_depth(target_bit_index));

            //get time tree ptr
            int time_tree_index2 = floor(times[1]);
            int target_dataIndex =data_index_relative(target_tree_ptr,target_bit_index,bit_lookup)+ data_index_root(target_tree_ptr);

            //load the time tree
            local_time_trees[lid] = as_uchar8( target_time_tree_array[target_dataIndex * target_subblk_num[3] + time_tree_index2 ]);

            //traverse tt
            int target_bit_index_t = traverse_tt(&local_time_trees[lid], times[1] - time_tree_index2);
            int target_data_ptr_tt = data_index_root_tt(&local_time_trees[lid])+ get_relative_index_tt(&local_time_trees[lid],target_bit_index_t);

            //fetch target data
            float target_alpha   = target_alpha_array[target_data_ptr_tt];
            float target_prob = 1 - exp(-target_alpha * target_side_len);
            MOG_TYPE target_app = target_app_array[target_data_ptr_tt];

/*
            //populate surf joint hist
            AtomicAddLocal( &(surf_joint_hist[0]), (1-prob) * (1-target_prob) );
            AtomicAddLocal( &(surf_joint_hist[1]), (1-prob) * (target_prob) );
            AtomicAddLocal( &(surf_joint_hist[2]), (prob) * (1-target_prob) );
            AtomicAddLocal( &(surf_joint_hist[3]), (prob) * (target_prob) );
*/

            //populate joint histograms...

            //now do the app hist.
            float bin_span = 2.0f / (float)app_hist_nbins;

            for(unsigned view_num = 0; view_num < VIEW_DIR_NUM; view_num++)
            {
              float app_mean, app_std;
              get_component(&app_model, 2*view_num, &app_mean,&app_std);
              float exp_app = prob * (1.0f+app_mean); // + (1-prob) * 0.5f;
              //float exp_app = app_mean;

              float target_app_mean;
              get_component(&target_app, 2*view_num, &target_app_mean,&app_std);
              float target_exp_app = target_prob * (1.0f + target_app_mean);

              int bin_id = clamp((int)floor(exp_app / bin_span), (int)0,(int)(app_hist_nbins-1));
              int target_bin_id = clamp((int)floor(target_exp_app / bin_span),(int)0,(int)(app_hist_nbins-1));

              atomic_inc( &(app_joint_hist[ (app_hist_nbins*app_hist_nbins*view_num) +   (bin_id*app_hist_nbins + target_bin_id) ]));
            }
          }
        }
      }
    }
  }


  //make sure every thread has finished populating hist.
  barrier(CLK_LOCAL_MEM_FENCE);


  if (lid == 0)
  {

    for(unsigned view_num = 0; view_num < VIEW_DIR_NUM; view_num++)
    {
      for (unsigned int i = 0; i < app_hist_nbins * app_hist_nbins; i++)
        atomic_add(& global_app_hist[ VIEW_DIR_NUM*app_hist_nbins*app_hist_nbins* (*particle_no) + (app_hist_nbins*app_hist_nbins*view_num) + i],
                                            app_joint_hist[(app_hist_nbins*app_hist_nbins*view_num) +  i]) ;
    }
  }
}
