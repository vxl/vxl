
#ifdef SEGLEN
//Update step cell functor::seg_len
void step_cell_seglen(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);

    //increment mean observation
    int cum_obsR = convert_int_rte(d * aux_args.obs.x * SEGLEN_FACTOR);
    int cum_obsG = convert_int_rte(d * aux_args.obs.y * SEGLEN_FACTOR);
    int cum_obsB = convert_int_rte(d * aux_args.obs.z * SEGLEN_FACTOR);
    atom_add(&aux_args.mean_obsR[data_ptr], cum_obsR);
    atom_add(&aux_args.mean_obsG[data_ptr], cum_obsG);
    atom_add(&aux_args.mean_obsB[data_ptr], cum_obsB);


    //reset cell_ptrs to negative one every time (prevents invisible layer bug)
    aux_args.cell_ptrs[llid] = -1;
}
#endif // SEGLEN

#ifdef PREINF

void pre_infinity_opt_view_based(  float    seg_len,
                                    float    cum_len,
                                    float4    mean_obs,
                                    float  * vis_inf,
                                    float  * pre_inf,
                                    float    alpha,
                                    MOG_TYPE   mixture,
                                    float8 nobs,
                                    float* app_model_weights)
{
    /* if total length of rays is too small, do nothing */
    float PI = 0.0f;
    if (cum_len>1.0e-10f)
    {
        PI = view_dep_mixture_model(mean_obs, mixture, app_model_weights);

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
void step_cell_preinf(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    ////cell data, i.e., alpha and app model is needed for some passes
    float  alpha    = aux_args.alpha[data_ptr];
    MOG_TYPE mixture = aux_args.mog[data_ptr];
    float8 num_obs = aux_args.num_obs[data_ptr];

#ifdef YUV
    float4 mean_obs = unpack_yuv(aux_args.mean_obs[data_ptr]);
#else
    float4 mean_obs = convert_float4(unpack_uchar4(aux_args.mean_obs[data_ptr])) * 255.0f;
#endif

    float cum_len = convert_float( aux_args.seg_len[data_ptr]) / SEGLEN_FACTOR;

    //calculate pre_infinity denomanator (shape of image)
    pre_infinity_opt_view_based( d*aux_args.linfo->block_len,
                                  cum_len*aux_args.linfo->block_len,
                                  mean_obs,
                                  aux_args.vis_inf,
                                  aux_args.pre_inf,
                                  alpha,
                                  mixture,
                                  num_obs,
                                  aux_args.app_model_weights);

    aux_args.ray_dir[data_ptr] = aux_args.viewdir;
}
#endif // PREINF

#ifdef BAYES


/* bayes ratio independent functor (for independent rays) */
void bayes_ratio_ind_view_based( float  seg_len,
                                  float  alpha,
                                  MOG_TYPE mixture,
                                  float8 nobs,
                                  float* app_model_weights,
                                  float  cum_len,
                                  float4  mean_obs,
                                  float  norm,
                                  float* ray_pre,
                                  float* ray_vis,
                                  float* ray_beta,
                                  float* vis_cont )
{
    float PI = 0.0;

    /* Compute PI for all threads */
    if (seg_len > 1.0e-10f) {    /* if  too small, do nothing */
        PI = view_dep_mixture_model(mean_obs, mixture, app_model_weights);
    }

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


//bayes step cell functor
void step_cell_bayes(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{

    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];
    MOG_TYPE mixture = aux_args.mog[data_ptr];
    float8 num_obs = aux_args.num_obs[data_ptr];

    //load aux data
    int cum_int = aux_args.seg_len[data_ptr];
#ifdef YUV
    float4 mean_obs = unpack_yuv(aux_args.mean_obs[data_ptr]);
#else
    float4 mean_obs = convert_float4(unpack_uchar4(aux_args.mean_obs[data_ptr])) * 255.0f;
#endif
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;

    float ray_beta, vis_cont;
    bayes_ratio_ind_view_based( d*aux_args.linfo->block_len,
                                 alpha,
                                 mixture,
                                 num_obs,
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


    //reset cell_ptrs to -1 every time
    aux_args.cell_ptrs[llid] = -1;
}
#endif // BAYES
