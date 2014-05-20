#ifdef QUADRATIC
typedef struct
{
    float A;
} AuxArgs;

float levenberg_marquadt_solve(__constant int * max_iter,    // x : nx1
                               int  m,                       // y : mx1
                               int  n,
                               __global float * output,
                               __local float * x,
                               __local float * h,            // J : mxn
                               __local float * y,            // A : nxn
                               __local float * g,
                               __local float * J,
                               __local float * A,
                               __local float * tempm,
                               __local float * tempn,
                               AuxArgs  args);
__kernel
void test_levenberg_marquardt(__constant int * max_iter, // x : nx1
                               __constant int * n,        // y : mx1
                               __constant int * m,        // J : mxn
                               __global float * x,        // A: nxn
                               __global float * y,
                               __global float * output,
                               __local float * lx,
                               __local float * lh,
                               __local float * ly,
                               __local float * lg,
                               __local float * lJ,
                               __local float * lA,
                               __local float * tempm,
                               __local float * tempn)
{
    int lid = get_local_id(0);
    int gid = get_global_id(0);
    int grpid = get_group_id(0);
    int lsize = get_local_size(0);

    for (unsigned i = lid ; i < (*m); i+=lsize)
        ly[i] =y[i];

    for (unsigned i = lid ; i < (*n); i+=lsize)
    {
        lx[i] = x[i];
        lg[i] = 0.0;
    }
    AuxArgs aux_args;

    int nl = (*n);
    //levenberg_marquadt_solve(max_iter,m,n,output,lx,lh,ly,lg,lJ,lA,tempm,tempn,aux_args);
    for (unsigned i = lid ; i < (*n); i+=lsize)
        output[i] = lx[i];

    barrier(CLK_GLOBAL_MEM_FENCE);
}
#endif

#ifdef PHONGS
__kernel
void test_ocl_phongs_model_est(__constant int * max_iter,
                               __constant int * m,       //
                               __global float * x,       //
                               __global float * output,
                               __global float * Iobs,      //(*nobs) * 1
                               __global float * weights,
                               __global float * Viewingobs, // (*nobs) * 3
                               __global float * sunangles,
                               __local float * ly,
                               __local float * lJ,
                               __local float * lA,
                               __local float * tempm,
                               __local float * lIobs,
                               __local float * lweights,
                               __local float * lxview,
                               __local float * lyview,
                               __local float * lzview
                              )
{
    int lid = get_local_id(0);
    int lsize = get_local_size(0);
    for (int i = lid; i < (* m) ; i+=lsize)
    {
        lIobs[i] = Iobs[i] ;
        lxview[i] = Viewingobs[i*3+0];
        lyview[i] = Viewingobs[i*3+1];
        lzview[i] = Viewingobs[i*3+2];
        lweights[i] = weights[i];
        ly[i] =0.0;
    }
    __local float xl[5];
    for (int i = lid; i < 5 ; i+=lsize)
    {
        xl[i] = x[i];
    }
    int mlocal = (*m);
    brad_ocl_phongs_model_est(max_iter,
                              mlocal,   //
                              xl,       //
                              output,
                              sunangles,
                              ly,
                              lJ,
                              lA,
                              tempm,
                              lIobs,
                              lweights,
                              lxview,
                              lyview,
                              lzview);
}
#endif
