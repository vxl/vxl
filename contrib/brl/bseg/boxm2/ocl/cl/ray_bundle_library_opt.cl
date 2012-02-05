/* ray_bundle_library_opt.cl
 * Optimized ray bundle library functions -
 *
 * Handles workgroup segmentation (needed to update only one block at a time
 * during raytrace)
 *
 * Implements seg_len_obs pass, pre_infinity pass, and bayes_update pass
 *
 * NOTE: update seg_len, pre_inf and bayes_update to take on more args and
 * become the step cell functor
 *
 * NOTE: There may be a bug in Pre Inf and Bayes Update because of the thresholding
 * if statement precludes first calculation, but not second
 */


/* Load Data Mutable Opt
 * This optimized version of the function above serves to replace it with
 * simpler data structures, and a more parallel algorithm. The current
 * idea is find the connected components in parallel, where segmentation data
 * is stored in a short2 array (one for each ray in the work group, as follows:
 *    x        y
 *  [next_ptr   | is_leader]
 *
 *    o  next_ptr points to the LLID of the following ray cast into the same cell
 *       (or it is -1 if there is no such ray).
 *    o  is_leader denotes whether other not this particular ray is the owner
 *       of the segment - if it is, it will be responsible for writing to the
 *       data cell that it's segment points to.
 *
 * It will be the job of the step cell functor (seg len, bayes update, etc) to
 * use the ray_bundle_array to iterate through each segment, calculating the
 * statistic, and writing it out to global mem.
 *
 * EDIT: What if ray doesn't hit anything - that is cell_ptrs is less than zero
 * (presumably the ray would have already been killed - but what happens to
 * that slot?  Is this what the ACTIVE tag did?) This shouldn't matter if the
 * inactive rays (broken rays) are just never assigned as a leader, nor are
 * pointed to by any other segment.
 *
 */
int load_data_mutable_opt(__local short2  * ray_bundle_array,
                          __local int     * cell_ptrs)
{
  uchar nbi = (uchar)get_local_size(0);   //num cols (size of X dimension)
  uchar nbj = (uchar)get_local_size(1);   //num rows (size of Y dimension)
  uchar lsize = nbi*nbj;                  //local workgroup size
  uchar col = (uchar)get_local_id(0);     //thread col
  uchar row = (uchar)get_local_id(1);     //thread row
  uchar llid = (col + nbi*row);           //[0-nbi*nbj] index

  //initialize segmentation information (if the ray isn't hitting a cell, make it a non-leader
  if (cell_ptrs[llid] >= 0)
    ray_bundle_array[llid] = (short2) (-1, 1);
  else
    ray_bundle_array[llid] = (short2) (-1, 0);
  barrier(CLK_LOCAL_MEM_FENCE);

#if 0
  //first pass looks to the right neighbor (make sure you don't look for the last one)
  if (cell_ptrs[llid] == cell_ptrs[llid+1] && llid<lsize-1) {
    ray_bundle_array[llid].x   = llid+1;      //NEXT pointer = llid+1
    ray_bundle_array[llid+1].y = 0;           //NEXT node is not a leader
  }
#endif // 0

  //make pass that looks through the right
  if (ray_bundle_array[llid].x == -1) {
    int start = llid+1;
    int end   = lsize;
    for (int i=start; i<end; ++i) {
      if (cell_ptrs[llid] == cell_ptrs[i]) {
        ray_bundle_array[llid].x = i;         //NEXT pointer = i;
        ray_bundle_array[i].y    = 0;         //i is not a leader anymore
        break;
      }
    }
  }

  barrier(CLK_LOCAL_MEM_FENCE);

#if 0
  //second pass - start at the next row (don't look in the last row)
  if (ray_bundle_array[llid].x == -1 && row<nbi-1) {

    //next row index
    int start = llid+1; //(row+1)*nbi;
    int end   = lsize; //start + col+1;
    for (int i=start; i<end; ++i) {
      if (cell_ptrs[llid] == cell_ptrs[i]) {
        ray_bundle_array[llid].x = i;         //NEXT pointer = i;
        ray_bundle_array[i].y    = 0;         //i is not a leader anymore
        break;
      }
    }
  }
#endif // 0
  return 1;
}


/* sums up the ray contribution to the segment length and mean obs for each cell */
void seg_len_obs_functor(          float    seg_len,
                                   float    obs,
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
        cached_aux_data[llid] = (float4) (seg_len, seg_len*obs, 0.0f, 0.0f);
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    /* now, limit access to the threads that own each connected region */
    if (ray_bundle_array[llid].y == 1)
    {
        /* The region owner (base) is now the only active thread within the region*/
        /* process the base ray */
        cached_aux_data[llid].x += seg_len;                       // seg_len sum
        cached_aux_data[llid].y += obs*seg_len;                   // weighted observations */

        /* traverse the linked list and increment sums */
        short next = ray_bundle_array[llid].x;  // linked list pointer
        while (next >= 0) {
            cached_aux_data[llid].x += cached_aux_data[next].x;
            cached_aux_data[llid].y += cached_aux_data[next].y;
            next = ray_bundle_array[next].x;
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
}

/*
*  Accumulate the pre and vis image arrays based on the cell data.
*  A vector of images is maintained image_vect that is updated as the rays
*  step through the volume. The image vector is assigned as:
*
*  [obs | alpha_integral | vis | pre ]
*
*/
void pre_infinity_opt(  float    seg_len,
                        float    cum_len,
                        float    mean_obs,
                        float  * vis_inf,
                        float  * pre_inf,
                        float    alpha,
                        float8   mixture,
                        float    weight3)
{
    /* if total length of rays is too small, do nothing */
    float PI = 0.0f;
    if (cum_len>1.0e-10f)
    {
        /* The mean intensity for the cell */
        PI = gauss_3_mixture_prob_density( mean_obs,
                                           mixture.s0,
                                           mixture.s1,
                                           mixture.s2,
                                           mixture.s3,
                                           mixture.s4,
                                           mixture.s5,
                                           mixture.s6,
                                           mixture.s7,
                                           weight3 //(1.0f-mixture.s2-mixture.s5)
                                          );/* PI */


    /* Calculate pre and vis infinity */
    float diff_omega = exp(-alpha * seg_len);
    float vis_prob_end = (*vis_inf) * diff_omega;

    /* updated pre                      Omega         *   PI  */
    (*pre_inf) += ((*vis_inf) - vis_prob_end) *  PI;

    /* updated visibility probability */
    (*vis_inf) = vis_prob_end;
  }
}


/* Image vector
 *  [norm | alpha_integral | vis | pre ] */
void bayes_ratio_functor(         float   seg_len,          // segment length for this ray
                                  float   mean_obs,         // mean observation for the currently intersected cell
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
    float PI = 0.0;
    if (seg_len > 1.0e-10f) {
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
    //cache current vis and current beta
    if ( seg_len>1e-10f ) {
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
        while (curr >= 0)
        {
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

/* bayes ratio independent functor (for independent rays) */
void bayes_ratio_ind( float  seg_len,
                      float  alpha,
                      float8 mixture,
                      float  weight3,
                      float  cum_len,
                      float  mean_obs,
                      float  norm,
                      float* ray_pre,
                      float* ray_vis,
                      float* ray_beta,
                      float* vis_cont )
{
    float PI = 0.0;

    /* Compute PI for all threads */
    if (seg_len > 1.0e-10f) {    /* if  too small, do nothing */
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
    (*ray_beta) = ((*ray_pre) + PI*(*ray_vis))*seg_len/norm;
    (*vis_cont) = (*ray_vis) * seg_len;

    //update ray_pre and ray_vis
    float temp  = exp(-alpha * seg_len);

    /* updated pre                      Omega         *  PI         */
    (*ray_pre) += (*ray_vis)*(1.0f-temp)*PI;//(image_vect[llid].z - vis_prob_end) * PI;
    /* updated visibility probability */
    (*ray_vis) *= temp;
}


/* bayes ratio independent functor (for independent rays) */
void compute_post_ind( float  seg_len, float block_len,
                       float  alpha,
                       float8 mixture,
                       float  weight3,
                       float  cum_len,
                       float  mean_obs,
                       float  norm,
                       float* ray_pre,
                       float* ray_vis,
                       float* ray_beta,
                       float* vis_cont )
{
    float PI = 0.0;

    /* Compute PI for all threads */
    if (seg_len > 1.0e-10f) {    /* if  too small, do nothing */
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
    float temp  = exp(-alpha * seg_len*block_len);
    (*ray_pre) += (*ray_vis)*(1.0f-temp)*PI;
    (*ray_beta) = ( (*ray_pre)/norm )*seg_len;
    (*vis_cont) = (*ray_vis) * seg_len;
    (*ray_vis) *= temp;
}

/* Aux Data = [cell_len, mean_obs*cell_len, beta, cum_vis]  */
void update_cell(float16 * data, float4 aux_data,float t_match, float init_sigma, float min_sigma)
{
    float mu0 = (*data).s1, sigma0 = (*data).s2, w0 = (*data).s3;
    float mu1 = (*data).s5, sigma1 = (*data).s6, w1 = (*data).s7;
    float mu2 = (*data).s9, sigma2 = (*data).sa;
    float w2=0.0f;


    if (w0>0.0f && w1>0.0f)
        w2=1-(*data).s3-(*data).s7;

    short Nobs0 = (short)(*data).s4, Nobs1 = (short)(*data).s8, Nobs2 = (short)(*data).sb;
    float Nobs_mix = (*data).sc;

    update_gauss_3_mixture(aux_data.y,              //mean observation
                           aux_data.w,              //cell_visability
                           t_match,
                           init_sigma,min_sigma,
                           &mu0,&sigma0,&w0,&Nobs0,
                           &mu1,&sigma1,&w1,&Nobs1,
                           &mu2,&sigma2,&w2,&Nobs2,
                           &Nobs_mix);

    float beta = aux_data.z; //aux_data.z/aux_data.x;
    clamp(beta,0.5f,2.0f);
    (*data).s0 *= beta;
    (*data).s1=mu0; (*data).s2=sigma0, (*data).s3=w0;(*data).s4=(float)Nobs0;
    (*data).s5=mu1; (*data).s6=sigma1, (*data).s7=w1;(*data).s8=(float)Nobs1;
    (*data).s9=mu2; (*data).sa=sigma2, (*data).sb=(float)Nobs2;
    (*data).sc=(float)Nobs_mix;
}


