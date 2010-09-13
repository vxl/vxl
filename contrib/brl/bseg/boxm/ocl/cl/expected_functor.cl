
void step_cell(__global float16* cell_data, int data_ptr,
               float d, float4 * data_return)
{
  float16 data = cell_data[data_ptr];
  // TODO - fix it

  float expected_int_cell = data.s1*data.s3+data.s5*data.s7+data.s9*(1-data.s3-data.s7);

  if (data.s0<0) return;

  //float expected_int_cell = data.s3;
  float alpha_integral = (*data_return).x;
  float vis            = (*data_return).y;
  float expected_int   = (*data_return).z;
  float intensity_norm = (*data_return).w;

  alpha_integral += data.s0*d;
  float vis_prob_end = exp(-alpha_integral);
  float omega = vis - vis_prob_end;
  expected_int += expected_int_cell*omega;
  (*data_return).x = alpha_integral;
  (*data_return).y = vis_prob_end;
  (*data_return).z = expected_int;
  (*data_return).w = intensity_norm + omega;
}

//Uses float data
void step_cell_render(__global float8* cell_data, __global float* alpha_data,int data_ptr,
                      float d, float4 * data_return)
{
  float8 data = cell_data[data_ptr];
  float alpha=alpha_data[data_ptr];

  float expected_int_cell = data.s0*data.s2+data.s3*data.s5+data.s6*(1-data.s2-data.s5);

  if (alpha<0) return;

  //float expected_int_cell = data.s3;
  float alpha_integral = (*data_return).x;
  float vis            = (*data_return).y;
  float expected_int   = (*data_return).z;
  float intensity_norm = (*data_return).w;

  alpha_integral += alpha*d;
  float vis_prob_end = exp(-alpha_integral);
  float omega = vis - vis_prob_end;
  expected_int += expected_int_cell*omega;
  (*data_return).x = alpha_integral;
  (*data_return).y = vis_prob_end;
  (*data_return).z = expected_int;
  (*data_return).w = intensity_norm + omega;
}

#if 0
//optimized data version
void step_cell_render_opt(__global uchar8* cell_data, __global float* alpha_data, int data_ptr,
                          float d, float4 * data_return)
{
  uchar8 data = cell_data[data_ptr];
  float alpha = alpha_data[data_ptr];
  float expected_int_cell = ((data.s0) * (data.s2)
                            +(data.s3) * (data.s5)
                            +(data.s6) * (255.0 - data.s2 - data.s5))/255.0/255.0;

  if (alpha<0) return;

  //float expected_int_cell = data.s3;
  float alpha_integral = (*data_return).x;
  float vis            = (*data_return).y;
  float expected_int   = (*data_return).z;
  float intensity_norm = (*data_return).w;

  alpha_integral += alpha*d;
  float vis_prob_end = exp(-alpha_integral);
  float omega = vis - vis_prob_end;
  expected_int += expected_int_cell*omega;
  (*data_return).x = alpha_integral;
  (*data_return).y = vis_prob_end;
  (*data_return).z = expected_int;
  (*data_return).w = intensity_norm + omega;
}
#endif
void step_cell_render_opt2(__global int2 * cell_data, 
                          __global float  * alpha_data, 
                                   int      data_ptr, 
                                   float    d, 
                                   float  * vis,
                                   float  * expected_i,
                                   float  * intensity_n)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  if(diff_omega<0.995)
  {
      uchar8 data = as_uchar8(cell_data[data_ptr]);
      expected_int_cell = ((data.s0) * (data.s2)
                          +(data.s3) * (data.s5)
                          +(data.s6) * (255.0 - data.s2 - data.s5))/255.0/255.0;
  }
  float omega=(*vis)*(1-diff_omega);
  (*vis)*=diff_omega;
  (*intensity_n) +=  omega;
  (*expected_i)+=expected_int_cell*omega;
}
#if 1
//optimized data version
void step_cell_render_opt(__global uchar8 * cell_data, 
                          __global float  * alpha_data, 
                                   int      data_ptr, 
                                   float    d, 
                                   float  * alpha_int,
                                   float  * expected_i,
                                   float  * intensity_n)
{
  uchar8 data = cell_data[data_ptr];
  float alpha = alpha_data[data_ptr];
  float expected_int_cell = ((data.s0) * (data.s2)
                            +(data.s3) * (data.s5)
                            +(data.s6) * (255.0 - data.s2 - data.s5))/255.0/255.0;
  if (alpha<0) return;

  //float expected_int_cell = data.s3;
  float alpha_integral = (*alpha_int);
  float vis            = exp(-alpha_integral);
  float expected_int   = (*expected_i);
  float intensity_norm = (*intensity_n);

  alpha_integral += alpha*d;
  float vis_prob_end = exp(-alpha_integral);
  float omega    = vis - vis_prob_end;
  expected_int  += expected_int_cell*omega;
  (*alpha_int)   = alpha_integral;
  (*expected_i)  = expected_int;
  (*intensity_n) = intensity_norm + omega;
}
#endif

void step_cell_change_detection(__global float8* cell_data, __global float* alpha_data,int data_ptr,
                                float d, float4 * data_return, float img_intensity)
{
  float8 data = cell_data[data_ptr];
  float alpha=alpha_data[data_ptr];

  if (alpha<0) return;

  //float expected_int_cell = data.s3;
  float alpha_integral = (*data_return).x;
  float vis            = (*data_return).y;
  float change_density = (*data_return).z;
  float intensity_norm = (*data_return).w;

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
