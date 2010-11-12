//: Expected functor debug

void step_cell_render( __global int2   * cell_data, 
                       __global float  * alpha_data, 
                                int      data_ptr, 
                                float    d, 
                                float  * vis,
                                float  * expected_i,
                                int    * cycleCount)
{
  float alpha = alpha_data[data_ptr];
  float diff_omega=exp(-alpha*d);
  float expected_int_cell=0.0f;
  *cycleCount += 11;
  if(diff_omega<0.995f)
  {
      uchar8 data = as_uchar8(cell_data[data_ptr]);
      expected_int_cell = ((data.s0) * (data.s2)
                          +(data.s3) * (data.s5)
                          +(data.s6) * (255.0f - data.s2 - data.s5))/255.0f/255.0f;
      *cycleCount += 12;
  }
  float omega=(*vis) * (1.0f - diff_omega);
  (*vis) *= diff_omega;
  (*expected_i)+=expected_int_cell*omega;
  *cycleCount += 9;
}
