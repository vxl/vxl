__kernel
void
clean_aux_data(__global RenderSceneInfo  * info,
               __global    int                * seg_len_array,    //four aux data arrays
               __global    int                * mean_obs_array,   // -
               __global    int                * vis_array,        // -
               __global    int                * beta_array        // -
              // __global float2           * cum_len_beta,     // cumulative ray length and beta aux vars
              // __global uchar2           * mean_obs_cum_vis) // mean_obs per cell and cumulative visibility
              )
{
    int gid=get_global_id(0);
    int datasize = info->data_len * info->num_buffer;
    if (gid<datasize)
    {
      seg_len_array[gid]   = 0;
      mean_obs_array[gid]  = 0;
      vis_array[gid]   = 0;
      beta_array[gid]  = 0;
    }
}
