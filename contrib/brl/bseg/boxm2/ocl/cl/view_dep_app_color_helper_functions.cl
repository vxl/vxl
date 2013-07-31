//The BOXM2_MOG6_view datatype is a float16 and is laid out as follows:
// [mu_x, sigma_x, mu_y, sigma_y, mu_z, sigma_z, mu_(-x), sigma_(-x), mu_(-y), sigma_(-y), mu_(-z), sigma_(-z)]

//The BOXM2_NUM_OBS_VIEW datatype is a float8 and is laid out as follows:
// [num_obs_(x),num_obs_(y),num_obs_(z),num_obs_(-x),num_obs_(-y),num_obs_(-z) ]
// num_obs_(x) is simply the sum of weights received so far for the appearance model at direction x.
// The weights are computed as visibility times the dot product between the viewing direction and appearance model direction.

//DECLARE app_model_view_directions

/*__constant  float4  app_model_view_directions[8] = {  (float4)(0,       0,      1, 0),
                                                      (float4)(0.707,    0,      0.707,0),
                                                      (float4)(0.354,    0.612,  0.707, 0),
                                                      (float4)(-0.354,    0.612, 0.707,0),
                                                      (float4)(-0.707,    0,     0.707,0),
                                                      (float4)(-0.354,   -0.612, 0.707,0),
                                                      (float4)(0.354,    -0.612, 0.707,0),
                                                      (float4)(0,         0,     0,0)};*/


__constant  float4  app_model_view_directions[8] = {  (float4)(0,       0,      1,  0),
                                                      (float4)(1,    0,         0,  0),
                                                      (float4)(0.5,    0.866,   0,  0),
                                                      (float4)(-0.5,    0.866,  0,  0),
                                                      (float4)(-1,    0,        0,  0),
                                                      (float4)(-0.5,   -0.866,  0,  0),
                                                      (float4)(0.5,   -0.866,   0,  0),
                                                      (float4)(0,         0,    0,  0)  };

void compute_app_model_weights(float* app_model_weights, float4 viewdir,__constant float4* app_model_view_directions)
{
    //compute the dot product btw ray dir and canonical directions
    //normalize weights to 1 in the end
    float sum_weights = 0.0f;
    for(short i = 0; i < 8; i++) {
        float cos_angle = -dot(viewdir,app_model_view_directions[i]);
        app_model_weights[i] = (cos_angle > 0.01f) ? cos_angle : 0.0f; //if negative, set to 0
	// app_model_weights[i] = 0.125f;
        sum_weights += app_model_weights[i];
    }

    for(short i = 0; i < 8; i++)
      app_model_weights[i] /= sum_weights;
}


float view_dep_mixture_model(float4 x, int16 mixture, float* app_model_weights)
{
    int* mixture_array = (int*)(&mixture);

    float PI = 0;
    for(short i= 0; i < 8; i++) {

      uchar4 tmp_mu = as_uchar4(mixture_array[2*i]) ;
      uchar4 tmp_sig = as_uchar4(mixture_array[2*i+1]);
      PI += app_model_weights[i] * gauss_prob_density_rgb(x, convert_float4(tmp_mu)/255.0f,convert_float4(tmp_sig)/255.0f );
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
        float4 tmp_mean = convert_float4(as_uchar4(mixture[2*i]))/255.0f;
        float4 tmp_sig  = convert_float4(as_uchar4(mixture[2*i+1]))/255.0f;


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

        float4 mean_fp = (float4)(mean_R,mean_G,mean_B,0);
        float4 sig_fp = (float4) (sqrt(sk_R/n),sqrt(sk_G/n),sqrt(sk_B/n),0);

        uchar4 mean_updt = convert_uchar4_sat_rte(mean_fp * 255.0f);
        uchar4 sig_updt = convert_uchar4_sat_rte(sig_fp * 255.0f);


        mixture[2*i] = as_int(mean_updt);
        mixture[2*i+1]= as_int(sig_updt);
        nobs[i] = n;
	   }

  }
}
