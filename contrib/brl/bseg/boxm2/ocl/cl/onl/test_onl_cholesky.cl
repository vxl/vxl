__kernel
void test_onl_cholesky(__global float *mat4x4,
                       __global float *Lmat4x4,
                       __global int * n,
                       __local  float *localLmat4x4
                      )
{
    int lid = get_local_id(0);
    int gid = get_global_id(0);
    int grpid = get_group_id(0);

    int nsqr = (*n)*(*n);
    if ( lid < nsqr )
    {
        localLmat4x4[lid ] = mat4x4[grpid*nsqr + lid];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    cholesky_decomposition(localLmat4x4,*n);
    if ( lid < nsqr )
    {
        Lmat4x4[grpid*nsqr + lid] = localLmat4x4[lid];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}

__kernel
void test_onl_cholesky_solve(__global float *mat4x4,
                             __global float *b4x1,
                             __global float *x4x1,
                             __global int * n,
                             __local  float *localLmat4x4,
                             __local  float *localb4x1,
                             __local  float *localx4x1)
{
    int lid = get_local_id(0);
    int gid = get_global_id(0);
    int grpid = get_group_id(0);

    int nsqr = (*n)*(*n);
    if ( lid < nsqr )
        localLmat4x4[lid ] = mat4x4[grpid*nsqr + lid];
    barrier(CLK_LOCAL_MEM_FENCE);


    if ( lid < (*n) )
    {
        localb4x1[lid] = b4x1[grpid*(*n) + lid];
        localx4x1[lid] =0.0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    cholesky_solve(localLmat4x4,*n,localb4x1,localx4x1);
    if ( lid < (*n) )
        x4x1[grpid*(*n) + lid] = localx4x1[lid];
    barrier(CLK_LOCAL_MEM_FENCE);
}
