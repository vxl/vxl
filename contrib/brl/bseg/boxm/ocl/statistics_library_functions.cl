#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
float gauss_prob_density(float x, float mu, float sigma)
{
  return 0.398942280f*exp(-0.5f*(x - mu)*(x - mu)/(sigma*sigma))/sigma;
}

/* This mixture distribution function uses the value of the weights to 
 *determine the number of components. A weight of 0 indicates the 
 *component is not used */
float gauss_3_mixture_prob_density(float x, 
                                   float mu0, float sigma0, float w0,
                                   float mu1, float sigma1, float w1,
                                   float mu2, float sigma2, float w2)
{
  float sum = 0.0f;
  if(w0>0.0f)
    {
      sum += w0*gauss_prob_density(x, mu0, sigma0);
      if(w1>0.0f)
        sum += w1*gauss_prob_density(x, mu1, sigma1);
      if(w2>0.0f)
        sum += w2*gauss_prob_density(x, mu2, sigma2);
    }
  return sum;
}

/*
 * update a single 1d gaussian with a sample. The gaussian parameters are
 * mutated in place. 
 *
 */
void update_gauss(float x, float rho, float* mu, float* sigma,
                  float min_sigma)
{
  float var = (*sigma)*(*sigma);
  float diff = x-(*mu);
  var = (1.0f-rho)*(var +rho*diff*diff);
  *mu = *mu + rho*diff;
  *sigma = sqrt(var);
  *sigma = (*sigma < min_sigma)? min_sigma: *sigma;
}

/* use insertion sort to order the components on the factor w/sigma
 * no assumptions are made about the number of components   
 * or if the mixture is already sorted. Based on qsort using pivot comp 1. 
 */
void sort_mix_3(float* mu0, float* sigma0, float* w0, short* Nobs0,
                float* mu1, float* sigma1, float* w1, short* Nobs1,
                float* mu2, float* sigma2, float* w2, short* Nobs2)
{
  if((*w2)==0.0f&&(*w1)==0.0f) return; /* no need to sort */

  float fa = (*w0)/(*sigma0), fb = (*w1)/(*sigma1), fc = (*w2)/(*sigma2);
  if((*w2)==0.0f) 
    if(fa<fb){/*only need to swap a and b*/
      float tmu0 = *mu0, tsig0 = *sigma0, tw0 = *w0;
      short n0 = *Nobs0;
      *mu0 = *mu1; *sigma0 = *sigma1; *w0 = *w1; *Nobs0 = *Nobs1;
      *mu1 = tmu0; *sigma1 = tsig0; *w1 = tw0; *Nobs1 = n0;
      return ;
    }else return;


  if(fa>=fb&&fb>=fc)/* [a b c ] - already sorted */
    return;
  if(fa<fb&&fb<fc)/* [c b a] - swap a and c */
    {
      float tmu0 = *mu0, tsig0 = *sigma0, tw0 = *w0;
      short n0 = *Nobs0;
      *mu0 = *mu2; *sigma0 = *sigma2; *w0 = *w2; *Nobs0 = *Nobs2;
      *mu2 = tmu0; *sigma2 = tsig0; *w2 = tw0; *Nobs2 = n0;
      return ;
    }
  if(fa<fb&&fb>=fc)
    if(fa>=fc)/* [b a c] - c stays where it is and a b swap */
      {
        float tmu0 = *mu0, tsig0 = *sigma0, tw0 = *w0;
        short n0 = *Nobs0;
        *mu0 = *mu1; *sigma0 = *sigma1; *w0 = *w1; *Nobs0 = *Nobs1;
        *mu1 = tmu0; *sigma1 = tsig0; *w1 = tw0; *Nobs1 = n0;
        return;
      }else{
        /* [b c a] - two swaps */
        float tmu0 = *mu0, tsig0 = *sigma0, tw0 = *w0;
        short n0 = *Nobs0;
        *mu0 = *mu1; *sigma0 = *sigma1; *w0 = *w1; *Nobs0 = *Nobs1;
        *mu1 = *mu2; *sigma1 = *sigma2; *w1 = *w2; *Nobs1 = *Nobs2;
        *mu2 = tmu0; *sigma2 = tsig0; *w2 = tw0; *Nobs2 = n0;
        return;
      }
  if(fa>=fb&&fb<fc)
    if(fa>=fc)/* [a c b] - b and c swap */
      {
        float tmu1 = *mu1, tsig1 = *sigma1, tw1 = *w1;
        short n1 = *Nobs1;
        *mu1 = *mu2; *sigma1 = *sigma2; *w1 = *w2; *Nobs1 = *Nobs2;
        *mu2 = tmu1; *sigma2 = tsig1; *w2 = tw1; *Nobs2 = n1;
        return;
      }else{
        /* [c a b] - two swaps */
        float tmu0 = *mu0, tsig0 = *sigma0, tw0 = *w0; 
        short n0 = *Nobs0;
        *mu0 = *mu2; *sigma0 = *sigma2; *w0 = *w2; *Nobs0 = *Nobs2; 
        *mu2 = *mu1; *sigma2 = *sigma1; *w2 = *w1; *Nobs2 = *Nobs1; 
        *mu1 = tmu0; *sigma1 = tsig0; *w1 = tw0; *Nobs1 = n0;
        return;
      }
}

/*
 * insert a new component in the mixture. The mixture is assumed
 * to be sorted so that if w2>0 the third component is the one replaced.
 */
void insert_gauss_3(float x, float init_weight, float init_sigma, char* match, 
                    float* mu0, float* sigma0, float* w0, short* Nobs0,
                    float* mu1, float* sigma1, float* w1, short* Nobs1,
                    float* mu2, float* sigma2, float* w2, short* Nobs2)
{
  if(*w2>0.0f||*w1>0.0f)  /* replace the third component */
    {
      float adjust = *w0 + *w1;
      adjust = (1.0f - init_weight)/adjust;
      *w0 = (*w0)*adjust; *w1 = (*w1)*adjust;
      *mu2 = x;
      *sigma2 = init_sigma;
      *w2 = init_weight;
      *Nobs2 = 1;
      *match = (char)2;
      return;
    }else if((*w0)>0.0f){/* replace the second component */
      *w0 = (1.0f-init_weight);
      *mu1 = x;
      *sigma1 = init_sigma;
      *w1 = init_weight;
      *Nobs1 = 1;
      *match = (char)1;
      return;
    }else{/* replace the first component */
      /*note that in C++ the weights don't sum to 1?
       see bsta_adaptive_updater.h - ::insert bug? */
      *w0 = 1.0f; /*init_weight in C++ */
      *mu0 = x;
      *sigma0 = init_sigma;
      *Nobs0 = 1;
      *match = (char)0;
    }
}

/* 
 *update a 1d gaussian mixture with a sample. The mixture parameters are
 * mutated in place. Note that the number of components can vary between
 * 0 and 3 as the updating proceeds. Initially, the mixture contains no 
 * components. The number of observations for each component is Nobs(i).
 * The number of observations for the whole mixture is Nobs_mix. When 
 * a new component is inserted in the mixture, it is given a mean of x
 * and a sigma of min_sigma. The threshold t_match deterimines the maximum
 * Mahalanobis distance that the sample can have in order to be considered
 * to belong to a given component.
 */
void update_gauss_3_mixture(float x, float w, float t_match,
                            float init_sigma, float min_sigma,
                            float* mu0, float* sigma0, float* w0, short* Nobs0,
                            float* mu1, float* sigma1, float* w1, short* Nobs1,
                            float* mu2, float* sigma2, float* w2, short* Nobs2,
                            float* Nobs_mix) 
{
  int match = -1;
  (*Nobs_mix) += w;
  float alpha = w/(*Nobs_mix), tsq=t_match*t_match;
  float weight = 0.0f, rho = 0.0f;
  /* test for a match of component 0 */
  if(*w0>0){
    weight = (1.0f-alpha)*(*w0);
    if(match<0 && 
       ((x-*mu0)*(x-*mu0)/((*sigma0)*(*sigma0))) < tsq){
      weight += alpha;
      (*Nobs0)++;
      rho = (1.0f-alpha)/((float)*Nobs0) + alpha;
      update_gauss(x, rho, mu0, sigma0, min_sigma);
      match = 0;
    }
    *w0 = weight;
  }
  /* test for a match of component 1 */
  if(*w1>0){
    weight = (1.0f-alpha)*(*w1);
    if(match<0 && ((x-*mu1)*(x-*mu1)/((*sigma1)*(*sigma1))) < tsq){
      weight += alpha;
      (*Nobs1)++;
      rho = (1.0f-alpha)/((float)*Nobs1) + alpha;
      update_gauss(x, rho, mu1, sigma1, min_sigma);
      match = 1;
    }
    *w1 = weight;
  }
  /* test for a match of component 2 */
  if(*w2>0){
    weight = (1.0f-alpha)*(*w2);
    if(match<0 && ((x-*mu2)*(x-*mu2)/((*sigma2)*(*sigma2))) < tsq){
      weight += alpha;
      (*Nobs2)++;
      rho = (1.0f-alpha)/((float)*Nobs2) + alpha;
      update_gauss(x, rho, mu2, sigma2, min_sigma);
      match = 2;
    }
    *w2 = weight;
  }
  /* If there were no matches then insert a new component */
  if(match<0)
    insert_gauss_3(x, alpha, init_sigma, &match,
                   mu0, sigma0, w0, Nobs0, 
                   mu1, sigma1, w1, Nobs1,
                   mu2, sigma2, w2, Nobs2);

  /* If there is more than one component, sort the components with
   * respect to weight/sigma.  
   */
  if(match>0)
    sort_mix_3(mu0, sigma0, w0, Nobs0, 
               mu1, sigma1, w1, Nobs1,
               mu2, sigma2, w2, Nobs2);
}

