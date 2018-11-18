#include <cmath>
#include <iostream>
#include <algorithm>
#include "bsta_mog3_grey.h"
//
#define TMATCH 2.5f

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_sf1.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/algo/bsta_fit_gaussian.h>

static float bsta_clamp(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }

bool sort_components (vnl_vector_fixed<float,3> i,vnl_vector_fixed<float,3> j)
{
  float ratio1=i[2]/i[1];
  float ratio2=j[2]/j[1];
  return ratio1>ratio2;
}

float  bsta_mog3_grey::expected_color( vnl_vector_fixed<unsigned char, 8> mog3)
{
  float w2=0.0f;
  if (mog3[2]>0 && mog3[5]>0)
    w2=(float)(255-mog3[2]-mog3[5]);

  float exp_intensity=(float)mog3[0]*(float)mog3[2]+
                      (float)mog3[3]*(float)mog3[5]+
                      (float)mog3[6]*w2;

  exp_intensity/=(255.0f*255.0f);

  return exp_intensity;
}

float  bsta_mog3_grey::gauss_prob_density(float x, float mu, float sigma)
{
  return 0.398942280f*std::exp(-0.5f*(x - mu)*(x - mu)/(sigma*sigma))/sigma;
}

float  bsta_mog3_grey::prob_density(const vnl_vector_fixed<unsigned char, 8> & mog3, float x)
{
  float sum = 0.0f;
  float mu0=((float)mog3[0]/255.0f);float sigma0=((float)mog3[1]/255.0f);float w0=((float)mog3[2]/255.0f);
  float mu1=((float)mog3[3]/255.0f);float sigma1=((float)mog3[4]/255.0f);float w1=((float)mog3[5]/255.0f);
  float mu2=((float)mog3[6]/255.0f);float sigma2=((float)mog3[7]/255.0f);float w2=0.0f;


  if (w0>0.0f && w1>0.0f)
    w2=1-w0-w1;

  if (w0>0.0f && sigma0 >0.0f)
  {
    sum += w0*bsta_mog3_grey::gauss_prob_density(x, mu0, sigma0);
    if (w1>0.0f && sigma1 >0.0f)
    {
      sum += w1*gauss_prob_density(x, mu1, sigma1);
      if (w2>0.0f && sigma2 >0.0f)
      {
        sum += w2*gauss_prob_density(x, mu2, sigma2);
      }
    }
  }
  else
    sum=1.0f;

  return sum;
}

void bsta_mog3_grey::update_gauss(float & x, float & rho, float & mu, float &  sigma,float min_sigma)
{
  float var = sigma*sigma;
  float diff = x-mu;
  var = (1.0f-rho)*(var +rho*diff*diff);
  mu += rho*diff;
  sigma = std::sqrt(var);
  sigma = sigma < min_sigma ? min_sigma: sigma;
}

void bsta_mog3_grey::sort_mix_3(float & mu0, float& sigma0, float& w0, float& Nobs0,
                                           float & mu1, float& sigma1, float& w1, float& Nobs1,
                                           float & mu2, float& sigma2, float& w2, float& Nobs2)
{
    if (w1>0.0f && sigma1>0.0f )
    { // no need to sort
        float fa = w0/sigma0, fb =w1/sigma1;
        if (w2==0.0f || sigma2==0.0f) {
            if (fa<fb) { //only need to swap a and b
                float tmu0 = mu0, tsig0 = sigma0, tw0 = w0;
                float n0 = Nobs0;
                mu0 = mu1; sigma0 = sigma1; w0 = w1; Nobs0 = Nobs1;
                mu1 = tmu0; sigma1 = tsig0; w1 = tw0; Nobs1 = n0;
                return ;
            }
            else return;

            float fc =w2/sigma2;

            if (fa>=fb&&fb>=fc) // [a b c ] - already sorted
                return;
            if (fa<fb&&fb<fc) // [c b a] - swap a and c
            {
                float tmu0 = mu0, tsig0 = sigma0, tw0 = w0;
                float n0 = Nobs0;
                mu0 = mu2; sigma0 = sigma2; w0 = w2; Nobs0 = Nobs2;
                mu2 = tmu0; sigma2 = tsig0; w2 = tw0; Nobs2 = n0;
                return ;
            }
            if (fa<fb&&fb>=fc)
            {
                if (fa>=fc) // [b a c] - c stays where it is and a b swap
                {
                    float tmu0 = mu0, tsig0 = sigma0, tw0 = w0;
                    float n0 = Nobs0;
                    mu0 = mu1; sigma0 = sigma1; w0 = w1; Nobs0 = Nobs1;
                    mu1 = tmu0; sigma1 = tsig0;   w1 = tw0; Nobs1 = n0;
                    return;
                }
                else {
                    // [b c a] - two swaps
                    float tmu0 = mu0, tsig0 = sigma0, tw0 = w0;
                    float n0 = Nobs0;
                    mu0 = mu1; sigma0 = sigma1; w0 = w1;  Nobs0 = Nobs1;
                    mu1 = mu2; sigma1 = sigma2; w1 = w2;  Nobs1 = Nobs2;
                    mu2 = tmu0;sigma2 = tsig0;  w2 = tw0; Nobs2 = n0;
                    return;
                }
            }
            if (fa>=fb&&fb<fc)
            {
                if (fa>=fc) // [a c b] - b and c swap
                {
                    float tmu1 = mu1, tsig1 = sigma1, tw1 = w1;
                    float n1 = Nobs1;
                    mu1 = mu2;  sigma1 = sigma2; w1 = w2;  Nobs1 = Nobs2;
                    mu2 = tmu1; sigma2 = tsig1;  w2 = tw1; Nobs2 = n1;
                    return;
                }
                else {
                    // [c a b] - two swaps
                    float tmu0 = mu0, tsig0 = sigma0, tw0 = w0;
                    float n0 = Nobs0;
                    mu0 = mu2; sigma0 = sigma2; w0 = w2;  Nobs0 = Nobs2;
                    mu2 = mu1; sigma2 = sigma1; w2 = w1;  Nobs2 = Nobs1;
                    mu1 = tmu0;sigma1 = tsig0;  w1 = tw0; Nobs1 = n0;
                    return;
                }
            }
        }
    }
}

void bsta_mog3_grey::insert_gauss_3(float x, float init_weight, float init_sigma, int & match,
                                               float& mu0, float& sigma0, float& w0, float& Nobs0,
                                               float& mu1, float& sigma1, float& w1, float& Nobs1,
                                               float& mu2, float& sigma2, float& w2, float& Nobs2)
{
  if (w1>0.0f && sigma1>0.0f)  // replace the third component
  {
    float adjust = w0 + w1;
    adjust = (1.0f - init_weight)/adjust;
    w0 = w0*adjust;
    w1 = w1*adjust;
    mu2 = x;
    sigma2 = init_sigma;
    w2 = init_weight;
    Nobs2 = 1;
    match = 2;
    return;
  }
  else if (w0>0.0f) { // replace the second component
    w0 = (1.0f-init_weight);
    mu1 = x;
    sigma1 = init_sigma;
    w1 = init_weight;
    Nobs1 = 1;
    match = 1;
    return;
  }
  else { // replace the first component
    w0 = 1.0f; // init_weight in C++
    mu0 = x;
    sigma0 = init_sigma;
    Nobs0 = 1;
    match = 0;
  }
}

void
bsta_mog3_grey::update_gauss_mixture_3(vnl_vector_fixed<unsigned char, 8> & mog3,
                                       vnl_vector_fixed<float, 4> & nobs,
                                       float x, float w, float init_sigma,float min_sigma)
{
  float mu0=((float)mog3[0]/255.0f);float sigma0=((float)mog3[1]/255.0f);float w0=((float)mog3[2]/255.0f);
  float mu1=((float)mog3[3]/255.0f);float sigma1=((float)mog3[4]/255.0f);float w1=((float)mog3[5]/255.0f);
  float mu2=((float)mog3[6]/255.0f);float sigma2=((float)mog3[7]/255.0f);float w2=0.0f; //1-w0-w1;

  if (w0>0.0f && w1>0.0f)
      w2=1-w0-w1;
  if (w>0.0f)
  {
    int match = -1;
    nobs[3] += w;
    float alpha = w/nobs[3], tsq=TMATCH*TMATCH;
    float weight = 0.0f, rho = 0.0f;

    // test for a match of component 0
    if (w0>0.0f && sigma0>0.0f) {
      weight = (1.0f-alpha)*(w0);
      if (match<0 && ((x-mu0)*(x-mu0)/(sigma0*sigma0)) < tsq) {
        weight += alpha;
        nobs[0]++;
        rho = (1.0f-alpha)/nobs[0] + alpha;
        bsta_mog3_grey::update_gauss(x, rho, mu0, sigma0, min_sigma);
        match = 0;
      }
      w0 = weight;
    }
    // test for a match of component 1
    if (w1>0.0f && sigma1>0.0f) {
      weight = (1.0f-alpha)*(w1);
      if (match<0 && (x-mu1)*(x-mu1) < sigma1*sigma1*tsq) {
        weight += alpha;
        nobs[1]++;
        rho = (1.0f-alpha)/nobs[1] + alpha;
        bsta_mog3_grey::update_gauss(x, rho, mu1, sigma1, min_sigma);
        match = 1;
      }
      w1 = weight;
    }
    // test for a match of component 2
    if (w2>0.0f &&  sigma2>0.0f) {
      weight = (1.0f-alpha)*w2;
      if (match<0 && (x-mu2)*(x-mu2) < sigma2*sigma2*tsq) {
        weight += alpha;
        nobs[2]++;
        rho = (1.0f-alpha)/nobs[2] + alpha;
        bsta_mog3_grey::update_gauss(x, rho, mu2, sigma2, min_sigma);
        match = 2;
      }
      w2 = weight;
    }
    // If there were no matches then insert a new component
    if (match<0)
      bsta_mog3_grey::insert_gauss_3(x, alpha, init_sigma, match,
                                                mu0, sigma0, w0, nobs[0],
                                                mu1, sigma1, w1, nobs[1],
                                                mu2, sigma2, w2, nobs[2]);

    bsta_mog3_grey::sort_mix_3(mu0, sigma0, w0, nobs[0],
                                          mu1, sigma1, w1, nobs[1],
                                          mu2, sigma2, w2, nobs[2]);
  }

  mog3[0]=(unsigned char)std::floor(bsta_clamp(mu0,0,1)*255.0f);
  mog3[1]=(unsigned char)std::floor(bsta_clamp(sigma0,0,1)*255.0f);
  mog3[2]=(unsigned char)std::floor(bsta_clamp(w0,0,1)*255.0f);
  mog3[3]=(unsigned char)std::floor(bsta_clamp(mu1,0,1)*255.0f);
  mog3[4]=(unsigned char)std::floor(bsta_clamp(sigma1,0,1)*255.0f);
  mog3[5]=(unsigned char)std::floor(bsta_clamp(w1,0,1)*255.0f);
  mog3[6]=(unsigned char)std::floor(bsta_clamp(mu2,0,1)*255.0f);
  mog3[7]=(unsigned char)std::floor(bsta_clamp(sigma2,0,1)*255.0f);
}

void bsta_mog3_grey::force_mog3_sigmas_to_value(vnl_vector_fixed<unsigned char, 8> & mog3,float sigma){
  mog3[1]=(unsigned char)std::floor(bsta_clamp(sigma,0,1)*255.0f);
  mog3[4]=(unsigned char)std::floor(bsta_clamp(sigma,0,1)*255.0f);
  mog3[7]=(unsigned char)std::floor(bsta_clamp(sigma,0,1)*255.0f);
}

bool bsta_mog3_grey::merge_gauss(float mu1,float var1, float w1,
                                            float mu2,float var2, float w2,
                                            vnl_vector_fixed<float, 3> & new_component)
{
  float denominator=w1+w2;
  if (denominator<=0.0f) return false;
  //mean
  new_component[0]=(w1*mu1+w2*mu2)/denominator;
  //variance
  new_component[1]=w1*var1/denominator+w2*var2/denominator+(mu1-mu2)*(mu1-mu2)*(w1*w2/denominator/denominator);
  //weight
  new_component[2]=w1+w2;
  return true;
}

void  bsta_mog3_grey::merge_mixtures(vnl_vector_fixed<unsigned char, 8> & mog3_1, float w1,
                                                vnl_vector_fixed<unsigned char, 8> & mog3_2, float w2,
                                                vnl_vector_fixed<unsigned char, 8> & mog3_3)
{
  float w3=w1+w2;if (w3<=0.0f)return;

  float mog3_float_1[9]={0.0f};
  float mog3_float_2[9]={0.0f};
  for (unsigned i=0;i<8;i++)
    mog3_float_1[i]=(float)mog3_1[i]/255.0f;
  if (mog3_float_1[2]>0.0f && mog3_float_1[5]>0.0f)
    mog3_float_1[8]=std::max(0.0f,1-mog3_float_1[2]-mog3_float_1[5]);

  for (unsigned i=0;i<8;i++)
    mog3_float_2[i]=(float)mog3_2[i]/255.0f;
  if (mog3_float_2[2]>0.0f && mog3_float_2[5]>0.0f)
    mog3_float_2[8]=std::max(0.0f,1-mog3_float_2[2]-mog3_float_2[5]);

  if (mog3_float_1[2]<=0.0f)
  {
    mog3_3=mog3_2;return;
  }
  else if (mog3_float_2[2]<=0.0f)
  {
    mog3_3=mog3_1;return;
  }
  std::vector<vnl_vector_fixed<float,3> > merged;

  // Merge all the compoentns.
  for (unsigned i=0;i<3;i++)
  {
    float w1c=w1*mog3_float_1[i*3+2];
    if (w1c<=0.0f || mog3_float_1[i*3+1] <=0.0f) continue;
    for (unsigned j=0;j<3;j++)
    {
      vnl_vector_fixed<float,3> new_component(0.0f);
      float w2c=w2*mog3_float_2[j*3+2];
      if (w2c<=0.0f || mog3_float_2[j*3+1] <=0.0f ) continue;
      if (merge_gauss(mog3_float_1[i*3+0],mog3_float_1[i*3+1],w1c,mog3_float_2[j*3+0],mog3_float_2[j*3+1],w2c,new_component))
        merged.push_back(new_component);
    }
  }
  // reduce the components to nine by merging the components with smallest weight/variance ratios.
  while (merged.size()>3)
  {
    std::sort(merged.begin(),merged.end(),sort_components);
    int compindex=(int)merged.size()-1;
    vnl_vector_fixed<float,3> new_component(0.0f);
    if (merge_gauss(merged[compindex][0],merged[compindex][1],merged[compindex][2],
                    merged[compindex-1][0],merged[compindex-1][1],merged[compindex-1][2],
                    new_component))
    {
      merged.erase(merged.begin()+compindex);
      merged.erase(merged.begin()+compindex-1);
      merged.push_back(new_component);
    }
  }

  // renormalize the weights so that they sum to 1.
  float sum=0.0f;
  for (unsigned i=0;i<3 && i<merged.size();i++)
    sum+=merged[i][2];
  int count=-1;
  for (unsigned i=0;i<3 && i<merged.size();i++)
  {
    mog3_3[++count]=(unsigned char)std::floor(merged[i][0]*255.0f);
    mog3_3[++count]=(unsigned char)std::floor(merged[i][1]*255.0f);
    if (i<2)
      mog3_3[++count]=(unsigned char)std::floor(merged[i][2]/sum*255.0f);
  }
}

// Most of the following code is copied over from boxm_mog_grey::compute_appearance

void bsta_mog3_grey::compute_app_model(vnl_vector_fixed<unsigned char, 8> & apm,
                                                  std::vector<float> const& obs,
                                                  std::vector<float> const& obs_weights,
                                                  const bsta_sigma_normalizer_sptr& n_table,
                                                  float min_sigma)
{
  //compute_gauss_mixture_3(apm,obs,obs_weights,n_table,min_sigma);
  std::vector<float> pre(obs.size(),0.0f);
  compute_app_model(apm,obs,pre,obs_weights,n_table,min_sigma);
}

void bsta_mog3_grey::compute_app_model(vnl_vector_fixed<unsigned char, 8> & mog3,
                                                  std::vector<float> const& obs,
                                                  std::vector<float> const& pre,
                                                  std::vector<float> const& vis,
                                                  const bsta_sigma_normalizer_sptr& n_table,
                                                  float min_sigma)
{
  constexpr unsigned int nmodes = 3;
  const float min_var = min_sigma*min_sigma;
  const auto big_sigma = (float)vnl_math::sqrt1_2; // maximum possible std. dev for set of samples drawn from [0 1]
  const float big_var = big_sigma * big_sigma;

  unsigned int nobs = (int)obs.size();
  if (nobs == 0) {
    // nothing to do.
    return;
  }
  if (nobs == 1) {
    // just make the sample the mean and the mixture a single mode distribution
    mog3[0]=(unsigned char)std::floor(bsta_clamp(obs[0],0,1)*255.0f);
    mog3[1]=(unsigned char)std::floor(bsta_clamp(big_sigma,0,1)*255.0f);
    mog3[2]=(unsigned char)std::floor(bsta_clamp(1.0f,0,1)*255.0f);
    mog3[3]=(unsigned char)std::floor(bsta_clamp(obs[0],0,1)*255.0f);
    mog3[4]=(unsigned char)std::floor(bsta_clamp(big_sigma,0,1)*255.0f);
    mog3[5]=(unsigned char)std::floor(bsta_clamp(0.0f,0,1)*255.0f);
    mog3[6]=(unsigned char)std::floor(bsta_clamp(obs[0],0,1)*255.0f);
    mog3[7]=(unsigned char)std::floor(bsta_clamp(big_sigma,0,1)*255.0f);
    return;
  }

  bsta_num_obs<bsta_mixture_fixed<bsta_num_obs<bsta_gauss_sf1>, nmodes> > model;

  // we always initialize parameters at the beginning of each iteration
  for (unsigned int m=0; m<nmodes; ++m) {
    float mean = (float(m) + 0.5f) / float(nmodes);
    float sigma = 0.3f;
    float mode_weight = 1.0f / float(nmodes);
    bsta_gauss_sf1 mode(mean, sigma*sigma);
    model.insert(bsta_num_obs<bsta_gauss_sf1>(mode), mode_weight);
  }

  std::vector<std::vector<float> > mode_probs(nobs);
  for (unsigned int n=0; n<nobs; ++n) {
    for (unsigned int m=0; m<nmodes; ++m) {
      mode_probs[n].push_back(0.0f);
    }
  }
  std::vector<float> mode_weight_sum(nmodes,0.0f);

  // run EM algorithm to maximize expected probability of observations
  constexpr unsigned int max_its = 50;
  const float max_converged_weight_change = 1e-3f;

  for (unsigned int i=0; i<max_its; ++i) {
    float max_weight_change = 0.0f;
    // EXPECTATION
    for (unsigned int n=0; n<nobs; ++n) {
      // for each observation, assign probabilities to each mode of appearance model (and to a "previous cell")
      float total_prob = 0.0f;
      std::vector<float> new_mode_probs(nmodes);
      for (unsigned int m=0; m<nmodes; ++m) {
        // compute probability that nth data point was produced by mth mode
        const float new_mode_prob = vis[n] * model.distribution(m).prob_density(obs[n]) * model.weight(m);
        new_mode_probs[m] = new_mode_prob;
        total_prob += new_mode_prob;
      }
      // compute the probability the observation came from an occluding cell
      const float prev_prob = pre[n];
      total_prob += prev_prob;
      if (total_prob > 1e-6) {
        for (unsigned int m=0; m<nmodes; ++m) {
          new_mode_probs[m] /= total_prob;
          const float weight_change = std::fabs(new_mode_probs[m] - mode_probs[n][m]);
          if (weight_change > max_weight_change) {
            max_weight_change = weight_change;
          }
          mode_probs[n][m] = new_mode_probs[m];
        }
      }
    }
    // check for convergence
    if (max_weight_change < max_converged_weight_change) {
      break;
    }
    // MAXIMIZATION
    // computed the weighted means and variances for each mode based on the probabilities
    float total_weight_sum = 0.0f;

    // update the mode parameters
    for (unsigned int m=0; m<nmodes; ++m) {
      mode_weight_sum[m] = 0.0f;
      std::vector<float> obs_weights(nobs);
      for (unsigned int n=0; n<nobs; ++n) {
        obs_weights[n] = mode_probs[n][m];
        mode_weight_sum[m] += obs_weights[n];
      }
      total_weight_sum += mode_weight_sum[m];
      float mode_mean(0.5f);
      float mode_var(1.0f);
      bsta_gauss_sf1 single_gauss(mode_mean,mode_var);
      bsta_fit_gaussian(obs, obs_weights, single_gauss);
      mode_mean = single_gauss.mean();
      mode_var = single_gauss.var();

      // unbias variance based on number of observations
      //float unbias_factor = sigma_norm_factor(mode_weight_sum[m]);
      // mode_var *= (unbias_factor*unbias_factor);

      // make sure variance does not get too big
      if (!(mode_var < big_var)) {
        mode_var = big_var;
      }
      // or too small
      if (!(mode_var > min_var)) {
        mode_var = min_var;
      }

      // update mode parameters
      model.distribution(m).set_mean(mode_mean);
      model.distribution(m).set_var(mode_var);
    }
    // update mode probabilities
    if (total_weight_sum > 1e-6) {
      for (unsigned int m=0; m<nmodes; ++m) {
        const float mode_weight = mode_weight_sum[m] / total_weight_sum;
        // update mode weight
        model.set_weight(m, mode_weight);
      }
    }
  }

  // unbias variance based on number of observations
  for (unsigned int m=0; m<nmodes; ++m) {
    float unbias_factor = n_table->normalization_factor(mode_weight_sum[m]);

    float mode_var = model.distribution(m).var();
    mode_var *= (unbias_factor*unbias_factor);

    // make sure variance does not get too big
    if (!(mode_var < big_var)) {
      mode_var = big_var;
    }
    // or too small
    if (!(mode_var > min_var)) {
      mode_var = min_var;
    }
    model.distribution(m).set_var(mode_var);
  }

  // sort the modes based on weight
  model.sort();

  mog3[0]=(unsigned char)std::floor(bsta_clamp(model.distribution(0).mean(),0,1)*255.0f);
  mog3[1]=(unsigned char)std::floor(bsta_clamp(std::sqrt(model.distribution(0).var()),0,1)*255.0f);
  mog3[2]=(unsigned char)std::floor(bsta_clamp(model.weight(0),0,1)*255.0f);
  mog3[3]=(unsigned char)std::floor(bsta_clamp(model.distribution(1).mean(),0,1)*255.0f);
  mog3[4]=(unsigned char)std::floor(bsta_clamp(std::sqrt(model.distribution(1).var()),0,1)*255.0f);
  mog3[5]=(unsigned char)std::floor(bsta_clamp(model.weight(1),0,1)*255.0f);
  mog3[6]=(unsigned char)std::floor(bsta_clamp(model.distribution(2).mean(),0,1)*255.0f);
  mog3[7]=(unsigned char)std::floor(bsta_clamp(std::sqrt(model.distribution(2).var()),0,1)*255.0f);

  return;
}
