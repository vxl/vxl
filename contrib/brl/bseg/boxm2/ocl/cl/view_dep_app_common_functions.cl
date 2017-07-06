//The BOXM2_MOG6_view datatype is a float16 and is laid out as follows:
// [mu_x, sigma_x, mu_y, sigma_y, mu_z, sigma_z, mu_(-x), sigma_(-x), mu_(-y), sigma_(-y), mu_(-z), sigma_(-z)]

//The BOXM2_NUM_OBS_VIEW datatype is a float8 and is laid out as follows:
// [num_obs_(x),num_obs_(y),num_obs_(z),num_obs_(-x),num_obs_(-y),num_obs_(-z) ]
// num_obs_(x) is simply the sum of weights received so far for the appearance model at direction x.
// The weights are computed as visibility times the dot product between the viewing direction and appearance model direction.

//DECLARE app_model_view_directions

#if 0
__constant  float4  app_model_view_directions[8] = {  (float4)(0,       0,      1,  0),
                                                      (float4)(1,    0,         0,  0),
                                                      (float4)(0.5,    0.866,   0,  0),
                                                      (float4)(-0.5,    0.866,  0,  0),
                                                      (float4)(-1,    0,        0,  0),
                                                      (float4)(-0.5,   -0.866,  0,  0),
                                                      (float4)(0.5,   -0.866,   0,  0),
                                                      (float4)(0,         0,    0,  0)  };
#endif

#if 1
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

#if 0
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
    for(short i = 0; i < 8; i++) {
        float cos_angle = -dot(viewdir,app_model_view_directions[i]);
        app_model_weights[i] = (cos_angle > 0.01f) ? cos_angle : 0.0f; //if negative, set to 0
        sum_weights += app_model_weights[i];
    }

#define NORMALIZE_VIEW_WEIGHTS
#ifdef NORMALIZE_VIEW_WEIGHTS
    for(short i = 0; i < 8; i++) {
      app_model_weights[i] /= sum_weights;
    }
#endif
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
