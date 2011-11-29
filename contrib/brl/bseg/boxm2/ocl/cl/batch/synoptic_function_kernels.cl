
#ifdef COMPUTE_CUBIC
__kernel 
void batch_fit_cubic_polynomial(__global float * aux0,
                                __global float * aux1,
                                __global float * aux2,
                                __constant int * nobs,
                                __constant float * interim_sigma,
                                __global int *datasize,
                                __global float * cubic_coeffs,
                                __local float * obs,
                                __local float * vis,
                                __local float * s,
                                __local float * temp,
                                __local float * XtWX,      // dim 16
                                __local float * cofactor,  // dim 16
                                __local float * invXtWX,   // dim 16
                                __local float * XtY,       // dim 4
                                __local float * outerprodl,// dim 16
                                __local float * l         // dim 4
                                )
{
    unsigned gid = get_group_id(0);
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    if(gid<(*datasize))
    {
        if(llid < (*nobs) )
        {
            //: OBTAIN obs, vis
            float seg_len = aux0[(*datasize)*llid+gid];
            obs[llid] = 0.0;
            vis[llid] = 0.0;
            if(seg_len > 1e-10f)
            {
                obs[llid] = aux1[(*datasize)*llid+gid]/seg_len;
                vis[llid] = aux2[(*datasize)*llid+gid]/seg_len;
            }
            //: OBTAIN  phi
            seg_len = aux0[(*datasize)*(llid + *nobs)+gid];
            s[llid] = seg_len;//0.0;
            if(seg_len > 1e-10f)
            {
                float x = aux1[(*datasize)*(llid + *nobs)+gid];
                float y = aux2[(*datasize)*(llid + *nobs)+gid];
                s[llid]  = atan2(y,x);
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        fit_intensity_cubic(obs,vis,s,temp, XtWX, cofactor, invXtWX,  XtY,outerprodl,l, cubic_coeffs, nobs);
        cubic_fit_error(obs, vis,s, temp, cubic_coeffs, interim_sigma, nobs);
        compute_empty(obs,vis,XtWX,cubic_coeffs,nobs);
    }
}
#endif