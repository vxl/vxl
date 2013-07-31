
#ifdef SEGLEN
//Update step cell functor::seg_len
void step_cell_seglen(AuxArgs aux_args, int data_ptr,  int data_ptr_tt, float d)
{
    //SLOW and accurate method
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);

    //increment mean observation
    int cum_obsR = convert_int_rte(d * aux_args.obs.x * SEGLEN_FACTOR);
    int cum_obsG = convert_int_rte(d * aux_args.obs.y * SEGLEN_FACTOR);
    int cum_obsB = convert_int_rte(d * aux_args.obs.z * SEGLEN_FACTOR);
    atom_add(&aux_args.mean_obsR[data_ptr], cum_obsR);
    atom_add(&aux_args.mean_obsG[data_ptr], cum_obsG);
    atom_add(&aux_args.mean_obsB[data_ptr], cum_obsB);
}
#endif // SEGLEN

#ifdef PREINF


//preinf step cell functor
void step_cell_preinf(AuxArgs aux_args, int data_ptr, int data_ptr_tt, float d)
{
  //keep track of cells being hit
  //cell data, i.e., alpha and app model is needed for some passes
  float  alpha    = aux_args.alpha[data_ptr_tt];
  CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr_tt])/NORM;

  float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR;
  float4 meanObs = convert_float4(as_uchar4(aux_args.mean_obs[data_ptr]))/255.0f;

  //calculate pre_infinity denomanator (shape of image)
  // if total length of rays is too small, do nothing
  float PI = 0.0f;
  if (cum_len>1.0e-10f)
  {
      /* The mean intensity for the cell */
#ifdef YUV
      PI = gauss_prob_density(meanObs.x, mixture.s0, mixture.s4);  //YUV edit
#else
      PI = gauss_prob_density_rgb(meanObs, mixture.s0123, mixture.s4567);
#endif
  }

  // calc vis and pre infinity
  float diff_omega = exp(-alpha * d * aux_args.linfo->block_len);
  float vis_prob_end = (*aux_args.vis_inf) * diff_omega;

  // updated pre                      Omega         *   PI
  (*aux_args.pre_inf) += ((*aux_args.vis_inf) - vis_prob_end) *  PI;

  // updated visibility probability
  (*aux_args.vis_inf) = vis_prob_end;
}
#endif // PREINF

#ifdef BAYES

void bayes_ratio_rgb_ind(     float   seg_len,          // segment length for this ray
                              float4  mean_obs,         // mean observation for the currently intersected cell
                              float * ray_pre,              // ray pre
                              float * ray_vis,              // ray vis
                              float   norm,             // normalization (pre_inf + vis_inf)
                              float * cell_beta,
                              float * cell_vis,
                              float   alpha,
                              float8  mixture)
{
    // linear thread id
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

    // Compute PI for all threads
    float PI = 0.0f;
    if (seg_len>1.0e-10f)
    {
      #ifdef YUV
            PI = gauss_prob_density(mean_obs.x, mixture.s0, mixture.s4);  //YUV edit
      #else
            PI = gauss_prob_density_rgb(mean_obs, mixture.s0123, mixture.s4567);
      #endif
      (*cell_beta) = ((*ray_pre) + PI*(*ray_vis))*seg_len/norm;
      (*cell_vis) = (*ray_vis) * seg_len;
    }

    //update ray pre and vis
    float temp = exp(-alpha * seg_len);
    (*ray_pre) += (*ray_vis)*(1-temp)*PI;
    (*ray_vis) *= temp;
}

//bayes step cell functor
void step_cell_bayes(AuxArgs aux_args, int data_ptr, int data_ptr_tt, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr_tt];

    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr_tt])/NORM;

    //load aux data
    float4 meanObs = convert_float4(as_uchar4(aux_args.mean_obs[data_ptr_tt]))/255.0f;

    float cell_beta = 0.0f;
    float cell_vis = 0.0f;
    bayes_ratio_rgb_ind( d * aux_args.linfo->block_len,
                         meanObs,
                         aux_args.ray_pre,
                         aux_args.ray_vis,
                         aux_args.norm,
                         &cell_beta,
                         &cell_vis,
                         alpha,
                         mixture);

    //discretize and store beta and vis contribution
    if(cell_beta > 0.0f) {
      int beta_int = convert_int_rte(cell_beta * SEGLEN_FACTOR);
      atom_add(&aux_args.beta_array[data_ptr], beta_int);
    }
    if(cell_vis > 0.0f) {
      int vis_int  = convert_int_rte(cell_vis * SEGLEN_FACTOR);
      atom_add(&aux_args.vis_array[data_ptr], vis_int);
    }
}
#endif // BAYES
