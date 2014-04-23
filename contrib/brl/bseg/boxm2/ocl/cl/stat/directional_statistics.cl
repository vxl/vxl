#ifdef ACCUMULATE_DIRECTION_VECTORS
__kernel
void accumulate_direction_vectors(__global RenderSceneInfo  * info,
                                  __global float * aux0,
                                  __global float * aux1,
                                  __global float * aux2,
                                  __global float * aux3,
                                  __global float * aux0_img,
                                  __global float * aux2_img,
                                  __global float * aux_direction_expectation_x,
                                  __global float * aux_direction_expectation_y,
                                  __global float * aux_direction_expectation_z,
                                  __global float * aux_vis_expectation,
                                  __global float * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;
    if (gid<datasize)
    {
        float xdir = 0.0f;
        float ydir = 0.0f;
        float zdir = 0.0f;
        float vis  = 0.0f;
        if(aux0[gid] >  0.0f )
        {
            xdir = aux1[gid]/aux0[gid];
            ydir = aux2[gid]/aux0[gid];
            zdir = aux3[gid]/aux0[gid];
        }
        if(aux0_img[gid] > 0.0f)
        {
            vis = aux2_img[gid]/aux0_img[gid] ;
        }
        aux_direction_expectation_x[gid] = aux_direction_expectation_x[gid] + xdir*vis;
        aux_direction_expectation_y[gid] = aux_direction_expectation_y[gid] + ydir*vis;
        aux_direction_expectation_z[gid] = aux_direction_expectation_z[gid] + zdir*vis;

        aux_vis_expectation[gid] = aux_vis_expectation[gid] +vis;
    }
}
#endif

#ifdef COMPUTE_DISPERSION
__kernel
void compute_dispersion(__global RenderSceneInfo  * info,
                        __global float * alpha,
                        __global float * aux_direction_expectation_x,
                        __global float * aux_direction_expectation_y,
                        __global float * aux_direction_expectation_z,
                        __global float * aux_vis_expectation,
                        __global float  * aux_dispersion,
                        __global float * output)
{
    int gid=get_global_id(0);
    int datasize = info->data_len ;

    float block_len = info->block_len ;
    float alphainit = -log(1 - 0.01) / block_len;
    if (gid<datasize)
    {
        float vis  = aux_vis_expectation[gid] ;
        if( vis > 0.0f )
        {

            float Sx   = aux_direction_expectation_x[gid]/vis;
            float Sy   = aux_direction_expectation_y[gid]/vis;
            float Sz   = aux_direction_expectation_z[gid]/vis;
            aux_dispersion[gid] = sqrt(Sx*Sx+Sy*Sy+Sz*Sz);
            if(aux_dispersion[gid]> 0.8f )  // corresponds to 20 degrees
                alpha[gid] = alphainit;
        }
    }
}
#endif