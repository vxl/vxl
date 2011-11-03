//-------------------------------------------------------
// Weighted EM Function and Weighted Gaussian functions
// Takes in an array of samples (obs) and an array of weights (vis)
// runs EM for a mixture of 3 gaussians
// Define:
//  - SAMPLE_TYPE 
//  - WEIGHT_TYPE
//


//--------------------------------------------------
// Weighted Mean/Variance
//define sample type and conversion to float
#define SAMPLE_TYPE global uchar
#define SAMPLE2FLOAT(samp)  (convert_float(samp)/255.0f)

//define EM weight type (may be different than GAUSS weight type
#define EM_WEIGHT_TYPE global uchar
#define EM_WEIGHT2FLOAT(samp)  (convert_float(samp)/255.0f)

//define wieght and conversion to float
#define GAUSS_WEIGHT_TYPE float
#define GAUSS_WEIGHT2FLOAT(samp)  samp; 


//weighted mean/variance calculation from global obs and weights
float2 weighted_mean_var(SAMPLE_TYPE* obs, GAUSS_WEIGHT_TYPE* vis, int numSamples)
{
  if(numSamples == 0)
    return (float2) (0.0f); 
  
  if(numSamples == 1)
    return (float2) ( SAMPLE2FLOAT(obs[0]), .1f ); 
    
  float sample_mean     = 0.0f;
  float sum_weights     = 0.0f; 
  float sum_sqr_weights = 0.0f; 
  for(uint i=0; i<numSamples; ++i) {
    float w = GAUSS_WEIGHT2FLOAT( vis[i] );
    float o = SAMPLE2FLOAT( obs[i] ); 
    sample_mean     += o*w; 
    sum_weights     += w; 
    sum_sqr_weights += w*w; 
  }

  //divide the weighted mean
  if(sum_weights > 0.0f)
    sample_mean /= sum_weights; 
  else
    return (float2) (0.0f); 
  
  //sum square differences for unbiased variance estimation
  float sum_sqr_diffs = 0.0f; 
  for(uint i=0; i<numSamples; ++i) {
    float w = GAUSS_WEIGHT2FLOAT( vis[i] ); 
    float o = SAMPLE2FLOAT( obs[i] ); 
    sum_sqr_diffs += w * (o-sample_mean)*(o-sample_mean); 
  }
  
  //calculate unbiased weighted variance
  float sample_var = (sum_weights / (sum_weights*sum_weights - sum_sqr_weights) ) * sum_sqr_diffs; 
  return (float2) (sample_mean, sample_var); 
}


//EM Constants-----------------------------
#define SQRT1_2      .70710678118654752440
#define MAX_SAMPLES  200

//---------------------------------------------------------------
// Weighted EM Function 
// Takes in an array of samples (obs) and an array of weights (vis)
//---------------------------------------------------------------
float8 weighted_mog3_em(SAMPLE_TYPE*    obs,     //samples from MOG3 distribution
                        EM_WEIGHT_TYPE* vis,     //visibility of samples (weights)
                        int             numSamples,
                        float           min_sigma )
{
  //EM Defines
  const uint  nmodes    = 3;
  const float min_var   = min_sigma*min_sigma;
  const float big_sigma = (float) SQRT1_2;      // maximum possible std. dev for set of samples drawn from [0 1]
  const float big_var   = big_sigma * big_sigma;
  
  //initialize to pretty flat distribution 
  float8 mog3 = (float8) (  .5f/nmodes, .3f, 1.0f/nmodes,  //mu0, sigma0, w0
                           1.5f/nmodes, .3f, 1.0f/nmodes,  //mu1, sigma1, w1
                           2.5f/nmodes, .3f );             //mu2, sigma2
  float* mog3Arr = (float*) &mog3; 

  //degenerate cases
  //if numsamples is 0, then return flat distro
  if(numSamples == 0)
    return mog3; 
  
  // just make the sample the mean and the mixture a single mode distribution
  if(numSamples == 1) {
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

  //if we have more than MAX_SAMPLES, limit it
  numSamples = min(numSamples, MAX_SAMPLES);      
  float obs_weights[MAX_SAMPLES] = {0}; 
  float mode_weight_sum[3];
   
  //2d array of responsibiliites - mode_probs[mode][n] = prob( mode given example n)
  float mode0_probs[MAX_SAMPLES] = {0}; 
  float mode1_probs[MAX_SAMPLES] = {0};
  float mode2_probs[MAX_SAMPLES] = {0};
  float* mode_probs[3]; 
  mode_probs[0] = mode0_probs; 
  mode_probs[1] = mode1_probs; 
  mode_probs[2] = mode2_probs; 

  // run EM algorithm to maximize expected probability of observations
  const unsigned int max_iterations              = 50;
  const float        max_converged_weight_change = 1e-3f;
  for(uint i=0; i<max_iterations; ++i) {
    float max_weight_change = 0.0f; 
    
    //---------------------------------------------------
    // EXPECTATION
    //---------------------------------------------------
    for(uint n=0; n<numSamples; ++n) {
      
      float obsN = SAMPLE2FLOAT(obs[n]); 
      float visN = EM_WEIGHT2FLOAT(vis[n]); 
      
      //compute probability that nth data point was produced by mth mode, weighted by visibility
      float new_mode0_prob = visN * gauss_prob_density(obsN, mog3.s0, mog3.s1) * mog3.s2; 
      float new_mode1_prob = visN * gauss_prob_density(obsN, mog3.s3, mog3.s4) * mog3.s5; 
      float new_mode2_prob = visN * gauss_prob_density(obsN, mog3.s6, mog3.s7) * (1.0f-mog3.s2-mog3.s5); 
      float total_prob = new_mode0_prob + new_mode1_prob + new_mode2_prob; 
      
      //compute probability that observation came from an occluding cell
      if(total_prob > 1e-6f) {
        new_mode0_prob /= total_prob; 
        new_mode1_prob /= total_prob; 
        new_mode2_prob /= total_prob; 
        max_weight_change = fmax( fabs(new_mode0_prob - mode0_probs[n]), 
                                      fmax( fabs(new_mode1_prob - mode1_probs[n]),
                                            fabs(new_mode2_prob - mode2_probs[n]) ) ); 

        //reassign mode responsibilities
        mode0_probs[n] = new_mode0_prob; 
        mode1_probs[n] = new_mode1_prob; 
        mode2_probs[n] = new_mode2_prob; 
      }
    }
    
    // check for convergence
    if (max_weight_change < max_converged_weight_change) 
      break;
    
    //---------------------------------------------
    // MAXIMIZATION
    //---------------------------------------------
    // computed the weighted means and variances for each mode based on the probabilities
    float total_weight_sum = 0.0f;

    // update the mode parameters
    float mode_weight_sum[3]; 
    for(uint m=0; m<nmodes; ++m) {
 
      //compute weight sum for this mode
      mode_weight_sum[m] = 0.0f; 
      for(uint n=0; n<numSamples; ++n) {
        obs_weights[n]      = mode_probs[m][n]; 
        mode_weight_sum[m] += obs_weights[n]; 
      }
      total_weight_sum += mode_weight_sum[m];
      
      //train this gaussian
      float  mode_mean = 0.5f;
      float  mode_var  = 1.0f;
      float2 mode_gauss = weighted_mean_var(obs, (GAUSS_WEIGHT_TYPE*) obs_weights, numSamples); 
      mode_mean = mode_gauss.x; 
      mode_var  = clamp(mode_gauss.y, min_var, big_var);

      // update mode parameters
      mog3Arr[ m*3 ]   = mode_mean; 
      mog3Arr[ m*3+1 ] = mode_var;  
    }
    
    // update mode weights
    if (total_weight_sum > 1e-6) {
      float mode0_weight = mode_weight_sum[0] / total_weight_sum; 
      float mode1_weight = mode_weight_sum[1] / total_weight_sum; 
      float mode2_weight = mode_weight_sum[2] / total_weight_sum; 
      mog3.s2 = mode0_weight; 
      mog3.s5 = mode1_weight; 
    }
  }
  
  //post EM calculation
  // unbias variance based on number of observations for each mode... (might be taken care of)
  // sort the modes based on weight
  //model.sort();
  mog3 = clamp(mog3, 0.0f, 1.0f); 
  return mog3;
}
