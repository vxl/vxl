//step_cell functors for various render functions

#if GAUSS_TYPE_2
    #define MOG_TYPE uchar2
    #define CONVERT_FUNC(lhs,data) uchar2 lhs1 = as_uchar2(data); uchar8 lhs = as_uchar8(0.0); lhs.s0 = lhs1.s0; lhs.s1 = lhs1.s1; lhs.s2 = 255;
    #define NORM 255;
#endif

#ifdef RENDER
void step_cell_render(__global MOG_TYPE * cell_data,
                      __global float    * alpha_data,
                               int        data_ptr,
                               float      d,
                               float    * vis,
                               float    * expected_i)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  // for rendering only
  if (diff_omega<0.995f)
  {
    CONVERT_FUNC(udata,cell_data[data_ptr]);
    float8  data=convert_float8(udata)/(float)NORM;
    EXPECTED_INT(expected_int_cell,data);
  }
  float omega=(*vis) * (1.0f - diff_omega);
  (*vis) *= diff_omega;
  (*expected_i)+=expected_int_cell*omega;
}
#endif

#ifdef RENDER_VIEW_DEP
void step_cell_render(__global MOG_TYPE * cell_data,
                       __global float   * alpha_data,
                       int        data_ptr,
                       float    * app_model_weights,
                       float      d,
                       float    * vis,
                       float    * expected_i)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  // for rendering only

  if (diff_omega<0.995f)
  {
#ifdef MOG_VIEW_DEP_COMPACT
    CONVERT_FUNC_FLOAT16(mixture,cell_data[data_ptr])/NORM;
#else
    float16 mixture = cell_data[data_ptr];
#endif
    float* mixture_array = (float*)(&mixture);

    float sum_weights = 0;
    for(short i= 0; i < 8; i++)
    {
        if(app_model_weights[i] > 0.01f)
            expected_int_cell += app_model_weights[i] * mixture_array[2*i];
    }

  }

  float omega=(*vis) * (1.0f - diff_omega);
  (*vis) *= diff_omega;
  (*expected_i)+=expected_int_cell*omega;
}
#endif //RENDER_VIEW_DEP

#ifdef RENDER_MAX
void step_cell_render_max(__global MOG_TYPE * cell_data,
                          __global float    * alpha_data,
                                   int        data_ptr,
                                   float      d,
                                   float    * vis,
                                   float    * expected_i,
                                   float    * max_omega)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  // for rendering only

  float omega=(*vis) * (1.0f - diff_omega);
  (*vis) *= diff_omega;
  if (omega > (*max_omega) )
  {
    (*expected_i)=cell_data[data_ptr];
    (*max_omega) = omega ;
  }
}
#endif

#ifdef RENDER_ALPHA_INTEGRAL
void step_cell_alpha_integral(__global float  * alpha_data,
                                       int      data_ptr,
                                       float    d,
                                       float  * alpha_integral)
{
  float alpha = alpha_data[data_ptr];
  (*alpha_integral) += alpha*d;
}
#endif

#ifdef RENDER_USING_ALPHA_INTEGRAL
void step_cell_render_using_alpha_intergal( __global MOG_TYPE * cell_data,
                                            __global float    * alpha_data,
                                                     int        data_ptr,
                                                     float      d,
                                                     float    * alpha_int,
                                                     float    * alpha_int_cum,
                                                     float    * expected_i)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  // for rendering only
  if (diff_omega<0.995f)
  {
    CONVERT_FUNC(udata,cell_data[data_ptr]);
    float8  data=convert_float8(udata)/NORM;
    EXPECTED_INT(expected_int_cell,data);
  }
  (*alpha_int) +=  alpha*d ;
  float vis = exp(-((*alpha_int_cum)-(*alpha_int)));
  float omega=vis * (1.0f - diff_omega);

  (*expected_i)+=expected_int_cell*omega;
}
#endif

#ifdef RENDER_SUN_VIS
void step_cell_render_sun_vis(__global float   * auxsun,
                              __global float  * alpha_data,
                                       int      data_ptr,
                                       float    d,
                                       float  * vis,
                                       float  * expected_i)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float omega=(*vis) * (1.0f - diff_omega);
  (*vis) *= diff_omega;
  (*expected_i)+=auxsun[data_ptr]*omega;
}
#endif

void step_cell_change_detection_uchar8(__global uchar8* cell_data, __global float* alpha_data,int data_ptr,
                                       float d, float * vis,float * change_density, float img_intensity)
{
  uchar8 uchar_data = cell_data[data_ptr];
  float alpha=alpha_data[data_ptr];

  if (alpha<0) return;

  float8 data= convert_float8(uchar_data)/255.0f;

  float prob_den=gauss_3_mixture_prob_density(img_intensity,
                                              data.s0,data.s1,data.s2,
                                              data.s3,data.s4,data.s5,
                                              data.s6,data.s7,1-data.s2-data.s5);

  float vis_prob_end = exp(-alpha*d);
  float omega = (*vis)*(1 - vis_prob_end);
  (*change_density) += prob_den*omega;
  (*vis)=(*vis)*vis_prob_end;
}

#ifdef CHANGE
void step_cell_change_detection_uchar8_w_expected(__global MOG_TYPE * cell_data,
                                                  __global float* alpha_data,
                                                  int data_ptr,float d,
                                                  float * vis,
                                                  float * change_density,
                                                  float * e_change_density,
                                                  float4 img_intensity,
                                                  float e_img_intensity,
                                                  int bit_index)
{
  //uchar8 uchar_data = cell_data[data_ptr];
  CONVERT_FUNC(uchar_data,cell_data[data_ptr]);
  float8 data= convert_float8(uchar_data)/NORM;

#ifdef USE_MAX_MODE
  //calc using max only if 1) Weight is non-zero and 2) variance is above some thresh
  float prob_den = 0.0f;
  float w2 = 1.0f - data.s2 - data.s5;
  float mode1_prob = (data.s2 > 10e-6f && data.s1 > .01f) ? gauss_prob_density(img_intensity, data.s0, data.s1) : 0.0f;
  float mode2_prob = (data.s5 > 10e-6f && data.s4 > .01f) ? gauss_prob_density(img_intensity, data.s3, data.s4) : 0.0f;
  float mode3_prob = (w2      > 10e-6f && data.s7 > .01f) ? gauss_prob_density(img_intensity, data.s6, data.s7) : 0.0f;
  prob_den = fmax(mode1_prob, fmax(mode2_prob, mode3_prob));
#else
  //choose value based on cell depth
  int cell_depth = get_depth(bit_index);
  float img_in = img_intensity.x;
#if 0
  if (cell_depth==0)
    img_in = img_intensity.w;
  else if (cell_depth==1)
    img_in = img_intensity.z;
  else if (cell_depth==2)
    img_in = img_intensity.y;
#endif
  float prob_den=gauss_3_mixture_prob_density(img_in,
                                              data.s0,data.s1,data.s2,
                                              data.s3,data.s4,data.s5,
                                              data.s6,data.s7,1-data.s2-data.s5);

#endif

  float alpha=alpha_data[data_ptr];
  float prob = 1-exp(-alpha*d);
  float omega = (*vis)*prob;
  (*vis)=(*vis)*(1-prob);
  (*change_density) += prob_den*omega;
  float e_prob_den=gauss_3_mixture_prob_density(e_img_intensity,
                                                data.s0,data.s1,data.s2,
                                                data.s3,data.s4,data.s5,
                                                data.s6,data.s7,1-data.s2-data.s5);
  (*e_change_density) += e_prob_den*omega;
}
#endif

#ifdef PROB_IMAGE
void step_cell_compute_probability_of_intensity(__global MOG_TYPE * cell_data,
                                                __global float* alpha_data,
                                                int data_ptr,
                                                float d,
                                                float * vis,
                                                float * prob_image,
                                                float img_intensity)
{
  CONVERT_FUNC(uchar_data,cell_data[data_ptr]);
  float8 data= convert_float8(uchar_data)/NORM;
  float prob_den=gauss_3_mixture_prob_density(img_intensity,
                                              data.s0,data.s1,data.s2,
                                              data.s3,data.s4,data.s5,
                                              data.s6,data.s7,1-data.s2-data.s5);
  float alpha    = alpha_data[data_ptr];
  float prob     = 1-exp(-alpha*d);
  float omega    = (*vis)*prob;
  (*vis)         = (*vis)*(1-prob);
  (*prob_image) += prob_den*omega;
}
#endif

void step_cell_change_detection(__global uchar8* cell_data, __global float* alpha_data,int data_ptr,
                                float d, float4 * data_return, float img_intensity)
{
  uchar8 uchar_data = cell_data[data_ptr];
  float alpha=alpha_data[data_ptr];

  if (alpha<0) return;

  //float expected_int_cell = data.s3;
  float alpha_integral = (*data_return).x;
  float vis            = (*data_return).y;
  float change_density = (*data_return).z;
  float intensity_norm = (*data_return).w;

  float8 data= convert_float8(uchar_data)/255.0f;

  float prob_den=gauss_3_mixture_prob_density(img_intensity,
                                              data.s0,data.s1,data.s2,
                                              data.s3,data.s4,data.s5,
                                              data.s6,data.s7,1-data.s2-data.s5);

  alpha_integral += alpha*d;
  float vis_prob_end = exp(-alpha_integral);
  float omega = vis - vis_prob_end;
  change_density += prob_den*omega;
  (*data_return).x = alpha_integral;
  (*data_return).y = vis_prob_end;
  (*data_return).z = change_density;
  (*data_return).w = intensity_norm + omega;
}

void step_cell_render_depth(__global float* alpha_data,int data_ptr,
                            float d,float depth, float4 * data_return)
{
  float alpha=alpha_data[data_ptr];

  if (alpha<0) return;

  float alpha_integral = (*data_return).x;
  float vis            = (*data_return).y;
  float expected_depth = (*data_return).z;
  float norm           = (*data_return).w;

  alpha_integral += alpha*d;

  float vis_prob_end = exp(-alpha_integral);
  float omega = vis - vis_prob_end;
  expected_depth += depth*omega;
  (*data_return).x = alpha_integral;
  (*data_return).y = vis_prob_end;
  (*data_return).z = expected_depth;
  (*data_return).w = norm + omega;
}

void step_cell_visibility(__global float* cell_data, int data_ptr,
                          float d, float * data_return)
{
  float alpha = cell_data[data_ptr];
  float alpha_integral = (*data_return);
  alpha_integral += alpha*d;
  (*data_return) = alpha_integral;
}

#ifdef RENDER_VISIBILITY
void step_cell_vis(__global float* cell_data, int data_ptr,
                   float d, float * vis)
{
  float alpha = cell_data[data_ptr];

  (*vis) = (*vis) * exp(-alpha* d);
}
#endif

#ifdef RENDER_DEPTH
void step_cell_render_depth2(float depth,
    float block_len,
    __global float  * alpha_data,
    int      data_ptr,
    float    d,
    float  * vis,
    float  * expected_depth,
    float  * expected_depth_square,
    float  * probsum,
    float * t)
{
    const float alpha = alpha_data[data_ptr];
    const float diff_omega = exp(-alpha*d);
    const float omega = (*vis) * (1.0f - diff_omega);
    (*probsum) += omega;
    (*t) = depth*block_len;
    //: to compute the expected depth
    if (alpha > 1e-6) {
        //float expdepth = (*vis)*((*t)*(1 - diff_omega) + d*(-diff_omega) + (1.0 - diff_omega) / alpha);
        const float t0 = *t;
        const float t1 = t0 + d;
        // the derivation for the following two equations is a bit ugly, but is based on starting
        // with the equation for expected depth, which is:
        // E[depth] = int_0^inf t * exp(-int_0^t alpha(s) ds) dt
        float expdepth_contribution = (*vis) / alpha * (-diff_omega*(alpha*t1 + 1) + alpha*t0 + 1.0);
        float expdepth_sqrd_contribution = (*vis)*(diff_omega*(-t1*t1 - 2 * t1 / alpha + 2 / (-alpha*alpha)) - (-t0*t0 - 2 * t0 / alpha + 2 / (-alpha*alpha)));
        (*expected_depth) += expdepth_contribution;
        (*expected_depth_square) += expdepth_sqrd_contribution;
        (*vis) *= diff_omega;
    }
    (*t) = depth*block_len + d;
}
void step_cell_render_height(float depth,
    float block_len,
    __global float  * alpha_data,
    int      data_ptr,
    float    d,
    float  * vis,
    float  * expected_depth,
    float  * expected_depth_square,
    float  * probsum,
    float * t)
{


    const float alpha = alpha_data[data_ptr];
    const float diff_omega = exp(-alpha*d);
    const float omega = (*vis) * (1.0f - diff_omega);
    (*probsum) += omega;
    (*t) = depth*block_len;
    //: to compute the expected depth
    if (alpha > 1e-6) {
        const float t0 = *t;
        const float t1 = t0 + d;
        float expdepth_contribution = (t0 + t1)  * omega / 2.0f;
        float expdepth_sqrd_contribution = (t0 + t1)*(t0 + t1) * omega / 4.0f;
        (*expected_depth) += expdepth_contribution;
        (*expected_depth_square) += expdepth_sqrd_contribution;
        (*vis) *= diff_omega;
    }
    (*t) = depth*block_len + d;
}
void step_cell_render_depth2_byte(float depth,
                                  float block_len,
                                  __global float  * alpha_data,
                                  int      data_ptr,
                                  float    d,
                                  float  * vis,
                                  float  * expected_depth,
                                  float  * expected_depth_square,
                                  float  * probsum,
                                  float * t)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float omega=(*vis) * (1.0f - diff_omega);
  (*probsum)+=omega;
  (*vis)    *= diff_omega;
  (*expected_depth)+=depth*omega;
  (*expected_depth_square)+=depth*depth*omega;
  (*t)=depth*block_len;
  //(*t) = depth;
}
#endif

#ifdef  RENDER_HEIGHT_MAP
void step_cell_render_depth2(float depth,
                             __global float  * alpha_data,
                             __global MOG_TYPE * cell_data,
                             int      data_ptr,
                             float    d,
                             float  * vis,
                             float  * expected_depth,
                             float  * expected_depth_square,
                             float  * probsum,
                             float  * expected_i)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);

  ////for rendering only
  float expected_int_cell=0.0f;
  if (diff_omega<0.995f)
  {
    CONVERT_FUNC(udata,cell_data[data_ptr]);
    float8  data=convert_float8(udata)/NORM;
    expected_int_cell = ( data.s0  *  data.s2
                        + data.s3  *  data.s5
                        + data.s6  * (1 - data.s2 - data.s5));
  }
  float omega=(*vis) * (1.0f - diff_omega);
  (*expected_i)+=expected_int_cell*omega;

  ////depth
  (*probsum)+=omega;
  (*vis) *= diff_omega;
  (*expected_depth)+=depth*omega;
  (*expected_depth_square)+=depth*depth*omega;
}

#endif

#ifdef RENDER_Z_IMAGE

void step_cell_render_z(float z,
                        __global float  * alpha_data,
                        int      data_ptr,
                        float    seg_len,
                        float  * vis,
                        float  * expected_z,
                        float  * expected_z_sqr,
                        float  * probsum)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*seg_len);
  float omega=(*vis) * (1.0f - diff_omega);
  (*probsum)+=omega;
  (*vis) *= diff_omega;
  (*expected_z)+= z*omega;
  (*expected_z_sqr)+= z*z*omega;
}
#endif

#ifdef RENDER_NAA
void step_cell_render_naa(AuxArgs aux_args,
                          int      data_ptr,
                          float    d,
                          float  * vis,
                          float  * expected_i)
{
  float alpha = aux_args.alpha[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  // for rendering only
  if (diff_omega<0.995f)
  {
    __global float16 *albedos = (__global float16*)&(aux_args.naa_apm[data_ptr*32]);
    __global float16 *normal_weights = (__global float16*)&(aux_args.naa_apm[data_ptr*32 + 16]);

    float16 predictions = *aux_args.radiance_scales * (*albedos) + *aux_args.radiance_offsets;
    expected_int_cell = dot(predictions, *normal_weights);
  }
  float omega=(*vis) * (1.0f - diff_omega);
  (*vis) *= diff_omega;
  (*expected_i)+=expected_int_cell*omega;
}
#endif

#ifdef RENDER_ALBEDO_NORMAL
void step_cell_render_albedo_normal(AuxArgs aux_args,
                                    int      data_ptr,
                                    float    d,
                                    float  * vis
                                   )
{
  float alpha = aux_args.alpha[data_ptr];
  float diff_omega = exp(-alpha*d);
  float4 expected_albedo_normal_cell = (float4)(0);
  // for rendering only
  if (diff_omega<0.995f)
  {
    __global float16 *albedos = (__global float16*)&(aux_args.naa_apm[data_ptr*32]);
    __global float16 *normal_weights = (__global float16*)&(aux_args.naa_apm[data_ptr*32 + 16]);

    expected_albedo_normal_cell.s0 = dot(*albedos, *normal_weights);
    expected_albedo_normal_cell.s1 = dot(*aux_args.normals_x, *normal_weights);
    expected_albedo_normal_cell.s2 = dot(*aux_args.normals_y, *normal_weights);
    expected_albedo_normal_cell.s3 = dot(*aux_args.normals_z, *normal_weights);
  }
  float omega=(*vis) * (1.0f - diff_omega);
  (*vis) *= diff_omega;
  *(aux_args.expected_albedo_normal) += expected_albedo_normal_cell*omega;
}
#endif
