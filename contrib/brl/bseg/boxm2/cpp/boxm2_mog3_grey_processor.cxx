#include "boxm2_mog3_grey_processor.h"
//:
// \file



float  boxm2_mog3_grey_processor::expected_color( vnl_vector_fixed<unsigned char, 8> mog3)
{
    float exp_intensity=(float)mog3[0]*(float)mog3[2]+
                        (float)mog3[3]*(float)mog3[5]+
                        (float)mog3[6]*(float)(255-mog3[2]-mog3[5]);

    exp_intensity/=(255.0f*255.0f);

    return exp_intensity;
}
float  boxm2_mog3_grey_processor::gauss_prob_density(float x, float mu, float sigma)
{
  return 0.398942280f*exp(-0.5f*(x - mu)*(x - mu)/(sigma*sigma))/sigma;
}
float  boxm2_mog3_grey_processor::prob_density(const vnl_vector_fixed<unsigned char, 8> & mog3, float x)
{
  float sum = 0.0f;
  float sum_weights=0.0f;
  float mu0=((float)mog3[0]/255.0f);float sigma0=((float)mog3[1]/255.0f);float w0=((float)mog3[2]/255.0f);
  float mu1=((float)mog3[3]/255.0f);float sigma1=((float)mog3[4]/255.0f);float w1=((float)mog3[5]/255.0f);
  float mu2=((float)mog3[6]/255.0f);float sigma2=((float)mog3[7]/255.0f);float w2=1-w0-w1;

  if(w0>0.0f && sigma0 >0.0f)
  {
      sum += w0*boxm2_mog3_grey_processor::gauss_prob_density(x, mu0, sigma0);
      sum_weights+=w0;
      if(w1>0.0f && sigma1 >0.0f)
      {
          sum += w1*gauss_prob_density(x, mu1, sigma1);
          sum_weights+=w1;
          if(w2>0.0f && sigma2 >0.0f)
          {
              sum += w2*gauss_prob_density(x, mu2, sigma2);
              sum_weights+=w2;
          }
      }
  }
  else 
      sum=1.0f;


  return sum;
}
