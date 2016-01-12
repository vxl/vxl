#ifdef AUX_PREVIS_NAA
//bayes step cell functor
void step_cell_aux_previs_naa(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    float  alpha = aux_args.alpha[data_ptr];
    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;

    //calculate pre_infinity denominator (shape of image)
    float seg_len_real = d*aux_args.linfo->block_len;
    float cum_len_real = cum_len*aux_args.linfo->block_len;

    // if total length of rays is too small, do nothing
    float PI = 0.0f;
    if (cum_len_real > 1.0e-10f)
    {
        // retrieve albedo and normal probabilities
        __global float16 *albedos = (__global float16*)&(aux_args.naa_apm[data_ptr*32]);
        __global float16 *normal_weights = (__global float16*)&(aux_args.naa_apm[data_ptr*32 + 16]);

        // compute mean and sigma of radiance value as linear function of albedo and albedo^2, respectively
        float16 radiance_predictions = *aux_args.radiance_reflectance_factors * (*albedos) + *aux_args.radiance_offsets;
        float16 radiance_variances = *aux_args.radiance_var_reflectance_sqrd_factors * (*albedos)*(*albedos) + *aux_args.radiance_var_offsets;
        int16 invalid = islessequal(radiance_variances,(float16)0);
        float16 radiance_sigmas = sqrt(radiance_variances);
        float16 prediction_densities = gauss_prob_density_f16(&radiance_predictions, mean_obs, &radiance_sigmas);
        // set prob. density to 0 for invalid normal directions
        prediction_densities = select(prediction_densities,(float16)0,invalid);
        // take weighted average based on surface normal probabilities
        PI = dot(prediction_densities, *normal_weights);
    }
    // store and update pre and vis
    float pass_prob = exp(-alpha * seg_len_real);
    float vis_prob_end = (*aux_args.ray_vis) * pass_prob;

    // compute this ray's contribution to beta
    float cell_vis = (*aux_args.ray_vis) * d;
    float cell_pre = (*aux_args.ray_pre) * d;

    // updated pre                      Omega         *  PI
    (* aux_args.ray_pre) += (* aux_args.ray_vis)*(1.0f-pass_prob)*PI;
    // updated visibility probability
    (* aux_args.ray_vis) *= pass_prob;

    //discretize and store beta and vis contribution
    int pre_int = convert_int_rte(cell_pre * SEGLEN_FACTOR);
    atom_add(&aux_args.pre_array[data_ptr], pre_int);

    int vis_int  = convert_int_rte(cell_vis * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
}
#endif // AUX_PREVIS_NAA


#ifdef PREINF_NAA

//preinf step cell functor for normal_albedo_array appearance model
void step_cell_preinf_naa(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    //cell data, i.e., alpha and app model is needed for some passes
    float  alpha = aux_args.alpha[data_ptr];

    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;

    //calculate pre_infinity denominator (shape of image)
    float seg_len_real = d*aux_args.linfo->block_len;
    float cum_len_real = cum_len*aux_args.linfo->block_len;

    // if total length of rays is too small, do nothing
    float PI = 0.0f;
    if (cum_len_real > 1.0e-10f)
    {
        // retrieve albedo and normal probabilities
        __global float16 *albedos = (__global float16*)&(aux_args.naa_apm[data_ptr*32]);
        __global float16 *normal_weights = (__global float16*)&(aux_args.naa_apm[data_ptr*32 + 16]);

        // compute mean and sigma of radiance value as linear function of albedo and albedo^2, respectively
        float16 radiance_predictions = *aux_args.radiance_reflectance_factors * (*albedos) + *aux_args.radiance_offsets;
        float16 radiance_variances = *aux_args.radiance_var_reflectance_sqrd_factors * (*albedos)*(*albedos) + *aux_args.radiance_var_offsets;
        int16 invalid = islessequal(radiance_variances,(float16)0);
        float16 radiance_sigmas = sqrt(radiance_variances);
        float16 prediction_densities = gauss_prob_density_f16(&radiance_predictions, mean_obs, &radiance_sigmas);
        // set prob. density to 0 for invalid normal directions
        prediction_densities = select(prediction_densities,(float16)0,invalid);
        // take weighted average based on surface normal probabilities
        PI = dot(prediction_densities, *normal_weights);
    }
    // Calculate pre and vis infinity
    float diff_omega = exp(-alpha * seg_len_real);
    float vis_prob_end = (*aux_args.vis_inf) * diff_omega;

    // updated pre                      Omega         *   PI
    (*aux_args.pre_inf) += ((*aux_args.vis_inf) - vis_prob_end) *  PI;

    // updated visibility probability
    (*aux_args.vis_inf) = vis_prob_end;
}

#endif // PREINF_NAA


#ifdef BAYES_NAA

//bayes step cell functor for normal-albedo-array model, only update alpha
void step_cell_bayes_naa(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha = aux_args.alpha[data_ptr];

    //load aux data
    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;

    float seg_len_real = d*aux_args.linfo->block_len;
    float cum_len_real = cum_len*aux_args.linfo->block_len;

    float PI = 0.0f;
    if (cum_len_real > 1.0e-10f)
    {
        // retrieve albedo and normal probabilities
        __global float16 *albedos = (__global float16*)&(aux_args.naa_apm[data_ptr*32]);
        __global float16 *normal_weights = (__global float16*)&(aux_args.naa_apm[data_ptr*32 + 16]);

        // compute mean and sigma of radiance value as linear function of albedo and albedo^2, respectively
        float16 radiance_predictions = *aux_args.radiance_reflectance_factors * (*albedos) + *aux_args.radiance_offsets;
        float16 radiance_variances = *aux_args.radiance_var_reflectance_sqrd_factors * (*albedos)*(*albedos) + *aux_args.radiance_var_offsets;
        int16 invalid = islessequal(radiance_variances,(float16)0);
        float16 radiance_sigmas = sqrt(radiance_variances);
        float16 prediction_densities = gauss_prob_density_f16(&radiance_predictions, mean_obs, &radiance_sigmas);
        // set prob. density to 0 for invalid normal directions
        prediction_densities = select(prediction_densities,(float16)0,invalid);
        // take weighted average based on surface normal probabilities
        PI = dot(prediction_densities, *normal_weights);
    }

    //calculate this ray's contribution to beta
    float ray_beta = ((*aux_args.ray_pre) + PI*(*aux_args.ray_vis))*seg_len_real/aux_args.norm;
    float vis_cont = (*aux_args.ray_vis) * seg_len_real;

    //update ray_pre and ray_vis
    float temp  = exp(-alpha * seg_len_real);

    // updated pre                      Omega         *  PI
    (*aux_args.ray_pre) += (*aux_args.ray_vis)*(1.0f-temp)*PI;
    // updated visibility probability
    (*aux_args.ray_vis) *= temp;

    //discretize and store beta and vis contribution
    int beta_int = convert_int_rte(ray_beta * SEGLEN_FACTOR);
    atom_add(&aux_args.beta_array[data_ptr], beta_int);
    int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR);
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
    //-------------------------------------------------------------------------- */

    //reset cell_ptrs to -1 every time
    aux_args.cell_ptrs[llid] = -1;
}
#endif // BAYES_NAA

