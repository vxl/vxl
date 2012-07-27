float phongs_intensity(float kd, float ks, float gamma,
                       float4 normal, float4 view_dir)
{
  float4 sun_dir ={sin(0.325398)*cos(-3.69),sin(0.325398)*sin(-3.69),cos(0.325398),0};

  float4 sun_reflected_dir = sun_dir - 2*normal*dot(normal,sun_dir);
  float diffuse = kd* fabs(dot(normal,sun_dir));
  float specular = fabs(dot(view_dir,sun_reflected_dir));
  float intensity = diffuse + ks*pow(specular,gamma);
  return intensity;
}

float phongs_prob_density(float  mean_obs,float4 viewdir, float8 mixture)
{
  float4 normal = {sin(mixture.s3)*cos(mixture.s4),
                   sin(mixture.s3)*sin(mixture.s4),
                   cos(mixture.s3),0};
  float predicted_val = phongs_intensity(mixture.s0,
                                         mixture.s1,
                                         mixture.s2,
                                         normal,
                                         viewdir);
  float diff = mean_obs-predicted_val;
  float prob_density = 1.0f;
  if (mixture.s5 > 0.0f)
  {
    float numer = exp(-0.5*diff*diff/( mixture.s5));
    float denom = sqrt(2*M_PI* mixture.s5);
    prob_density = numer/denom;
  }
  return prob_density;
}

#ifdef PREINF_PHONGS
//preinf step cell functor
void step_cell_preinf_phongs(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
  //keep track of cells being hit
  //cell data, i.e., alpha and app model is needed for some passes
  float  alpha    = aux_args.alpha[data_ptr];
  float8 mixture    = aux_args.mog[data_ptr];
  int cum_int   = aux_args.seg_len[data_ptr];
  int mean_int  = aux_args.mean_obs[data_ptr];
  float mean_obs= convert_float(mean_int) / convert_float(cum_int);
  float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;
  float seg_len = d*aux_args.linfo->block_len;
  float PI = 0.0f;
  if (cum_len>1.0e-10f)
  {
    // The mean intensity for the cell
    PI = phongs_prob_density(mean_obs,
                             aux_args.viewdir,
                             mixture);


    // Calculate pre and vis infinity
    float diff_omega = exp(-alpha * seg_len);
    float vis_prob_end = *(aux_args.vis_inf)* diff_omega;
    // updated pre                      Omega         *   PI
    *(aux_args.pre_inf) += (*(aux_args.vis_inf) - vis_prob_end) *  PI;
    // updated visibility probability
    *(aux_args.vis_inf) = vis_prob_end;
  }
}
#endif // PREINF_phongs

#ifdef BAYES_PHONGS
// bayes step cell functor
void step_cell_bayes_phongs(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
  // slow beta calculation ----------------------------------------------------
  float  alpha    = aux_args.alpha[data_ptr];
  CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
  float weight3   = (1.0f-mixture.s2-mixture.s5);

  // load aux data
  int cum_int = aux_args.seg_len[data_ptr];
  int mean_int = aux_args.mean_obs[data_ptr];
  float mean_obs = convert_float(mean_int) / convert_float(cum_int);
  float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;
  float seg_len = d*aux_args.linfo->block_len;

  float ray_beta, vis_cont;
  float PI = 0.0;
  if (cum_len>1.0e-10f)
  {
    PI = phongs_prob_density(mean_obs,
                             aux_args.viewdir,
                             mixture);

    // calculate this ray's contribution to beta
    ray_beta = (*(aux_args.ray_pre) + PI*(*(aux_args.ray_vis)))*d/aux_args.norm;
    vis_cont = (*(aux_args.ray_vis)) * d;

    //update ray_pre and ray_vis
    float temp  = exp(-alpha * seg_len);
    // updated pre Omega         *  PI
    *(aux_args.ray_pre) += (*(aux_args.ray_vis))*(1.0f-temp)*PI;
    // updated visibility probability
    *(aux_args.ray_vis) *= temp;

    //discretize and store beta and vis contribution
    int beta_int = convert_int_rte(ray_beta * SEGLEN_FACTOR);
    atom_add(&aux_args.beta_array[data_ptr], beta_int);
    int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
  }
  //-------------------------------------------------------------------------- */
}
#endif // BAYES_phongs

