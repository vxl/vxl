//The BOXM2_MOG6_view datatype is a float16 and is laid out as follows:
// [mu_dir1, sigma_dir1, mu_dir2, sigma_dir2...etc]

//The BOXM2_NUM_OBS_VIEW datatype is a float8 and is laid out as follows:
// [num_obs_dir1,num_obs_dir2...etc]
// num_obs_(x) is simply the sum of weights received so far for the appearance model at direction x.
// The weights are computed as visibility times the dot product between the viewing direction and appearance model direction.

//DECLARE app_model_view_directions


#if 0
//used for aerial scenes.
__constant  float4  app_model_view_directions[8] = {  (float4)(0,       0,      1, 0),
                                                      (float4)(0.707,    0,      0.707,0),
                                                      (float4)(0.354,    0.612,  0.707, 0),
                                                      (float4)(-0.354,    0.612, 0.707,0),
                                                      (float4)(-0.707,    0,     0.707,0),
                                                      (float4)(-0.354,   -0.612, 0.707,0),
                                                      (float4)(0.354,    -0.612, 0.707,0),
                                                      (float4)(0,         0,     0,0)};
#endif

#if 1
//used for aerial scenes.
__constant  float4  app_model_view_directions[8] = {  (float4)(0.81654 ,  0.00000 ,  0.57729 ,0),
                                                       (float4)(0.00000 ,  0.81654 ,  0.57729,0),
                                                      (float4)(-0.81654 ,  0.00000 ,  0.57729,0),
                                                       (float4)(0.00000 , -0.81654 ,  0.57729,0),
                                                       (float4)(0.57735 ,  0.57735 ,  0.57735,0),
                                                       (float4)(0.57735 , -0.57735 ,  0.57735,0),
                                                      (float4)(-0.57735 ,  0.57735 ,  0.57735,0),
                                                      (float4)(-0.57735 , -0.57735 ,  0.57735,0) };
#endif

#if 0
//used for motion capture scenes
__constant  float4  app_model_view_directions[8] = {  (float4)(0,       0,      1, 0),
                                                      (float4)(1,    0,      0,0),
                                                      (float4)(0.5,    0.866,  0, 0),
                                                      (float4)(-0.5,    0.866, 0,0),
                                                      (float4)(-1,    0,     0,0),
                                                      (float4)(-0.5,   -0.866, 0,0),
                                                      (float4)(0.5,   -0.866, 0,0),
                                                      (float4)(0,         0,     0,0)};
#endif

void compute_app_model_weights(float* app_model_weights, float4 viewdir,__constant float4* app_model_view_directions)
{
    //compute the dot product btw ray dir and canonical directions
    //normalize weights to 1 in the end
    float sum_weights = 0.0f;
    for (short i = 0; i < 8; i++) {
        float cos_angle = -dot(viewdir,app_model_view_directions[i]);
        app_model_weights[i] = (cos_angle > 0.01f) ? (cos_angle) : 0.0f; //if negative, set to 0
        sum_weights += app_model_weights[i];
    }

    for (short i = 0; i < 8; i++)
        app_model_weights[i] /= sum_weights;
}

#if 0
void compute_app_model_weights_unormalized(float* app_model_weights, float4 viewdir,__constant float4* app_model_view_directions)
{
    //compute the dot product btw ray dir and canonical directions
    for (short i = 0; i < 8; i++) {
        float cos_angle = -dot(viewdir,app_model_view_directions[i]);
        app_model_weights[i] = (cos_angle > 0.01f) ? cos_angle : 0.0f; //if negative, set to 0
    }
}
#endif // 0


float view_dep_mixture_model(float x,float16 mixture, float* app_model_weights)
{
    float* mixture_array = (float*)(&mixture);
    float PI = 0;
    for (short i= 0; i < 8; i++)
        PI += app_model_weights[i] * gauss_prob_density(x, mixture_array[2*i],mixture_array[2*i+1]);
    return PI;
}

void update_view_dep_app(float x, float w, float* view_dep_w, float* mixture, float * nobs, float mog_fixed_std)
{
    for (short i = 0; i < 8; i++)
    {
        if (view_dep_w[i] > 0.01f)   //update only if view weight is positive
        {
            float mean = mixture[2*i];
            float sigma = mixture[2*i+1];
            float n = nobs[i];
            float sk = sigma * sigma * n;

            if ( n  == 0.0f) //initial condition
            {
                n = w * view_dep_w[i];
                sk = 0.0f;
                mean = x;
            }
            else
            {
                n +=  w * view_dep_w[i];
                float prev_mean = mean;
                mean =  prev_mean + (x-prev_mean)* w*view_dep_w[i] / n;
                sk += w*view_dep_w[i]*(x-prev_mean)*( x - mean );
            }
            mixture[2*i] = mean;
            //if nobs < 10, variance is not to be trusted, keep it fixed at  mog_fixed_std.
            if (n > 10.0f &&  mog_fixed_std > 0)
              mixture[2*i+1] = clamp(sqrt(sk / n) , 0.02f,0.20f); //clamp sigma
            else
              mixture[2*i+1] =  mog_fixed_std;
            nobs[i] = n; //clamp(n,0.0f, 16.0f);
        }
    }
}


#if 0
void update_view_dep_app(float x, float w, float* view_dep_w, float* mixture, float * nobs_array)
{
    for (short i = 0; i < 8; i++)
    {
        if (view_dep_w[i] > 0.01f)   //update only if view weight is positive
        {
            float mean = mixture[2*i];
            float sigma = mixture[2*i+1];
            float n = nobs_array[i];
            float nobs = nobs_array[i+8] + 1;

            if ( n  == 0.0f) //initial condition
            {
                n = w * view_dep_w[i];
                sigma = 0.15f;
                mean = x;
            }
            else
            {
                n +=  w * view_dep_w[i];
                float prev_mean = mean;
                float rho = ( 1-(w*view_dep_w[i] / n) ) / nobs + (w*view_dep_w[i] / n);
                float rho_comp = 1-rho;
                mean =  prev_mean + (x-prev_mean)*  rho;
                sigma = sqrt (sigma * sigma * rho_comp + (rho * rho_comp) * (x-prev_mean)*(x-prev_mean) );
            }
            mixture[2*i] = mean;
            if (n > 5.0f)
              mixture[2*i+1] = clamp( sigma, 0.02f,0.2f); //clamp sigma
            else
              mixture[2*i+1] = 0.15f;

            nobs_array[i] = n;
            nobs_array[i+8] = clamp(nobs,0.0f, 20.0f);
        }
    }
}
#endif // 0
