float cubic_prob_density(float  mean_obs,float phi,float8 mixture)
{
    float predicted_val = mixture.s0+mixture.s1*phi+ mixture.s2*phi*phi+mixture.s3*phi*phi*phi;
    float diff = mean_obs-predicted_val;
    float prob_density = 1.0f;
    if (mixture.s4 > 0.0f)
    {
        float numer = exp(-0.5*diff*diff/( mixture.s4*mixture.s4));
        float denom = sqrt(2*M_PI)* mixture.s4;

        prob_density = numer/denom;
    }
    return prob_density;
}

#ifdef PROB_CUBIC_IMAGE
void step_cell_cubic_compute_probability_of_intensity(AuxArgs aux_args,
                                                      int data_ptr,
                                                      float d,
                                                      float * vis,
                                                      float * prob_image)
{
    CONVERT_FUNC(uchar_data,aux_args.mog[data_ptr]);
    float8 data= convert_float8(uchar_data)/NORM;

    float meanobs  = aux_args.aux1[data_ptr]/aux_args.aux0[data_ptr];
    float prob_den = cubic_prob_density(meanobs,aux_args.phi,data);
    float alpha    = aux_args.alpha[data_ptr];
    float prob     = 1-exp(-alpha*d);
    float omega    = (*vis)*prob;
    (*vis)         = (*vis)*(1-prob);
    (*prob_image) += prob_den*omega;
}
#endif

#ifdef PREINF_CUBIC
//preinf step cell functor
void step_cell_preinf_cubic(AuxArgs aux_args, int data_ptr, uchar llid, float d)
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
        /* The mean intensity for the cell */
        PI = cubic_prob_density( mean_obs,
                                 aux_args.phi,
                                 mixture);


        /* Calculate pre and vis infinity */
        float diff_omega = exp(-alpha * seg_len);
        float vis_prob_end = *(aux_args.vis_inf)* diff_omega;
        /* updated pre                      Omega         *   PI  */
        *(aux_args.pre_inf) += (*(aux_args.vis_inf) - vis_prob_end) *  PI;
        /* updated visibility probability */
        *(aux_args.vis_inf) = vis_prob_end;
    }
}
#endif // PREINF_CUBIC

#ifdef PREINF_DEPTH_CUBIC
//preinf step cell functor
void step_cell_preinf_depth_cubic(AuxArgs aux_args, int data_ptr, uchar llid, float d, float tblock)
{
    //keep track of cells being hit
    //cell data, i.e., alpha and app model is needed for some passes
    float  alpha    = aux_args.alpha[data_ptr];
    float8 mixture= aux_args.mog[data_ptr];

    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;
    float seg_len = d*aux_args.linfo->block_len;

    float PI = 0.0f;
    if (cum_len>1.0e-10f)
    {
        /* The mean intensity for the cell */
        PI = cubic_prob_density(mean_obs,
                                aux_args.phi,
                                mixture);
    }
    /* Calculate pre and vis infinity */
    float diff_omega = exp(-alpha * seg_len);
    float vis_prob_end = *(aux_args.vis_inf)* diff_omega;
    /* updated pre                      Omega         *   PI  */
    *(aux_args.pre_inf)              += (*(aux_args.vis_inf) - vis_prob_end) *  PI;
    *(aux_args.pre_depth_inf)      += (*(aux_args.vis_inf) - vis_prob_end);
    /* updated visibility probability */
    *(aux_args.vis_inf) = vis_prob_end;
}
#endif // PREINF_DEPTH_CUBIC

#ifdef UPDATE_DEPTH_DENSITY
//preinf step cell functor
void step_cell_post_depth_density_cubic(AuxArgs aux_args, int data_ptr, uchar llid, float d, float tblock)
{
    //keep track of cells being hit
    //cell data, i.e., alpha and app model is needed for some passes
    float  alpha    = aux_args.alpha[data_ptr];
    float8 mixture= aux_args.mog[data_ptr];

    int cum_int    = aux_args.seg_len[data_ptr];
    int mean_int   = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len  = convert_float(cum_int) / SEGLEN_FACTOR;
    float seg_len  = d*aux_args.linfo->block_len;

    float PI = 0.0f;
    if (cum_len>1.0e-10f)
        PI = cubic_prob_density( mean_obs,aux_args.phi,mixture);
    /* Calculate pre and vis infinity */
    float diff_omega   = exp(-alpha * seg_len);
    float vis_prob_end = *(aux_args.vis) * diff_omega;
    /* updated pre                      Omega         *   PI  */
    *(aux_args.pre)              += (*(aux_args.vis) - vis_prob_end) *  PI;
    *(aux_args.pre_depth)      += (*(aux_args.vis) - vis_prob_end);
    float post                   = *(aux_args.pre_inf)         -  *(aux_args.pre);
    float post_depth           = *(aux_args.pre_depth_inf)   -  *(aux_args.pre_depth);

    float post_PI  = post_depth > 1e-5f ? post/post_depth : 0.0;
    float density  =(*(aux_args.pre) + (*(aux_args.vis) )*PI)/((*(aux_args.pre)     + (*(aux_args.vis) )*PI) *(1-diff_omega)+( (*(aux_args.pre)+ (*(aux_args.vis) )*post_PI)*diff_omega)+ *(aux_args.vis_inf) );

    //(*(aux_args.pre)     + (*(aux_args.vis_inf) )*PI) / ((*(aux_args.pre)     + (*(aux_args.vis_inf) )*PI)*(1-diff_omega)+ (*(aux_args.pre)+ (*(aux_args.vis_inf) )*post/post_depth)*diff_omega);
    density  =  density * d;

    int density_int = convert_int_rte(density * SEGLEN_FACTOR);
    atom_add(&aux_args.depth_density[data_ptr], density_int);

    int vis_int = convert_int_rte((*(aux_args.vis)) * SEGLEN_FACTOR);
    atom_add(&aux_args.mean_vis[data_ptr], vis_int);

    /* updated visibility probability */
    *(aux_args.vis) = vis_prob_end;
}
#endif // UPDATE_DEPTH_DENSITY

#ifdef BAYES_CUBIC
//bayes step cell functor
void step_cell_bayes_cubic(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);

    //load aux data
    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;
    float seg_len = d*aux_args.linfo->block_len;

    float ray_beta, vis_cont;
    float PI = 0.0;
    if (cum_len>1.0e-10f)
    {
        PI = cubic_prob_density( mean_obs,aux_args.phi,mixture);

        //calculate this ray's contribution to beta
        ray_beta = (*(aux_args.ray_pre) + PI*(*(aux_args.ray_vis)))*d/aux_args.norm;
        vis_cont = (*(aux_args.ray_vis)) * d;

        //update ray_pre and ray_vis
        float temp  = exp(-alpha * seg_len);
        /* updated pre Omega         *  PI         */
        *(aux_args.ray_pre) += (*(aux_args.ray_vis))*(1.0f-temp)*PI;
        /* updated visibility probability */
        *(aux_args.ray_vis) *= temp;

        //discretize and store beta and vis contribution
        int beta_int = convert_int_rte(ray_beta * SEGLEN_FACTOR);
        atom_add(&aux_args.beta_array[data_ptr], beta_int);
        int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR);
        atom_add(&aux_args.vis_array[data_ptr], vis_int);
    }
    //-------------------------------------------------------------------------- */
}
#endif // BAYES_CUBIC

#ifdef POST_CUBIC
//bayes step cell functor
void step_cell_post_cubic(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);
    //load aux data
    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;
    float seg_len = d*aux_args.linfo->block_len;
    float cell_post, vis_cont;
    float PI = 0.0;
    /* Compute PI for all threads */
    if (seg_len > 1.0e-10f) {    /* if  too small, do nothing */
        PI = cubic_prob_density( mean_obs,aux_args.phi,mixture);
    }
    //calculate this ray's contribution to beta
    float temp  = exp(-alpha * seg_len);
    *(aux_args.ray_pre) += (* aux_args.ray_vis)*(1-temp)*1.0;
    //float ratio = (aux_args.norm/(* aux_args.ray_pre) - 1);
    //ratio = ratio > 10? 10 : ratio;
    //cell_post = ratio*d;

    float post = aux_args.norm - *(aux_args.ray_pre);
    //ratio = ratio > 10? 10 : ratio;
    cell_post = post *d;

    vis_cont  = (* aux_args.ray_vis) * d;
    (* aux_args.ray_vis) *= temp;

    //discretize and store beta and vis contribution
    int post_int = convert_int_rte(cell_post * SEGLEN_FACTOR);
    atom_add(&aux_args.beta_array[data_ptr], post_int);
    int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
}
#endif // POST_CUBIC

#ifdef POST_CUBIC
//bayes step cell functor
void step_cell_post_cubic(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);
    //load aux data
    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;
    float seg_len = d*aux_args.linfo->block_len;
    float cell_post, vis_cont;
    float PI = 0.0;
    /* Compute PI for all threads */
    if (seg_len > 1.0e-10f) {    /* if  too small, do nothing */
        PI = cubic_prob_density( mean_obs,aux_args.phi,mixture);
    }
    //calculate this ray's contribution to beta
    float temp  = exp(-alpha * seg_len);
    *(aux_args.ray_pre) += (* aux_args.ray_vis)*(1-temp)*1.0;
    //float ratio = (aux_args.norm/(* aux_args.ray_pre) - 1);
    //ratio = ratio > 10? 10 : ratio;
    //cell_post = ratio*d;

    float post = aux_args.norm - *(aux_args.ray_pre);
    //ratio = ratio > 10? 10 : ratio;
    cell_post = post *d;

    vis_cont  = (* aux_args.ray_vis) * d;
    (* aux_args.ray_vis) *= temp;

    //discretize and store beta and vis contribution
    int post_int = convert_int_rte(cell_post * SEGLEN_FACTOR);
    atom_add(&aux_args.beta_array[data_ptr], post_int);
    int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
}
#endif // POST_CUBIC

#ifdef AVG_SURFACE_EMPTY_RATIO
//bayes step cell functor
void step_cell_avg_ratio_cubic(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);
    //load aux data
    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];

    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;

    float seg_len = d*aux_args.linfo->block_len;

    float cell_post, vis_cont;

    float PI = 0.0;
    /* Compute PI for all threads */
    if (seg_len > 1.0e-10f) {    /* if  too small, do nothing */
        PI = cubic_prob_density( mean_obs,aux_args.phi,mixture);
    }
    //calculate this ray's contribution to beta
#ifdef INDEPENDENT
    float temp  = exp(-alpha * seg_len);
    float numer = *(aux_args.ray_pre) +  (* aux_args.ray_vis) * PI;
    cell_post = (numer/(aux_args.pre_inf +aux_args.vis_inf)) *d;

    (* aux_args.ray_pre) += (* aux_args.ray_vis)*(1-temp)*PI ;
    (* aux_args.ray_vis) *= temp;
#endif

#ifdef JOINT
    //calculate this ray's contribution to beta
    float temp  = exp(-alpha * seg_len);
    float numer = *(aux_args.ray_pre) +  (* aux_args.ray_vis) * PI;
    float denom = *(aux_args.ray_pre)*temp +  (aux_args.pre_inf - *(aux_args.ray_pre) - (* aux_args.ray_vis)*(1-temp)*PI );// + aux_args.vis_inf*1.0 ;

    cell_post = (numer/denom) *d;
    vis_cont  = (aux_args.vis_inf/numer) * d;
    (* aux_args.ray_pre) += (* aux_args.ray_vis)*(1-temp)*PI ;
    (* aux_args.ray_vis) *= temp;
#endif

    //discretize and store beta and vis contribution
    int post_int = convert_int_rte(cell_post * SEGLEN_FACTOR);
    atom_add(&aux_args.beta_array[data_ptr], post_int);
    int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
}
#endif // AVG_SURFACE_EMPTY_RATIO
