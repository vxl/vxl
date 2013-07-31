
#ifdef SEGLEN
//Update step cell functor::seg_len
void step_cell_seglen(AuxArgs aux_args, int data_ptr,  int data_ptr_tt, float d)
{
    //SLOW and accurate method
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);
    int cum_obs = convert_int_rte(d * aux_args.obs * SEGLEN_FACTOR);
    atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
}
#endif // SEGLEN

#ifdef PREINF
void pre_infinity_opt_view_based(  float    seg_len,
                                   float    cum_len,
                                   float    mean_obs,
                                   float  * vis_inf,
                                   float  * pre_inf,
                                   float    alpha,
                                   MOG_TYPE   mixture,
                                   float* app_model_weights)
{
    /* if total length of rays is too small, do nothing */
    float PI = 0.0f;
    if (cum_len>1.0e-10f)
    {

#ifdef MOG_VIEW_DEP_COMPACT
        CONVERT_FUNC_FLOAT16(mixture_float,mixture);
        PI = view_dep_mixture_model(mean_obs, mixture_float, app_model_weights);
#else
        PI = view_dep_mixture_model(mean_obs, mixture, app_model_weights);
#endif
        /* Calculate pre and vis infinity */
        float diff_omega = exp(-alpha * seg_len);
        float vis_prob_end = (*vis_inf) * diff_omega;

        /* updated pre                      Omega         *   PI  */
        (*pre_inf) += ((*vis_inf) - vis_prob_end) *  PI;
        /* updated visibility probability */
        (*vis_inf) = vis_prob_end;
  }
}

//preinf step cell functor
void step_cell_preinf(AuxArgs aux_args, int data_ptr, int data_ptr_tt, float d)
{
    float  alpha    = aux_args.alpha[data_ptr_tt];
    MOG_TYPE mixture = aux_args.mog[data_ptr_tt];

    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;

    //calculate pre_infinity denomanator (shape of image)
    pre_infinity_opt_view_based( d*aux_args.linfo->block_len,
                                 cum_len*aux_args.linfo->block_len,
                                 mean_obs,
                                 aux_args.vis_inf,
                                 aux_args.pre_inf,
                                 alpha,
                                 mixture,
                                 aux_args.app_model_weights);

    aux_args.ray_dir[data_ptr] = aux_args.viewdir;
}
#endif // PREINF

#ifdef BAYES


/* bayes ratio independent functor (for independent rays) */
void bayes_ratio_ind_view_based( float  seg_len,
                                 float  alpha,
                                 MOG_TYPE mixture,
                                 float* app_model_weights,
                                 float  cum_len,
                                 float  mean_obs,
                                 float  norm,
                                 float* ray_pre,
                                 float* ray_vis,
                                 float* ray_beta,
                                 float* vis_cont )
{
    float PI = 0.0f;

    /* Compute PI for all threads */
    if (seg_len > 1.0e-10f) {    /* if  too small, do nothing */

#ifdef MOG_VIEW_DEP_COMPACT
        CONVERT_FUNC_FLOAT16(mixture_float,mixture);
        PI = view_dep_mixture_model(mean_obs, mixture_float, app_model_weights);
#else
        PI = view_dep_mixture_model(mean_obs, mixture, app_model_weights);
#endif

        //calculate this ray's contribution to beta
        (*ray_beta) = ((*ray_pre) + PI*(*ray_vis))*seg_len/norm;
        (*vis_cont) = (*ray_vis) * seg_len;

        //update ray_pre and ray_vis
        float temp  = exp(-alpha * seg_len);

        /* updated pre                      Omega         *  PI         */
        (*ray_pre) += (*ray_vis)*(1.0f-temp)*PI;//(image_vect[llid].z - vis_prob_end) * PI;
        /* updated visibility probability */
        (*ray_vis) *= temp;
    }

}


//bayes step cell functor
void step_cell_bayes(AuxArgs aux_args, int data_ptr, int data_ptr_tt, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr_tt];
    MOG_TYPE mixture = aux_args.mog[data_ptr_tt];

    //load aux data
    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;

    float ray_beta, vis_cont;
    bayes_ratio_ind_view_based( d*aux_args.linfo->block_len,
                                alpha,
                                mixture,
                                aux_args.app_model_weights,
                                cum_len*aux_args.linfo->block_len,
                                mean_obs,
                                aux_args.norm,
                                aux_args.ray_pre,
                                aux_args.ray_vis,
                                &ray_beta,
                                &vis_cont);

    //discretize and store beta and vis contribution
    int beta_int = convert_int_rte(ray_beta * SEGLEN_FACTOR);
    atom_add(&aux_args.beta_array[data_ptr], beta_int);
    int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
}
#endif // BAYES
