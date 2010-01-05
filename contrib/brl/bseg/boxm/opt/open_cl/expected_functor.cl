void expected_image_funct(__global float16* cell_data, int data_ptr,
                          float d, float4 * data_return)
{

  float16 data = cell_data[data_ptr];
  float alpha = data.s0;
  
  //: TODO fix it
  float expected_int_cell = data.s3*data.s5+data.s6*data.s8+data.s9*data.sb;

  
  if(alpha<0) return;


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
