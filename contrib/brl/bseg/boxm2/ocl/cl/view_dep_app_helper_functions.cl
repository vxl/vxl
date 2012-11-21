//The BOXM2_MOG6_view datatype is a float16 and is laid out as follows:
// [mu_x, S_x, mu_y, S_y, mu_z, S_z, mu_(-x), S_(-x), mu_(-y), S_(-y), mu_(-z), S_(-z)]
// Note that S is not the same as Sigma. It is an intermediate quantity from which (using also BOXM2_NUM_OBS_VIEW datatype) Sigma can be computed.

//The BOXM2_NUM_OBS_VIEW datatype is a float8 and is laid out as follows:
// [num_obs_(x),num_obs_(y),num_obs_(z),num_obs_(-x),num_obs_(-y),num_obs_(-z) ]
// num_obs_(x) is simply the sum of weights received so far for the appearance model at direction x.
// The weights are computed as visibility times the dot product between the viewing direction and appearance model direction. 

//Select the relevant three quantities from mixture using viewing direction and write them in view_dep_mixture.
float select_view_dep_mog(float16 mixture, float4 view_dir, float8 * view_dep_mixture)
{
    if( view_dir.x < 0.0f)
    {
        (*view_dep_mixture).s0 = mixture.s0;
        (*view_dep_mixture).s1 = mixture.s1;
        (*view_dep_mixture).s2 = -view_dir.x;
    }
    else
    {
        (*view_dep_mixture).s0 = mixture.s6;
        (*view_dep_mixture).s1 = mixture.s7;
        (*view_dep_mixture).s2 = view_dir.x; 
    }
    
    if( view_dir.y < 0.0f)
    {
        (*view_dep_mixture).s3 = mixture.s2;
        (*view_dep_mixture).s4 = mixture.s3;
        (*view_dep_mixture).s5 = -view_dir.y;
    }
    else
    {
        (*view_dep_mixture).s3 = mixture.s8;
        (*view_dep_mixture).s4 = mixture.s8;
        (*view_dep_mixture).s5 = view_dir.y; 
    }

        
    if( view_dir.z < 0.0f)
    {
        (*view_dep_mixture).s6 = mixture.s4;
        (*view_dep_mixture).s7 = mixture.s5;
        return -view_dir.z;
    }
    else
    {
        (*view_dep_mixture).s6 = mixture.sa;
        (*view_dep_mixture).s7 = mixture.sb;
        return view_dir.z;
    }
    
}

//Select the relevant three nobs from mixture using viewing direction and write them in view_dep_nobs.
void select_view_dep_nobs(float8 nobs, float4 view_dir, float4 * view_dep_nobs)
{
    if( view_dir.x < 0)
        (*view_dep_nobs).s0 = nobs.s0;
    else
        (*view_dep_nobs).s0 = nobs.s3;
        
    if( view_dir.y < 0)
        (*view_dep_nobs).s1 = nobs.s1;
    else
        (*view_dep_nobs).s1 = nobs.s4;

        
    if( view_dir.z < 0)
        (*view_dep_nobs).s2 = nobs.s2;
    else
        (*view_dep_nobs).s2 = nobs.s5;
    
}

//Given updated appearance paramters and the viewing direction, update the relevant components
void update_view_dep_mixture(float16* post_mix, float4 view_dir, float16 *data)
{
    
    if( view_dir.x < 0)
    {
        (*post_mix).s0 = (*data).s1;
        (*post_mix).s1 = (*data).s2;
    }
    else
    {
        (*post_mix).s6 = (*data).s1;
        (*post_mix).s7 = (*data).s2;
    }
    
    if( view_dir.y < 0)
    {
        (*post_mix).s2 = (*data).s5;
        (*post_mix).s3 = (*data).s6;
    }
    else
    {
        (*post_mix).s8 = (*data).s5;
        (*post_mix).s9 = (*data).s6;
    }

    if( view_dir.z < 0)
    {
        (*post_mix).s4 = (*data).s9;
        (*post_mix).s5 = (*data).sa;
    }
    else
    {
        (*post_mix).sa = (*data).s9;
        (*post_mix).sb = (*data).sa;
    }
}

//Given updated appearance paramters and the viewing direction, update the relevant components
void update_view_dep_nobs(float8* post_nobs, float4 view_dir, float16 *data)
{
    if( view_dir.x < 0)
      (*post_nobs).s0 = (*data).s4;
    else
      (*post_nobs).s3 = (*data).s4;
    
    if( view_dir.y < 0)
      (*post_nobs).s1 = (*data).s8;
    else
      (*post_nobs).s4 = (*data).s8;

    if( view_dir.z < 0)
      (*post_nobs).s2 = (*data).sb;
    else
      (*post_nobs).s5 = (*data).sb;
    
}

//Compute the sigmas of each component using the nobs
void compute_sigmas(float8* view_dep_mixture, float4 * view_dep_nobs)
{
  if( (*view_dep_nobs).s0 > 0)
    (*view_dep_mixture).s1 = sqrt( (*view_dep_mixture).s1 / (*view_dep_nobs).s0);
  else
    (*view_dep_mixture).s1 = 0.0f;

  if( (*view_dep_nobs).s1 > 0)
    (*view_dep_mixture).s4 = sqrt( (*view_dep_mixture).s4 / (*view_dep_nobs).s1);
  else
    (*view_dep_mixture).s4 = 0.0f;

  if( (*view_dep_nobs).s2 > 0)
    (*view_dep_mixture).s7 = sqrt( (*view_dep_mixture).s7 / (*view_dep_nobs).s2);
  else
    (*view_dep_mixture).s7 = 0.0f;     
}

void view_dep_update_cell(float16 * data, float4 aux_data)
{
    float mu0 = (*data).s1, sk0 = (*data).s2, w0 = (*data).s3;
    float mu1 = (*data).s5, sk1 = (*data).s6, w1 = (*data).s7;
    float mu2 = (*data).s9, sk2 = (*data).sa;
    float w2=0.0f;
    if (w0>0.0f && w1>0.0f)
        w2=1-(*data).s3-(*data).s7;

    float Nobs0 = (*data).s4, Nobs1 = (*data).s8, Nobs2 = (*data).sb;


    update_view_dep_app (aux_data.y,              //mean observation
                         aux_data.w,              //cell_visability
                         &mu0,&sk0,w0,&Nobs0,
                         &mu1,&sk1,w1,&Nobs1,
                         &mu2,&sk2,w2,&Nobs2);
            
    float beta = aux_data.z; //aux_data.z/aux_data.x;
    clamp(beta,0.5f,2.0f);
    (*data).s0 *= beta;
    (*data).s1=mu0; (*data).s2=sk0, (*data).s3=w0;(*data).s4=(float)Nobs0;
    (*data).s5=mu1; (*data).s6=sk1, (*data).s7=w1;(*data).s8=(float)Nobs1;
    (*data).s9=mu2; (*data).sa=sk2, (*data).sb=(float)Nobs2;
}
