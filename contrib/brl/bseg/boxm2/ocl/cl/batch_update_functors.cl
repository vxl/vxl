#ifdef SEGLENNOBS

//Update step cell functor::seg_len
void step_cell_seglen_nobs(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //SLOW and accurate method
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);
    int cum_obs = convert_int_rte(d * aux_args.obs * SEGLEN_FACTOR);
    atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
    atom_inc(&aux_args.nobs[data_ptr]);
}
#endif //SEGLENNOBS

#ifdef AUX_PREVISPOST
void step_cell_aux_previspost(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];
    float8 mixture  = convert_float8(aux_args.mog[data_ptr])/(float)NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);

    //load aux data
    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float_rte(mean_int) / convert_float_rte(cum_int);
    float cum_len = convert_float_rte(cum_int) / SEGLEN_FACTOR;

    float PI=0.0f;
    // Compute PI for all threads
    if (cum_len * aux_args.linfo->block_len > 1.0e-10f) { // if  too small, do nothing
        PI = gauss_3_mixture_prob_density(mean_obs,
                                          mixture.s0,
                                          mixture.s1,
                                          mixture.s2,
                                          mixture.s3,
                                          mixture.s4,
                                          mixture.s5,
                                          mixture.s6,
                                          mixture.s7,
                                          weight3 );

        float temp  = exp(-alpha * d * aux_args.linfo->block_len);

        //calculate this cell's vis, pre and post.
        float cell_vis, cell_pre, cell_post;
        cell_vis = (* aux_args.ray_vis) * d;
        cell_pre = (* aux_args.ray_pre) * d;
        cell_post= (* aux_args.pre_inf) - (* aux_args.ray_pre) - (* aux_args.ray_vis)*(1-temp)*PI;
        cell_post /= temp;
        cell_post += (* aux_args.vis_inf)*1; //appearance model at infinity is uniform
        cell_post *= d;

        float vis_prob_end = (* aux_args.ray_vis) * temp;
        (* aux_args.ray_pre) += ((* aux_args.ray_vis) - vis_prob_end) *  PI;
        (* aux_args.ray_vis) = vis_prob_end;

        //discretize and store pre, vis and post contributions
        int pre_int = convert_int_rte(cell_pre * SEGLEN_FACTOR);
        atom_add(& aux_args.pre_array[data_ptr], pre_int);
        int vis_int  = convert_int_rte(cell_vis * SEGLEN_FACTOR);
        atom_add(& aux_args.vis_array[data_ptr], vis_int);
        int post_int  = convert_int_rte(cell_post * SEGLEN_FACTOR);
        atom_add(& aux_args.post_array[data_ptr], post_int);
        //--------------------------------------------------------------------------
     }
}
#endif // AUX_PREVISPOST

#ifdef AUX_PREVIS
//bayes step cell functor
void step_cell_aux_previs(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];
    float8 mixture  = convert_float8(aux_args.mog[data_ptr])/(float)NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);

    //load aux data
    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR;
    float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
    mean_obs = mean_obs/cum_len;

    float cell_vis, cell_pre;
    float PI=0.0f;
    // Compute PI for all threads
    if (cum_len > 1.0e-10f) {    // if  too small, do nothing
        PI = gauss_3_mixture_prob_density(mean_obs,
                                          mixture.s0,
                                          mixture.s1,
                                          mixture.s2,
                                          mixture.s3,
                                          mixture.s4,
                                          mixture.s5,
                                          mixture.s6,
                                          mixture.s7,
                                          weight3 );
    }

    //calculate this ray's contribution to beta
    cell_vis = (* aux_args.ray_vis) * d;
    cell_pre = (* aux_args.ray_pre) * d;

    //update ray_pre and ray_vis
    float temp  = exp(-alpha * d * aux_args.linfo->block_len); // blovk_len is multiplied so that alpha is computed with respect to the scale.
    // updated pre                      Omega         *  PI
    (* aux_args.ray_pre) += (* aux_args.ray_vis)*(1.0f-temp)*PI;
    // updated visibility probability
    (* aux_args.ray_vis) *= temp;

    //discretize and store beta and vis contribution
    int pre_int = convert_int_rte(cell_pre * SEGLEN_FACTOR);
    atom_add(&aux_args.pre_array[data_ptr], pre_int);
    int vis_int  = convert_int_rte(cell_vis * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
    //--------------------------------------------------------------------------
}
#endif // AUX_PREVIS
#ifdef AUX_LEN_INT_VIS
//bayes step cell functor
void step_cell_aux_len_int_vis(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);
    int cum_obs = convert_int_rte(d * aux_args.obs * SEGLEN_FACTOR);
    atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
    float vis = *(aux_args.ray_vis);

    int vis_int  = convert_int_rte(d*vis * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
    //update  ray_vis
    float  alpha    = aux_args.alpha[data_ptr];
    float temp  = exp(-alpha * d * aux_args.linfo->block_len); // blovk_len is multiplied so that alpha is computed with respect to the scale.
    // updated visibility probability
    *(aux_args.ray_vis) =vis*temp;

    //--------------------------------------------------------------------------
}
#endif
#ifdef UPDATE_AUX_DIRECTION
//bayes step cell functor
void step_cell_directions(AuxArgs aux_args, int data_ptr, float d)
{
    //slow beta calculation ----------------------------------------------------
    int len_int = convert_int_rte( d * SEGLEN_FACTOR);
    atom_add(&aux_args.len[data_ptr], len_int);

    int X_int = convert_int_rte(aux_args.xdir*d * SEGLEN_FACTOR);
    atom_add(&aux_args.X[data_ptr], X_int);

    int Y_int = convert_int_rte(aux_args.ydir*d * SEGLEN_FACTOR);
    atom_add(&aux_args.Y[data_ptr], Y_int);

    int Z_int = convert_int_rte(aux_args.zdir*d * SEGLEN_FACTOR);
    atom_add(&aux_args.Z[data_ptr], Z_int);
    //--------------------------------------------------------------------------
}

#endif // AUX_PREVIS
