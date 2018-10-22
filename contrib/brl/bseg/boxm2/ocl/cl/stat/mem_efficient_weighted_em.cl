//-------------------------------------------------------
//:
// \file
// \brief Weighted EM Function and Weighted Gaussian functions
// Takes in an array of samples (obs) and an array of weights (vis)
// runs EM for a mixture of 3 gaussians
// Define:
//  - SAMPLE_TYPE
//  - WEIGHT_TYPE
//

//: define sample type and conversion to float

#define SAMPLE_TYPE __private uchar
#define SAMPLE2FLOAT(samp)  (convert_float(samp)/255.0f)

//: define EM weight type (may be different than GAUSS weight type
#define EM_WEIGHT_TYPE __private float
#define EM_WEIGHT2FLOAT(samp)  (samp)


//: define weight and conversion to float
#define GAUSS_WEIGHT_TYPE const float
#define GAUSS_WEIGHT2FLOAT(samp)  samp;

//: weighted mean/variance calculation from global obs and weights
float2 weighted_mean_var(SAMPLE_TYPE* obs, GAUSS_WEIGHT_TYPE* vis, int numSamples)
{
  if (numSamples == 0)
    return (float2)(0.0f);

  else if (numSamples == 1)
    return (float2) ( SAMPLE2FLOAT(obs[0]), .1f );

  float sample_mean = 0.0f;
  float sum_weights = 0.0f;

  for (uint i=0; i<numSamples; ++i) {
    float w = GAUSS_WEIGHT2FLOAT( vis[i] );
    float o = SAMPLE2FLOAT( obs[i] );
    sample_mean     += o*w;
    sum_weights     += w;
    //sum_sqr_weights += w*w;
  }

  //divide the weighted mean
  if (sum_weights > 0.0f)
    sample_mean /= sum_weights;
  else
    return (float2)(0.0f);

  //sum square differences for unbiased variance estimation
  float sum_sqr_diffs = 0.0f;
  float sum_sqr_weights = 0.0f;
  for (uint i=0; i<numSamples; ++i) {
    float w = GAUSS_WEIGHT2FLOAT( vis[i] ) ;
    w = w/ sum_weights;
    float o = SAMPLE2FLOAT( obs[i] );
    sum_sqr_diffs =sum_sqr_diffs+ w * (o-sample_mean)*(o-sample_mean);
    sum_sqr_weights += w*w;
  }
  //calculate unbiased weighted variance
  float sample_var = (sum_sqr_diffs / (1.0f - sum_sqr_weights) );

  return (float2) (sample_mean, sample_var);
}

//sort an MOG3 in place
void sort_mog3(float8* mog3)
{
  //store some vars
  float mu0 = mog3->s0, sigma0 = mog3->s1, w0 = mog3->s2,
        mu1 = mog3->s3, sigma1 = mog3->s4, w1 = mog3->s5,
        mu2 = mog3->s6, sigma2 = mog3->s7;
  float w2 = 1.0f - w0 - w1;
  short Nobs0, Nobs1, Nobs2;

  //sort mix3 in statistics library function
  sort_mix_3( &mu0, &sigma0, &w0, &Nobs0,
              &mu1, &sigma1, &w1, &Nobs1,
              &mu2, &sigma2, &w2, &Nobs2);

  //replace vars
  mog3->s0 = mu0, mog3->s1 = sigma0, mog3->s2 = w0,
  mog3->s3 = mu1, mog3->s4 = sigma1, mog3->s5 = w1,
  mog3->s6 = mu2, mog3->s7 = sigma2;
}

//EM Constants-----------------------------
#define SQRT1_2      .70710678118654752440



float8 weighted_gaussian(   SAMPLE_TYPE*    obs,
                            EM_WEIGHT_TYPE*  vis,
                            const uint  numSamples,
                            const float min_sigma)
{
  //initialize to pretty flat distribution
  float8 mog3 = 0;

  float2 gauss_param = weighted_mean_var(obs, (GAUSS_WEIGHT_TYPE*)vis, numSamples);

  mog3.s0 = gauss_param.x;
  mog3.s1 = max(gauss_param.y, min_sigma);
  mog3.s2 = 1.0;

  sort_mog3(&mog3);
  return mog3;
}

#ifdef MEM_EFFICIENT_EM
//---------------------------------------------------------------
//: Weighted EM Function
// Takes in an array of samples (obs) and an array of weights (vis)
//---------------------------------------------------------------
float8 weighted_mog3_em(SAMPLE_TYPE*    obs,     //samples from MOG3 distribution
                        EM_WEIGHT_TYPE* vis,     //visibility of samples (weights)
                        int             numSamples,
                        const float     min_sigma,
                        const float     weights_normalizer,
                        __global ushort4 *  num_iter)
{

  float log_likelihood = 0;

  //EM Defines
  constexpr uint  nmodes = 3;
  const float big_sigma = (float) SQRT1_2;      // maximum possible std. dev for set of samples drawn from [0 1]

  //init with old mog
  float8 mog3;


  mog3 =  (float8) (  0.25, .06f, 1.0f/nmodes,  //mu0, sigma0, w0
                      0.5,  .06f, 1.0f/nmodes,  //mu1, sigma1, w1
                      0.75, .06f );             //mu2, sigma2


  // just make the sample the mean and the mixture a single mode distribution
  if (numSamples == 1) {
    float obs0 = clamp( SAMPLE2FLOAT(obs[0]), 0.0f, 1.0f );
    mog3.s0 = obs0;                       //mu0
    mog3.s1 = big_sigma;                  //sigma0
    mog3.s2 = 1.0f;                       //w0
    mog3.s3 = obs0;                       //mu1
    mog3.s4 = big_sigma;                  //sigma1
    mog3.s5 = 0.0f;                       //w1
    mog3.s6 = obs0;                       //mu2
    mog3.s7 = big_sigma;                  //sigma2
    return mog3;
  }


  // run EM algorithm to maximize expected probability of observations
  constexpr short max_iterations = 250;
  const float TOL = 5e-4f;
  float old_likelihood = 0.0;


  unsigned short i=0;
  for (; i<max_iterations; ++i) {

    //---------------------------------------------------
    // EXPECTATION
    //---------------------------------------------------
    float total_weight_sum = 0.0f;
    float mode_weight_sum0 = 0;
    float mode_weight_sum1 = 0;
    float curr_likelihood = 0.0;

    float running_mean_mode0, running_mean_mode1, running_mean_mode2;
    float running_mean_mode0_old, running_mean_mode1_old, running_mean_mode2_old;
    float running_std_mode0, running_std_mode1, running_std_mode2;
    float running_weights_mode0, running_weights_mode1, running_weights_mode2;


    for (uint n=0; n<numSamples; ++n)
    {
      //get observation
      float obsN = SAMPLE2FLOAT(obs[n]);

      //compute probability that nth data point was produced by mth mode
      float new_mode0_prob =  gauss_prob_density(obsN, mog3.s0, mog3.s1) * mog3.s2;
      float new_mode1_prob =  gauss_prob_density(obsN, mog3.s3, mog3.s4) * mog3.s5;
      float new_mode2_prob =  gauss_prob_density(obsN, mog3.s6, mog3.s7) * (1.0f-mog3.s2-mog3.s5);
      float total_prob = new_mode0_prob + new_mode1_prob + new_mode2_prob;

      //get weight
      float visN = EM_WEIGHT2FLOAT(vis[n]) / weights_normalizer;


      //compute weighted expectations
      if (total_prob > 1e-07)
      {

        //compute likelihood of data
        curr_likelihood += log(total_prob) * visN;

        float weight_mode0 = new_mode0_prob * (visN / total_prob);
        float weight_mode1 = new_mode1_prob * (visN / total_prob);
        float weight_mode2 = new_mode2_prob * (visN / total_prob);

        mode_weight_sum0 += weight_mode0;
        mode_weight_sum1 += weight_mode1;

        total_weight_sum += weight_mode0 + weight_mode1 + weight_mode2;



        //update running means/stds/weights
        if(n==0)
        {
          running_mean_mode0 = obsN; running_mean_mode1 = obsN; running_mean_mode2 = obsN;
          running_std_mode0 = 0; running_std_mode1 = 0; running_std_mode2 = 0;
          running_weights_mode0 = weight_mode0; running_weights_mode1 = weight_mode1; running_weights_mode2 = weight_mode2;
        }
        else
        {
            if(weight_mode0 > 0)
            {
                running_weights_mode0 += weight_mode0;
                running_mean_mode0_old = running_mean_mode0;
                running_mean_mode0 = running_mean_mode0_old + (obsN - running_mean_mode0_old) * (weight_mode0 / running_weights_mode0);
                running_std_mode0 += weight_mode0 * (obsN - running_mean_mode0_old) * (obsN - running_mean_mode0);
            }
            if(weight_mode1 > 0)
            {
                running_weights_mode1 += weight_mode1;
                running_mean_mode1_old = running_mean_mode1;
                running_mean_mode1 = running_mean_mode1_old + (obsN - running_mean_mode1_old) * (weight_mode1 / running_weights_mode1);
                running_std_mode1 += weight_mode1 * (obsN - running_mean_mode1_old) * (obsN - running_mean_mode1);
            }
            if(weight_mode2 > 0)
            {
              running_weights_mode2 += weight_mode2;
              running_mean_mode2_old = running_mean_mode2;
              running_mean_mode2 = running_mean_mode2_old + (obsN - running_mean_mode2_old) * (weight_mode2 / running_weights_mode2);
              running_std_mode2 += weight_mode2 * (obsN - running_mean_mode2_old) * (obsN - running_mean_mode2);
            }
        }
      }
    }
    //E step done.


    // check for convergence
    if (fabs(old_likelihood - curr_likelihood) <  TOL)
      break;
    old_likelihood = curr_likelihood;


    //M step

    // computed the weighted means and variances for each mode based on the probabilities
    mog3.s0 = running_mean_mode0;
    mog3.s1 = (running_weights_mode0 <= 0 ) ? min_sigma : clamp(sqrt( running_std_mode0 / running_weights_mode0), min_sigma, big_sigma);

    mog3.s3 = running_mean_mode1;
    mog3.s4 = (running_weights_mode1 <= 0 ) ? min_sigma : clamp(sqrt( running_std_mode1 / running_weights_mode1), min_sigma, big_sigma);

    mog3.s6 = running_mean_mode2;
    mog3.s7 = (running_weights_mode2 <= 0 ) ? min_sigma : clamp(sqrt( running_std_mode2 / running_weights_mode2), min_sigma, big_sigma);


    // update mode weights
    if (total_weight_sum > 1e-07) {
      mog3.s2 = mode_weight_sum0 / total_weight_sum;
      mog3.s5 = mode_weight_sum1 / total_weight_sum;
    }
    //M step done.

  }

  (*num_iter).x = i;


  //post EM calculation

  sort_mog3(&mog3);
  mog3 = clamp(mog3, 0.0f, 1.0f);
  return mog3;

}
#endif
