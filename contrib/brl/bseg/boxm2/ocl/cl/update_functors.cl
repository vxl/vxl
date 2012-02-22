
#ifdef SEGLEN
//Update step cell functor::seg_len
void step_cell_seglen(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
#ifdef ATOMIC_OPT
    // --------- faster and less accurate method... --------------------------
    //keep track of cells being hit
    aux_args.cell_ptrs[llid] = data_ptr;
    aux_args.cached_aux[llid] = (float4) 0.0f;  //leaders retain the mean obs and the cell length
    barrier(CLK_LOCAL_MEM_FENCE);

    //segment workgroup
    load_data_mutable_opt(aux_args.ray_bundle_array,aux_args.cell_ptrs);

    //back to normal mean of mean obs...
    seg_len_obs_functor(d, aux_args.obs, aux_args.ray_bundle_array, aux_args.cached_aux);
    barrier(CLK_LOCAL_MEM_FENCE);

    //set aux data here (for each leader.. )
    if (aux_args.ray_bundle_array[llid].y==1)
    {
        //scale!
        int seg_int = convert_int_rte(aux_args.cached_aux[llid].x * SEGLEN_FACTOR);
        int cum_obs = convert_int_rte(aux_args.cached_aux[llid].y * SEGLEN_FACTOR);

        //atomically update the cells
        atom_add(&aux_args.seg_len[data_ptr], seg_int);
        atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
    }
    //reset cell_ptrs to negative one every time (prevents invisible layer bug)
    aux_args.cell_ptrs[llid] = -1;
    //------------------------------------------------------------------------
#else
    //SLOW and accurate method
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);
    int cum_obs = convert_int_rte(d * aux_args.obs * SEGLEN_FACTOR);
    atom_add(&aux_args.mean_obs[data_ptr], cum_obs);

#ifdef DEBUG
    (*aux_args.ray_len) += d;
#endif

#endif
}
#endif // SEGLEN

#ifdef PREINF
//preinf step cell functor
void step_cell_preinf(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    ////cell data, i.e., alpha and app model is needed for some passes
    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float  weight3  = (1.0f-mixture.s2-mixture.s5);

    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;

    //calculate pre_infinity denomanator (shape of image)
    pre_infinity_opt( d*aux_args.linfo->block_len,
                      cum_len*aux_args.linfo->block_len,
                      mean_obs,
                      aux_args.vis_inf,
                      aux_args.pre_inf,
                      alpha,
                      mixture,
                      weight3);
}
#endif // PREINF

#ifdef BAYES
//bayes step cell functor
void step_cell_bayes(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
#ifdef ATOMIC_OPT
    //keep track of cells being hit
    aux_args.cell_ptrs[llid] = data_ptr;
    barrier(CLK_LOCAL_MEM_FENCE);
    load_data_mutable_opt(aux_args.ray_bundle_array, aux_args.cell_ptrs);

    //if this current thread is a segment leader...
    //cell data, i.e., alpha and app model is needed for some passes
    float  alpha    = aux_args.alpha[data_ptr];
    //float8 mixture  = convert_float8(aux_args.mog[data_ptr])/(float)NORM;
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);

    //load aux data
    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR;
    float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
    mean_obs = mean_obs/cum_len;
    aux_args.linfo->block_len;
    float cell_beta = 0.0f;
    float cell_vis  = 0.0f;
    barrier(CLK_LOCAL_MEM_FENCE);

    //calculate bayes ratio
    bayes_ratio_functor(d,
#if 0
                        *aux_args.linfo->block_len,
#endif
                        mean_obs,
                        aux_args.ray_pre,
                        aux_args.ray_vis,
                        aux_args.norm,
                        &cell_beta,
                        &cell_vis,
                        aux_args.ray_bundle_array,
                        aux_args.cell_ptrs,
                        aux_args.cached_vis,
                        alpha,
                        mixture,
                        weight3);

    //set aux data here (for each leader.. )
    if (aux_args.ray_bundle_array[llid].y==1)
    {
        int beta_int = convert_int_rte(cell_beta * SEGLEN_FACTOR);
        atom_add(&aux_args.beta_array[data_ptr], beta_int);
        int vis_int  = convert_int_rte(cell_vis * SEGLEN_FACTOR);
        atom_add(&aux_args.vis_array[data_ptr], vis_int);
    }
#else
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);

    //load aux data
    int cum_int = aux_args.seg_len[data_ptr];
    int mean_int = aux_args.mean_obs[data_ptr];
    float mean_obs = convert_float(mean_int) / convert_float(cum_int);
    float cum_len = convert_float(cum_int) / SEGLEN_FACTOR;

    float ray_beta, vis_cont;
    bayes_ratio_ind( d*aux_args.linfo->block_len,
                     alpha,
                     mixture,
                     weight3,
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

#if 0
    //debug expected int along ray
    *(aux_args.outInt) += mean_obs*vis_cont;
#endif
    //-------------------------------------------------------------------------- */
#endif

    //reset cell_ptrs to -1 every time
    aux_args.cell_ptrs[llid] = -1;
}
#endif // BAYES

#ifdef UPDATE_HIST
//bayes step cell functor
void step_cell_update_hist(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //slow beta calculation ----------------------------------------------------
    float  alpha    = aux_args.alpha[data_ptr];

    //load aux data
    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR;
    float obs = aux_args.obs;

    float vis=*(aux_args.vis);
    float upcount=d/cum_len* vis;

    int index1=(int)floor(obs*4);
    index1=min(index1,3);

    //discretize and store beta and vis contribution
    int upcount_int = convert_int_rte(upcount * SEGLEN_FACTOR);
    atom_add(&aux_args.hist[8*data_ptr+index1], upcount_int);

    int index2=(int)floor((obs-0.125)*3);
    vis=vis*exp(-alpha*d*aux_args.linfo->block_len);
    *(aux_args.vis)=vis;

    if (index2>=0 && index2<=2)
        atom_add(&aux_args.hist[8*data_ptr+4+index2], upcount_int);
      //atom_add(&aux_args.hist[8*data_ptr+7],        upcount_int);
}
#endif // UPDATE_HIST

#ifdef CUMLEN
void step_cell_cumlen(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //SLOW and accurate method
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);
}
#endif // CUMLEN


#ifdef INGEST_HEIGHT_MAP
void step_cell_ingest_height_map(AuxArgs aux_args, int data_ptr, float d)
{
    float alpha = - (log(1-0.999))/d;
    aux_args.alpha[data_ptr] = alpha;
}
#endif // INGEST_HEIGHT_MAP

#ifdef POST
//bayes step cell functor
void step_cell_post(AuxArgs aux_args, int data_ptr, uchar llid, float d)
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

    float ray_beta, vis_cont;
    compute_post_ind(d,aux_args.linfo->block_len,
                     alpha,
                     mixture,
                     weight3,
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
#endif // POST

#ifdef INGEST_BUCKEYE_DEM
#define Z_SIGMA 1.0
#define NEAR_ZERO 1e-5

void step_cell_ingest_buckeye_dem(AuxArgs aux_args, int data_ptr, float d0, float d1)
{
    float b = aux_args.belief[data_ptr];
    float u = aux_args.uncertainty[data_ptr];

    // probability first return lies within cell
    const float norm_const = 1.0/(sqrt(2.0)*Z_SIGMA);
    //const float norm_const2 = 1.0/(sqrt(2.0)*2*Z_SIGMA);
    // null case (first_depth > last_depth)
    //const float null_case_prob = 1.0 - 0.5 * (1.0 + erf((aux_args.last_depth - aux_args.first_depth)*norm_const2));
    // P(d0 < first_depth < d1)
    const float P1 = 0.5 * (erf((d1 - aux_args.first_depth)*norm_const) - erf((d0 - aux_args.first_depth)*norm_const));
    // P(d0 < last_depth < d1)
    const float P2 = 0.5 * (erf((d1 - aux_args.last_depth)*norm_const) - erf((d0 - aux_args.last_depth)*norm_const));


    const float b_obs =  P1 + P2 - P1*P2;
    // P(d1 < first_depth)
    const float d_obs1 = 1.0 - 0.5 * (1.0 + erf((d1 - aux_args.first_depth)*norm_const));
    const float d_obs2 = 1.0 - 0.5 * (1.0 + erf((d1 - aux_args.last_depth)*norm_const));
    const float d_obs =  d_obs1*d_obs2;
    const float u_obs = 1.0 - b_obs - d_obs;

    const float denom = u + u_obs - u*u_obs;

    // Perform Cumalitive Fusion on two subjective logic opinions
    if ((u_obs > NEAR_ZERO) || (u > NEAR_ZERO)) {
      b = (b*u_obs + b_obs*u)/denom;
      u = u*u_obs/denom;
    }
    else {
      b = 0.5*(b_obs + b);
      u = 0.0;
    }
    aux_args.belief[data_ptr] = b;
    aux_args.uncertainty[data_ptr] = u;
    //aux_args.belief[data_ptr] = aux_args.first_depth;
    //aux_args.uncertainty[data_ptr] = aux_args.last_depth;
}
#endif // INGEST_BUCKEYE_DEM

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
#endif // BAYES
