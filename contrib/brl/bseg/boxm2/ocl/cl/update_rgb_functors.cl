#ifdef SEGLEN
/* sums up the ray contribution to the segment length and mean obs for each cell */
void seg_len_obs_rgb_functor(      float    seg_len,
                                   float4   obs,
                           __local short2 * ray_bundle_array,
                           __local float4 * cached_aux_data)
{
    /* linear thread id */
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

    /* limit access to threads that do not own a connected region */
    if (ray_bundle_array[llid].y == 0)
    {
        /* store seg_len in the corresponding aux data slot to be accessed
        * by other threads since these aux_data items are not used, they
        * can be cleared and used to store the seg_len of non-owner rays
        */
        cached_aux_data[llid] = (float4) (seg_len, seg_len*obs.x, seg_len*obs.y, seg_len*obs.z);
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    /* now, limit access to the threads that own each connected region */
    if (ray_bundle_array[llid].y == 1)
    {
        /* The region owner (base) is now the only active thread within the region*/
        /* process the base ray */
        cached_aux_data[llid].x += seg_len;                       // seg_len sum
        cached_aux_data[llid].y += obs.x*seg_len;                   // weighted observations */
        cached_aux_data[llid].z += obs.y*seg_len;                   // weighted observations */
        cached_aux_data[llid].w += obs.z*seg_len;                   // weighted observations */

        /* traverse the linked list and increment sums */
        short next = ray_bundle_array[llid].x;  // linked list pointer
        while(next >= 0) {
            cached_aux_data[llid].x += cached_aux_data[next].x;
            cached_aux_data[llid].y += cached_aux_data[next].y;
            cached_aux_data[llid].z += cached_aux_data[next].z;
            cached_aux_data[llid].w += cached_aux_data[next].w;
            next = ray_bundle_array[next].x;
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}



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
    seg_len_obs_rgb_functor(d, aux_args.obs, aux_args.ray_bundle_array, aux_args.cached_aux);
    barrier(CLK_LOCAL_MEM_FENCE);

    //set aux data here (for each leader.. )
    if (aux_args.ray_bundle_array[llid].y==1)
    {
        //scale!
        int seg_int = convert_int_rte(aux_args.cached_aux[llid].x * SEGLEN_FACTOR);
        int cum_obsR = convert_int_rte(aux_args.cached_aux[llid].y * SEGLEN_FACTOR);
        int cum_obsG = convert_int_rte(aux_args.cached_aux[llid].z * SEGLEN_FACTOR);
        int cum_obsB = convert_int_rte(aux_args.cached_aux[llid].w * SEGLEN_FACTOR);


        //atomically update the cells
        atom_add(&aux_args.seg_len[data_ptr], seg_int);
        atom_add(&aux_args.mean_obsR[data_ptr], cum_obsR);
        atom_add(&aux_args.mean_obsG[data_ptr], cum_obsG);
        atom_add(&aux_args.mean_obsB[data_ptr], cum_obsB);
    }
    //------------------------------------------------------------------------
#else
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
#endif

    //reset cell_ptrs to negative one every time (prevents invisible layer bug)
    aux_args.cell_ptrs[llid] = -1;
}
#endif // SEGLEN

#ifdef PREINF
//preinf step cell functor
void step_cell_preinf(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    //cell data, i.e., alpha and app model is needed for some passes
    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;

    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR;
    float4 meanObs = convert_float4(as_uchar4(aux_args.mean_obs[data_ptr]))/255.0f;

    //calculate pre_infinity denomanator (shape of image)
    // if total length of rays is too small, do nothing
    float PI = 0.0f;
    if (cum_len>1.0e-10f)
    {
        /* The mean intensity for the cell */
        PI = gauss_prob_density(meanObs.x, mixture.s0, mixture.s4);  //YUV edit
        //PI = gauss_prob_density_rgb(meanObs, mixture.s0123, mixture.s4567);
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
/* Image vector
 *  [norm | alpha_integral | vis | pre ] */
void bayes_ratio_rgb_functor(         float   seg_len,          // segment length for this ray
                                      float4  mean_obs,         // mean observation for the currently intersected cell
                                      float * ray_pre,              // ray pre
                                      float * ray_vis,              // ray vis
                                      float   norm,             // normalization (pre_inf + vis_inf)
                                      float * cell_beta,
                                      float * cell_vis,
                              __local short2* ray_bundle_array,
                              __local float*  cached_beta,
                              __local float*  cached_vis,
                                      float   alpha,
                                      float8  mixture,
                                      float   weight3)
{
    // linear thread id
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

    // Compute PI for all threads
    float PI = 0.0f;
    if (seg_len>1.0e-10f)
    {
        // The mean intensity for the cell
        PI = gauss_prob_density_rgb( mean_obs, mixture.s0123, mixture.s4567);
    }

    //cache current vis and current beta
    if( seg_len>1e-10f ) {
        cached_beta[llid] = ((*ray_pre) + PI*(*ray_vis))*seg_len/norm;
        cached_vis[llid] =  (*ray_vis) * seg_len;
    }
    else {
        cached_beta[llid] = 0.0f;
        cached_vis[llid] = 0.0f;
    }
    barrier(CLK_LOCAL_MEM_FENCE); /*wait for all threads to complete */

    /* region owner scans the region and computes Bayes ratio and weighted vis.
    * The aux_data cell elements are assigned as:
    *
    *   [seg_len sum | weighted obs | update_ratio (beta) | weighted vis]
    */
    if (ray_bundle_array[llid].y==1) {

        /* compute data for base ray cell */
        /* traverse the linked list and increment sums */
        short curr = convert_short(llid); //ray_bundle_array[llid].x;  // linked list pointer
        while(curr >= 0) {

            /* Bayes ratio */
            /* ( pre + PI*vis)/norm)*seg_len */
            (*cell_beta) += cached_beta[curr];
            (*cell_vis)  += cached_vis[curr];
            curr = ray_bundle_array[curr].x;
        }
    }

    //update ray pre and vis
    float temp = exp(-alpha * seg_len);
    (*ray_pre) += (*ray_vis)*(1-temp)*PI;
    (*ray_vis) *= temp;

    // wait for all threads to complete
    barrier(CLK_LOCAL_MEM_FENCE);
}

/* Image vector */
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
        //PI = gauss_prob_density_rgb( mean_obs, mixture.s0123, mixture.s4567);
        PI = gauss_prob_density(mean_obs.x, mixture.s0, mixture.s4);  //YUV edit

        (*cell_beta) = ((*ray_pre) + PI*(*ray_vis))*seg_len/norm;
        (*cell_vis) = (*ray_vis) * seg_len;
    }

    //update ray pre and vis
    float temp = exp(-alpha * seg_len);
    (*ray_pre) += (*ray_vis)*(1-temp)*PI;
    (*ray_vis) *= temp;
}


//bayes step cell functor
void step_cell_bayes(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //if this current thread is a segment leader...
    //cell data, i.e., alpha and app model is needed for some passes
    float  alpha    = aux_args.alpha[data_ptr];
    CONVERT_FUNC_FLOAT8(mixture,aux_args.mog[data_ptr])/NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);

    //load aux data
    float4 meanObs = convert_float4(as_uchar4(aux_args.mean_obs[data_ptr]))/255.0f;

#ifdef ATOMIC_OPT
    //keep track of cells being hit
    aux_args.cell_ptrs[llid] = data_ptr;
    barrier(CLK_LOCAL_MEM_FENCE);
    load_data_mutable_opt(aux_args.ray_bundle_array, aux_args.cell_ptrs);

    float cell_beta = 0.0f;
    float cell_vis  = 0.0f;
    barrier(CLK_LOCAL_MEM_FENCE);

    //calculate bayes ratio
    bayes_ratio_rgb_functor(d * aux_args.linfo->block_len,
                            meanObs,
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
    //-------------------------------------------------------------------------- */
#endif

    //reset cell_ptrs to -1 every time
    aux_args.cell_ptrs[llid] = -1;
}
#endif // BAYES


#ifdef SEGLEN_VIS
//Update step cell functor::seg_len_vis
void step_cell_seglen_vis(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    float alpha    = aux_args.alpha[data_ptr];
    float diff_omega = exp(-alpha * d * aux_args.linfo->block_len);
    float vis_prob_end = (*aux_args.vis_inf) * diff_omega;

    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);

    //increment mean observation
    int cum_obsR = convert_int_rte(d * aux_args.obs.x * SEGLEN_FACTOR);
    int cum_obsG = convert_int_rte(d * aux_args.obs.y * SEGLEN_FACTOR);
    int cum_obsB = convert_int_rte(d * aux_args.obs.z * SEGLEN_FACTOR);
    int cum_vis = convert_int_rte(d * (*aux_args.vis_inf) * SEGLEN_FACTOR);

    atom_add(&aux_args.mean_obsR[data_ptr], cum_obsR);
    atom_add(&aux_args.mean_obsG[data_ptr], cum_obsG);
    atom_add(&aux_args.mean_obsB[data_ptr], cum_obsB);
    atom_add(&aux_args.mean_vis[data_ptr], cum_vis);

    *aux_args.vis_inf = vis_prob_end;

    //dec: copy/pasting the line below.. not sure if its needed here
    //reset cell_ptrs to negative one every time (prevents invisible layer bug)
    aux_args.cell_ptrs[llid] = -1;
}
#endif // SEGLEN_VIS
