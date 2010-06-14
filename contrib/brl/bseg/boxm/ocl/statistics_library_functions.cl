float gauss_prob_density(float x, float mu, float sigma)
{
  return 0.398942280f*exp(-0.5f*(x - mu)*(x - mu)/(sigma*sigma))/sigma;
}

float gauss_3_mixture_prob_density(float obs, 
                                   float mu0, float sigma0, float w0,
                                   float mu1, float sigma1, float w1,
                                   float mu2, float sigma2, float w2)
{
  float sum = 0.0f;
  if(w0>0.0f)
    {
      sum += w0*gauss_prob_density(obs, mu0, sigma0);
      if(w1>0.0f)
        sum += w1*gauss_prob_density(obs, mu1, sigma1);
      if(w2>0.0f)
        sum += w2*gauss_prob_density(obs, mu2, sigma2);
    }
  return sum;
}
