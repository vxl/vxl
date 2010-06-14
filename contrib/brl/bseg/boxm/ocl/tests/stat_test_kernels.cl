#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
__kernel
void
test_gaussian_pdf(__global float* input, __global float* results)
{
  float x = input[0], mu = input[1], sigma = input[2];

  results[0] = gauss_prob_density(x, mu, sigma);
}

__kernel
void
test_gaussian_3_mixture_pdf(__global float* input, __global float* results)
{
  float x = input[0], mu0 = input[1], sigma0 = input[2], w0 = input[3];
  float mu1 = input[4], sigma1 = input[5], w1 = input[6];
  float mu2 = input[7], sigma2 = input[8], w2 = input[9];

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
