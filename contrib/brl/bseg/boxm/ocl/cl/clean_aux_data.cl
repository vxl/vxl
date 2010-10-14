__kernel
void
clean_aux_data(__global RenderSceneInfo  * info,
               __global float2           * cum_len_beta,    // cumulative ray length and beta aux vars
               __global uchar2           * mean_obs_cum_vis) // mean_obs per cell and cumulative visibility
{
    int gid=get_global_id(0);
    int datasize = info->data_len * info->num_buffer;
    if (gid<datasize)
    {
      cum_len_beta[gid]   = (float2)0.0f;
      mean_obs_cum_vis[gid]=(uchar2)0;
    }
}