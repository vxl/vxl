#include "boxm2_mog3_grey_processor.h"
//:
// \file
#define TMATCH 2.5f

#include <boxm2/boxm2_util.h>
#include <vcl_cmath.h> // for std::exp() && std::sqrt()
#include <vcl_algorithm.h>
bool sort_components (vnl_vector_fixed<float,3> i,vnl_vector_fixed<float,3> j) 
{ 

    float ratio1=i[2]/i[1];
    float ratio2=j[2]/j[1];
    return ratio1>ratio2;
}

float  boxm2_mog3_grey_processor::expected_color( vnl_vector_fixed<unsigned char, 8> mog3)
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

float  boxm2_mog3_grey_processor::gauss_prob_density(float x, float mu, float sigma)
{
  return 0.398942280f*vcl_exp(-0.5f*(x - mu)*(x - mu)/(sigma*sigma))/sigma;
}

float  boxm2_mog3_grey_processor::prob_density(const vnl_vector_fixed<unsigned char, 8> & mog3, float x)
{
  float sum = 0.0f;
  float sum_weights=0.0f;
  float mu0=((float)mog3[0]/255.0f);float sigma0=((float)mog3[1]/255.0f);float w0=((float)mog3[2]/255.0f);
  float mu1=((float)mog3[3]/255.0f);float sigma1=((float)mog3[4]/255.0f);float w1=((float)mog3[5]/255.0f);
  float mu2=((float)mog3[6]/255.0f);float sigma2=((float)mog3[7]/255.0f);float w2=0.0f;


  if (w0>0.0f && w1>0.0f)
      w2=1-w0-w1;

  if (w0>0.0f && sigma0 >0.0f)
  {
    sum += w0*boxm2_mog3_grey_processor::gauss_prob_density(x, mu0, sigma0);
    sum_weights+=w0;
    if (w1>0.0f && sigma1 >0.0f)
    {
      sum += w1*gauss_prob_density(x, mu1, sigma1);
      sum_weights+=w1;
      if (w2>0.0f && sigma2 >0.0f)
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

void boxm2_mog3_grey_processor::update_gauss(float & x, float & rho, float & mu, float &  sigma,float min_sigma)
{
  float var = sigma*sigma;
  float diff = x-mu;
  var = (1.0f-rho)*(var +rho*diff*diff);
  mu += rho*diff;
  sigma = vcl_sqrt(var);
  sigma = sigma < min_sigma ? min_sigma: sigma;
}

void boxm2_mog3_grey_processor::sort_mix_3(float & mu0, float& sigma0, float& w0, float& Nobs0,
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

void boxm2_mog3_grey_processor::insert_gauss_3(float x, float init_weight, float init_sigma, int & match,
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
boxm2_mog3_grey_processor::update_gauss_mixture_3(vnl_vector_fixed<unsigned char, 8> & mog3,
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
        boxm2_mog3_grey_processor::update_gauss(x, rho, mu0, sigma0, min_sigma);
        match = 0;
      }
      w0 = weight;
    }
    // test for a match of component 1
    if (w1>0.0f && sigma1>0.0f) {
      weight = (1.0f-alpha)*(w1);
      if (match<0 && ((x-mu1)*(x-mu1)/(sigma1*sigma1)) < tsq) {
        weight += alpha;
        nobs[1]++;
        rho = (1.0f-alpha)/nobs[1] + alpha;
        boxm2_mog3_grey_processor::update_gauss(x, rho, mu1, sigma1, min_sigma);
        match = 1;
      }
      w1 = weight;
    }
    // test for a match of component 2
    if (w2>0.0f &&  sigma2>0.0f) {
      weight = (1.0f-alpha)*w2;
      if (match<0 && ((x-mu2)*(x-mu2)/(sigma2*sigma2)) < tsq) {
        weight += alpha;
        nobs[2]++;
        rho = (1.0f-alpha)/nobs[2] + alpha;
        boxm2_mog3_grey_processor::update_gauss(x, rho, mu2, sigma2, min_sigma);
        match = 2;
      }
      w2 = weight;
    }
    // If there were no matches then insert a new component
    if (match<0)
      boxm2_mog3_grey_processor::insert_gauss_3(x, alpha, init_sigma, match,
                                                mu0, sigma0, w0, nobs[0],
                                                mu1, sigma1, w1, nobs[1],
                                                mu2, sigma2, w2, nobs[2]);

    boxm2_mog3_grey_processor::sort_mix_3(mu0, sigma0, w0, nobs[0],
                                          mu1, sigma1, w1, nobs[1],
                                          mu2, sigma2, w2, nobs[2]);
  }

  mog3[0]=(unsigned char)vcl_floor(boxm2_mog3_grey_processor::clamp(mu0,0,1)*255.0f);
  mog3[1]=(unsigned char)vcl_floor(boxm2_mog3_grey_processor::clamp(sigma0,0,1)*255.0f);
  mog3[2]=(unsigned char)vcl_floor(boxm2_mog3_grey_processor::clamp(w0,0,1)*255.0f);
  mog3[3]=(unsigned char)vcl_floor(boxm2_mog3_grey_processor::clamp(mu1,0,1)*255.0f);
  mog3[4]=(unsigned char)vcl_floor(boxm2_mog3_grey_processor::clamp(sigma1,0,1)*255.0f);
  mog3[5]=(unsigned char)vcl_floor(boxm2_mog3_grey_processor::clamp(w1,0,1)*255.0f);
  mog3[6]=(unsigned char)vcl_floor(boxm2_mog3_grey_processor::clamp(mu2,0,1)*255.0f);
  mog3[7]=(unsigned char)vcl_floor(boxm2_mog3_grey_processor::clamp(sigma2,0,1)*255.0f);
}
bool boxm2_mog3_grey_processor::merge_gauss(float mu1,float var1, float w1,
                                            float mu2,float var2, float w2,
                                            vnl_vector_fixed<float, 3> & new_component)
{

    float denominator=w1+w2;
    if(denominator<=0.0f) return false;
    //mean
    new_component[0]=(w1*mu1+w2*mu2)/denominator;
    //variance
    new_component[1]=w1*var1/denominator+w2*var2/denominator+(mu1-mu2)*(mu1-mu2)*(w1*w2/denominator/denominator);
    //weight
    new_component[2]=w1+w2;
    return true;
}

void  boxm2_mog3_grey_processor::merge_mixtures(vnl_vector_fixed<unsigned char, 8> & mog3_1, float w1,
                                                vnl_vector_fixed<unsigned char, 8> & mog3_2, float w2,
                                                vnl_vector_fixed<unsigned char, 8> & mog3_3)
{
    float w3=w1+w2;if(w3<=0.0f)return;

    float mog3_float_1[9]={0.0f};
    float mog3_float_2[9]={0.0f};
    for(unsigned i=0;i<8;i++)
        mog3_float_1[i]=(float)mog3_1[i]/255.0f;
    if(mog3_float_1[2]>0.0f && mog3_float_1[5]>0.0f)
         mog3_float_1[8]=vcl_max(0.0f,1-mog3_float_1[2]-mog3_float_1[5]);
    
    for(unsigned i=0;i<8;i++)
        mog3_float_2[i]=(float)mog3_2[i]/255.0f;
    if(mog3_float_2[2]>0.0f && mog3_float_2[5]>0.0f)
         mog3_float_2[8]=vcl_max(0.0f,1-mog3_float_2[2]-mog3_float_2[5]);

    if(mog3_float_1[2]<=0.0f)
    {
        mog3_3=mog3_2;return;
    }
    else if(mog3_float_2[2]<=0.0f)
    {
        mog3_3=mog3_1;return;
    }
    vcl_vector<vnl_vector_fixed<float,3> > merged;

    //: Merge all the compoentns.
   for(unsigned i=0;i<3;i++)
   {
       float w1c=w1*mog3_float_1[i*3+2];
       if(w1c<=0.0f || mog3_float_1[i*3+1] <=0.0f) continue;
       for(unsigned j=0;j<3;j++)
       {
           vnl_vector_fixed<float,3> new_component(0.0f);
           float w2c=w2*mog3_float_2[j*3+2];
           if(w2c<=0.0f || mog3_float_2[j*3+1] <=0.0f ) continue;
           if(merge_gauss(mog3_float_1[i*3+0],mog3_float_1[i*3+1],w1c,mog3_float_2[j*3+0],mog3_float_2[j*3+1],w2c,new_component))
               merged.push_back(new_component);
       }
   }
   //: reduce the components to nine by merging the components with smallest weight/variance ratios.
   while(merged.size()>3)
    {
        vcl_sort(merged.begin(),merged.end(),sort_components);
        int compindex=merged.size()-1;
        vnl_vector_fixed<float,3> new_component(0.0f);
        if(merge_gauss(merged[compindex][0],merged[compindex][1],merged[compindex][2],
                       merged[compindex-1][0],merged[compindex-1][1],merged[compindex-1][2],
                       new_component))
        {
            merged.erase(merged.begin()+compindex);
            merged.erase(merged.begin()+compindex-1);
            merged.push_back(new_component);
        }
    }

   //: renormalize the weights so that they sum to 1.
   float sum=0.0f;
   for(unsigned i=0;i<3 && i<merged.size();i++)
       sum+=merged[i][2];
   int count=-1;
   for(unsigned i=0;i<3 && i<merged.size();i++)
   {
       mog3_3[++count]=(unsigned char)vcl_floor(merged[i][0]*255.0f);
       mog3_3[++count]=(unsigned char)vcl_floor(merged[i][1]*255.0f);
       if(i<2)
           mog3_3[++count]=(unsigned char)vcl_floor(merged[i][2]/sum*255.0f);
   }


}