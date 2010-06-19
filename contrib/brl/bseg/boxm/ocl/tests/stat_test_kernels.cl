#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
__kernel
void
test_gaussian_pdf(__global float* input, __global float* data,
                  __global float* results)
{
  float x = data[1]; 
  float mu = input[0], sigma = input[1];

  results[0] = gauss_prob_density(x, mu, sigma);
}

__kernel
void
test_gaussian_3_mixture_pdf(__global float* input, __global float* data,
                            __global float* results)
{
  float x = data[1]; 
  float mu0 = input[0], sigma0 = input[1], w0 = input[2];
  float mu1 = input[3], sigma1 = input[4], w1 = input[5];
  float mu2 = input[6], sigma2 = input[7], w2 = input[8];

  results[0] = gauss_3_mixture_prob_density(x, mu0, sigma0, w0,
                                            mu1, sigma1, w1,
                                            mu2, sigma2, w2);
  results[1] = gauss_3_mixture_prob_density(x, mu0, sigma0, w0+0.5f*w2,
                                            mu1, sigma1, w1+0.5f*w2,
                                            mu2, sigma2, 0.0f);
  results[2] = gauss_3_mixture_prob_density(x, mu0, sigma0, 1.0f,
                                            mu1, sigma1, 0.0f,
                                            mu2, sigma2, 0.0f);
}
/* 
 * The first element of the data vector is the size, followed by the data
 * The input vector holds the initial Gaussian parameters
*/
__kernel
void test_update_gauss(__global float* input, __global float* data,
                       __global float* results)
{
  float mu = input[0], sigma = input[1]; 
  float min_sigma = input[2], rho = input[3];

  /* number of samples for updating the gaussian */
  unsigned n = (unsigned)data[0];
  for(unsigned i = 1; i<(n+1); ++i){
    update_gauss(data[i], rho, &mu, &sigma, min_sigma);
    results[2*(i-1)]=mu;     results[2*(i-1)+1]=sigma;
  }
}

__kernel
void test_mixture_sort(__global float* input, __global float* data,
                       __global float* results)
{
  float mu0 = input[0], sigma0 = input[1], w0 = input[2];
  float mu1 = input[3], sigma1 = input[4], w1 = input[5];
  float mu2 = input[6], sigma2 = input[7], w2 = input[8];  
  short Nobs0 = 0, Nobs1 = 1, Nobs2 = 2;
  sort_mix_3(&mu0, &sigma0, &w0, &Nobs0,
             &mu1, &sigma1, &w1, &Nobs1,
             &mu2, &sigma2, &w2, &Nobs2);
  results[0]=mu0;  results[1]=sigma0; results[2]=w0;
  results[3]=mu1;  results[4]=sigma1; results[5]=w1;
  results[6]=mu2;  results[7]=sigma2; results[8]=w2;
}

__kernel
void test_insert(__global float* input, __global float* data,
                 __global float* results)
{
  float init_weight = input[0], init_sigma = input[1];
  float mu0 = input[2], sigma0 = input[3], w0 = input[4];
  float mu1 = input[5], sigma1 = input[6], w1 = input[7];
  float mu2 = input[8], sigma2 = input[9], w2 = input[10];  
  short Nobs0 = 3, Nobs1 = 4, Nobs2 = 2; /* Example initial values */
  char match = -1;
  float x = data[1];
  /* replacing third component */
  insert_gauss_3(x, init_weight, init_sigma, &match,
                 &mu0, &sigma0, &w0, &Nobs0,
                 &mu1, &sigma1, &w1, &Nobs1,
                 &mu2, &sigma2, &w2, &Nobs2);
  
  results[0]=mu0;  results[1]=sigma0; results[2]=w0;  results[3] = Nobs0;
  results[4]=mu1;  results[5]=sigma1; results[6]=w1;  results[7] = Nobs1;
  results[8]=mu2;  results[9]=sigma2; results[10]=w2; results[11] = Nobs2;
  results[12]=match;

  /* replacing 2nd component */
  w1 = 0.0f, w2 = 0.0f; Nobs2 = 0; Nobs1 = 0;
  insert_gauss_3(x, init_weight, init_sigma, &match,
                 &mu0, &sigma0, &w0, &Nobs0,
                 &mu1, &sigma1, &w1, &Nobs1,
                 &mu2, &sigma2, &w2, &Nobs2);
  
  results[13]=mu0;  results[14]=sigma0; results[15]=w0;  results[16] = Nobs0;
  results[17]=mu1;  results[18]=sigma1; results[19]=w1;  results[20] = Nobs1;
  results[21]=mu2;  results[22]=sigma2; results[23]=w2; results[24] = Nobs2;
  results[25]=match;

  /* replacing 1st component */
  w0 = 0.0f; w1 = 0.0f; w2 = 0.0f, Nobs0 = 0; Nobs1 = 0; Nobs2 = 0;
  insert_gauss_3(x, init_weight, init_sigma, &match,
                 &mu0, &sigma0, &w0, &Nobs0,
                 &mu1, &sigma1, &w1, &Nobs1,
                 &mu2, &sigma2, &w2, &Nobs2);
  
  results[26]=mu0;  results[27]=sigma0; results[28]=w0;  results[29] = Nobs0;
  results[30]=mu1;  results[31]=sigma1; results[32]=w1;  results[33] = Nobs1;
  results[34]=mu2;  results[35]=sigma2; results[36]=w2; results[37] = Nobs2;
  results[38]=match;}

__kernel
void test_update_mix(__global float* input, __global float* data,
                     __global float* results)
{
  float weight = input[0], init_sigma = input[1];
  float min_sigma = input[2]; float t_match = input[3];
  float mu0 = input[4], sigma0 = input[5], w0 = input[6];
  float mu1 = input[7], sigma1 = input[8], w1 = input[9];
  float mu2 = input[10], sigma2 = input[11], w2 = input[12];  
  short Nobs0 = 0, Nobs1 = 0, Nobs2 = 0; /* Example initial values */
  float Nobs_mix = 0.0f;
  float x = data[1];
  /* number of samples for updating the gaussian */
  unsigned n = (unsigned)data[0];
  for(unsigned i = 1; i<(n+1); ++i){

    update_gauss_3_mixture(data[i], weight, t_match,init_sigma, min_sigma, 
                           &mu0, &sigma0, &w0, &Nobs0,
                           &mu1, &sigma1, &w1, &Nobs1,
                           &mu2, &sigma2, &w2, &Nobs2,
                           &Nobs_mix);

    unsigned b = 13*(i-1);
    results[b]=mu0; results[b+1]=sigma0; results[b+2]=w0; results[b+3]=Nobs0;
    results[b+4]=mu1; results[b+5]=sigma1; results[b+6]=w1; results[b+7]=Nobs1;
    results[b+8]=mu2; results[b+9]=sigma2;results[b+10]=w2;results[b+11]=Nobs2;
    results[b+12]=Nobs_mix;
  }
}
