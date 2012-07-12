__kernel
void test_matrix4x4_inverse(__global float *mat4x4,
                            __global float *invmat4x4,
                            __local  float *lmat4x4,
                            __local  float *lcofactor4x4,
                            __local  float *linvmat4x4)
{
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    lmat4x4[llid] = mat4x4[llid];
    barrier(CLK_LOCAL_MEM_FENCE);

    onl_inverse_4x4(lmat4x4,lcofactor4x4, linvmat4x4);
    invmat4x4[llid] = linvmat4x4[llid];
}

__kernel
void test_cubic_fit(__global float * gobs,       // dim n
                    __global float * gvis,       // dim n
                    __global float * gs,       // dim 4
                    __global float * coeffs,       // dim 4
                    __global float * vars,       // dim 4
                    __constant int * nobs,         // dim n
                    __local float * obs,       // dim n
                    __local float * vis,       // dim n
                    __local float * s,         // dim n
                    __local float * temp,
                    __local float * XtWX,      // dim 16
                    __local float * cofactor,  // dim 16
                    __local float * invXtWX,   // dim 16
                    __local float * XtY)
{
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
    obs[llid] = gobs[llid];
    vis[llid] = gvis[llid];
    s[llid] = gs[llid];
    barrier(CLK_LOCAL_MEM_FENCE);

    fit_intensity_cubic(obs,vis,s,temp,XtWX,cofactor,invXtWX,XtY,coeffs,nobs);
    cubic_fit_error(obs, vis,s, temp,coeffs,vars,nobs);
}

