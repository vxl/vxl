#ifdef PHONGS


typedef struct
{
  __local float* xview;
  __local float* yview;
  __local float* zview;
  __local float * Iobs;
  __local float * weights;
  __global float * sunangles;
} AuxArgs;


void f(__local float * x,
       __local float * y,
       int m,
       int n,
       AuxArgs args)
{
  int lid = get_local_id(0);
  int lsize = get_local_size(0);

  float kd = x[0] ;
  float ks = x[1] ;
  float gamma = x[2] ;

  float theta = x[3] ;
  float phi = x[4];

  float sun_theta = (args.sunangles)[0];
  float sun_phi =   (args.sunangles)[1];

  float4 normal = {sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta), 0};
  float4 sun_dir = {sin(sun_theta)*cos(sun_phi), sin(sun_theta)*sin(sun_phi), cos(sun_theta), 0};
  float4 sun_reflected_dir = sun_dir - 2*normal*dot(normal,sun_dir);

  float diffuse = (kd)* fabs(dot(normal,sun_dir));
  for (int i = lid; i < m; i+=lsize)
  {
    float4 view_dir = {args.xview[i],args.yview[i],args.zview[i],0};
    float specular = fabs(dot(view_dir,sun_reflected_dir));
    y[i]=(diffuse + (ks)*pow(specular,gamma)-args.Iobs[i])*sqrt(args.weights[i]);
  }
}

void Jacobian(__local float * x,
              __local float * J,
              int  m,
              int  n,
              AuxArgs args)
{
  int lid = get_local_id(0);
  int lsize = get_local_size(0);

  float kd = x[0] ;
  float ks = x[1] ;
  float gamma = x[2] ;

  float theta = x[3];
  float phi   = x[4];

  float sun_theta = (args.sunangles)[0];
  float sun_phi   = (args.sunangles)[1];

  float4 normal   = {sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta), 0};

  float4 dndtheta = {cos(theta)*cos(phi), cos(theta)*sin(phi), -sin(theta), 0};
  float4 dndphi   = {-sin(theta)*sin(phi), sin(theta)*cos(phi), 0 , 0};

  float4 sun_dir = {sin(sun_theta)*cos(sun_phi), sin(sun_theta)*sin(sun_phi), cos(sun_theta), 0};
  float4 sun_reflected_dir = sun_dir - 2*normal*dot(normal,sun_dir);

  float diffuse = fabs(kd)* fabs(dot(normal,sun_dir));
  for (int i = lid; i < m; i+=lsize)
  {
    float4 view_dir= {args.xview[i],args.yview[i],args.zview[i],0};
    float specular = fabs(dot(view_dir,sun_reflected_dir));
    float exp_term = pow(specular,gamma);
    float w = sqrt(args.weights[i]);
    J[i*n+0]    =  fabs(dot(normal,sun_dir))*w;
    J[i*n+1]    =  exp_term*w;
    J[i*n+2]    =  ks*exp_term*log(specular)*w;
    J[i*n+3]    =  kd*dot(dndtheta,sun_dir) *w
       +gamma*ks*w*pow(specular,gamma-1)*(-2)*(dot(sun_dir,dndtheta)*dot(normal,view_dir) +dot(sun_dir,normal)*dot(dndtheta,view_dir) );
    J[i*n+4]    =  kd*dot(dndphi,sun_dir) *w
       +gamma*ks*w*pow(specular,gamma-1)*(-2)*(dot(sun_dir,dndphi)*dot(normal,view_dir) +dot(sun_dir,normal)*dot(dndphi,view_dir) );;
  }
}

float levenberg_marquadt_solve(__constant int * max_iter,   // x : nx1
                               int  m,int  n,
                               __global float * output,
                               __local float * x,
                               __local float * h,// J : mxn
                               __local float * y,        // A: nxn
                               __local float * g,
                               __local float * J,
                               __local float * A,
                               __local float * tempm,
                               __local float * tempn,
                               AuxArgs  args);

void
brad_ocl_phongs_model_est(__constant int * max_iter,
                          int  m,     //
                          __local float * x,     //
                          __global float * output,
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
  // create local variables of the dimension of unknown variable.
  int n  = 5;
  __local float  lx[5];
  __local float  lh[5];
  __local float  lg[5];
  __local float  tempn[5];

  for ( int i = lid; i < n ; i+=lsize)
  {
    lx[i] = 0.0;
    //lx[i] =0.0;
    lh[i] =0.0;
    lg[i] =0.0;
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  AuxArgs aux_args;
  aux_args.xview = lxview;
  aux_args.yview = lyview;
  aux_args.zview = lzview;
  aux_args.Iobs  = lIobs;
  aux_args.weights  = lweights;
  aux_args.sunangles = sunangles;
  // call Levenberg Marquardt
  float err  = levenberg_marquadt_solve(max_iter,m,n,output,
                                        lx,lh,ly,lg,lJ,lA,
                                        tempm,tempn,aux_args);
  // get the optimized params out
  for (unsigned i = lid ; i < n; i+=lsize)
    x[i] = lx[i];
  barrier(CLK_LOCAL_MEM_FENCE);
#if 0
  if (lid == 0)
    x[5] =  err ;
  barrier(CLK_LOCAL_MEM_FENCE);
#endif
}

float variance_multiplier(float n_obs)
{
  if (n_obs<2) // this shouldn't happen !!
    return 100.0;
  if (n_obs==2)
    return 79.79;
  float noff = n_obs-1.5;
  float npow = pow(noff, 1.55);
  return 1.28 + 20.0/(1.0+npow);
}

void dot_product(__local float * x1, __local float * x2, __local float * dotproduct,  int  n );

float
brad_ocl_phongs_model_est_var( int m,
                               __local float * result,
                               __global float * sunangles,
                               __local float * ly,
                               __local float * lIobs,
                               __local float * lweights,
                               __local float * lxview,
                               __local float * lyview,
                               __local float * lzview)
{
  int lid = get_local_id(0);
  AuxArgs aux_args;
  aux_args.xview = lxview;
  aux_args.yview = lyview;
  aux_args.zview = lzview;
  aux_args.Iobs  = lIobs;
  aux_args.weights  = lweights;
  aux_args.sunangles = sunangles;

  __local float ftf[1];
  __local float wtw[1];
  __local float var[1];

  f(result,ly,m,5,aux_args);
  dot_product(ly,ly,ftf,m);
  dot_product(lweights,lweights,wtw,m);

  if (lid == 0)
  {
    if (wtw[0] <5)
      var[0] = 10.0;
    else
      var[0] = ftf[0] /(wtw[0]);
  }
  barrier(CLK_LOCAL_MEM_FENCE);
  return var[0];
}

#endif
