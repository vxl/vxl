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
    /* Compute PI for all threads */
    if (cum_len > 1.0e-10f) {    /* if  too small, do nothing */
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
    float temp  = exp(-alpha * d);
    /* updated pre                      Omega         *  PI         */
    (* aux_args.ray_pre) += (* aux_args.ray_vis)*(1.0f-temp)*PI;
    /* updated visibility probability */
    (* aux_args.ray_vis) *= temp;

    //discretize and store beta and vis contribution
    int pre_int = convert_int_rte(cell_pre * SEGLEN_FACTOR);
    atom_add(&aux_args.pre_array[data_ptr], pre_int);
    int vis_int  = convert_int_rte(cell_vis * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
    //-------------------------------------------------------------------------- */

}
#endif // BAYES