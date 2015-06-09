//The BOXM2_MOG6_view datatype is a float16 and is laid out as follows:
// [mu_x, sigma_x, mu_y, sigma_y, mu_z, sigma_z, mu_(-x), sigma_(-x), mu_(-y), sigma_(-y), mu_(-z), sigma_(-z)]

//The BOXM2_NUM_OBS_VIEW datatype is a float8 and is laid out as follows:
// [num_obs_(x),num_obs_(y),num_obs_(z),num_obs_(-x),num_obs_(-y),num_obs_(-z) ]
// num_obs_(x) is simply the sum of weights received so far for the appearance model at direction x.
// The weights are computed as visibility times the dot product between the viewing direction and appearance model direction.


float view_dep_mixture_model(float4 x, MOG_TYPE mixture, float* app_model_weights)
{
    int* mixture_array = (int*)(&mixture);

    float PI = 0;
    for(short i= 0; i < 8; i++) {

#ifdef YUV
      float4 mu = unpack_yuv(mixture_array[2*i]);
#else
      float4 mu = convert_float4(unpack_uchar4(mixture_array[2*i]))/255.0f ;
#endif
      uchar4 tmp_sig = unpack_uchar4(mixture_array[2*i+1]);
      PI += app_model_weights[i] * gauss_prob_density_rgb(x, mu, convert_float4(tmp_sig)/255.0f );
      // PI += app_model_weights[i] * gauss_prob_density(x.x, (convert_float4(tmp_mu)/255.0f).x,(convert_float4(tmp_sig)/255.0f).x );
    }
     return PI;
}


void update_view_dep_app(float4 x, float w, float* view_dep_w, MOG_TYPE* mixture_formal, float * nobs)
{
  int* mixture = (int*)(mixture_formal)  ;
  for(short i = 0; i < 8; i++)
    {
      if(view_dep_w[i] > 0.01f)   //update only if view weight is positive
      {
#ifdef YUV
        float4 tmp_mean = unpack_yuv(mixture[2*i]);
#else
        float4 tmp_mean = convert_float4(unpack_uchar4(mixture[2*i]))/255.0f;
#endif
        float4 tmp_sig  = convert_float4(unpack_uchar4(mixture[2*i+1]))/255.0f;


        float n = nobs[i];
        float sk_R = tmp_sig.x * tmp_sig.x * n;
        float sk_G = tmp_sig.y * tmp_sig.y * n;
        float sk_B = tmp_sig.z * tmp_sig.z * n;

        float mean_R = tmp_mean.x;
        float mean_G = tmp_mean.y;
        float mean_B = tmp_mean.z;

        if( n  == 0.0f)
        {
          n = w * view_dep_w[i];
          sk_R = 0.0f;
          sk_G = 0.0f;
          sk_B = 0.0f;
          mean_R = x.x;
          mean_G = x.y;
          mean_B = x.z;
        }
        else
        {
            n +=  w * view_dep_w[i];
            float prev_mean_R = mean_R;
            float prev_mean_G = mean_G;
            float prev_mean_B = mean_B;
            mean_R =  prev_mean_R + (x.x-prev_mean_R)*w*view_dep_w[i] / n;
            mean_G =  prev_mean_G + (x.y-prev_mean_G)*w*view_dep_w[i] / n;
            mean_B =  prev_mean_B + (x.z-prev_mean_B)*w*view_dep_w[i] / n;

            sk_R += w*view_dep_w[i]*(x.x-prev_mean_R)*( x.x - mean_R );
            sk_G += w*view_dep_w[i]*(x.y-prev_mean_G)*( x.y - mean_G );
            sk_B += w*view_dep_w[i]*(x.z-prev_mean_B)*( x.z - mean_B );
        }

        // dec: use the "alpha" component to signal that a valid view direction has been
        // observed.  If the flag is unset when rendering, you'll want to avoid incorporating
        // that viewpoint into the output.
        float4 mean_fp = (float4)(mean_R,mean_G,mean_B,1.0f);
        float4 sig_fp = (float4) (sqrt(sk_R/n),sqrt(sk_G/n),sqrt(sk_B/n),1.0f);

#ifdef YUV
        int mean_updt = pack_yuv(mean_fp);
#else
        int mean_updt = pack_uchar4(convert_uchar4_sat_rte(mean_fp * 255.0f));
#endif
        uchar4 sig_updt = convert_uchar4_sat_rte(sig_fp * 255.0f);

        mixture[2*i] = mean_updt;
        mixture[2*i+1]= pack_uchar4(sig_updt);
        nobs[i] = n;
	   }
      else {
        if (nobs[i] == 0.0f) {
            // dec: no prior observations, and this viewpoint weight is 0.
            // Make sure that "valid data" flag, i.e. the alpha component, is not set
            mixture[2*i] = 0;
        }
      }
  }
}
