
//Update step cell functor::seg_len
#ifdef SEGLEN
void step_cell_seglen(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
#if 0
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
    if(aux_args.ray_bundle_array[llid].y==1) 
    {
      //scale!
      int seg_int = convert_int_rte(aux_args.cached_aux[llid].x * SEGLEN_FACTOR); 
      atom_add(&aux_args.seg_len[data_ptr], seg_int); 

      
      //atomically update the cells
      //atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
      //int cum_obs = convert_int_rte(aux_args.cached_aux[llid].y * SEGLEN_FACTOR);  

      //do cum obs for R G and B
      int cum_obsR = convert_int_rte(d * aux_args.obs.x * SEGLEN_FACTOR); 
      atom_add(&aux_args.mean_obsR[data_ptr], cum_obsR);
      int cum_obsG = convert_int_rte(d * aux_args.obs.y * SEGLEN_FACTOR); 
      atom_add(&aux_args.mean_obsG[data_ptr], cum_obsG);
      int cum_obsB = convert_int_rte(d * aux_args.obs.z * SEGLEN_FACTOR); 
      atom_add(&aux_args.mean_obsB[data_ptr], cum_obsB);
    }
    //------------------------------------------------------------------------ 
#else
    //SLOW and accurate method
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);  
    //int cum_obs = convert_int_rte(d * aux_args.obs * SEGLEN_FACTOR); 
    //atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
    int cum_obsR = convert_int_rte(d * aux_args.obs.x * SEGLEN_FACTOR); 
    atom_add(&aux_args.mean_obsR[data_ptr], cum_obsR);
    int cum_obsG = convert_int_rte(d * aux_args.obs.y * SEGLEN_FACTOR); 
    atom_add(&aux_args.mean_obsG[data_ptr], cum_obsG);
    int cum_obsB = convert_int_rte(d * aux_args.obs.z * SEGLEN_FACTOR); 
    atom_add(&aux_args.mean_obsB[data_ptr], cum_obsB);
#endif
  
    //reset cell_ptrs to negative one every time (prevents invisible layer bug)
    aux_args.cell_ptrs[llid] = -1;
}
#endif

#ifdef PREINF
//preinf step cell functor
void step_cell_preinf(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    //cell data, i.e., alpha and app model is needed for some passes 
    float  alpha    = aux_args.alpha[data_ptr];
    float8 mixture  = convert_float8(aux_args.mog[data_ptr])/(float)NORM;
    float  weight3  = (1.0f-mixture.s2-mixture.s5);
    
    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
    
    //float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
    //mean_obs = mean_obs/cum_len;
    
    uchar4 meanObs = as_uchar4(aux_args.mean_obs[data_ptr]); 
    float mean_obs = convert_float(meanObs.x)/255.0f; 
    //float4 mean_obs = convert_float4( as_uchar4(aux_args.mean_obs[data_ptr]) ) / 255.0f;  

   
    //calculate pre_infinity denomanator (shape of image)
    pre_infinity_opt(d, cum_len, mean_obs, aux_args.inImage, alpha, mixture, weight3);
}      
#endif

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
    float8 mixture  = convert_float8(aux_args.mog[data_ptr])/(float)NORM;
    float weight3   = (1.0f-mixture.s2-mixture.s5);
    
    //load aux data
    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
    //float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
    uchar4 meanObs = as_uchar4(aux_args.mean_obs[data_ptr]); 
    float mean_obs = convert_float(meanObs.x)/255.0f; 
    //mean_obs = mean_obs/cum_len;
    //float4 mean_obs = convert_float4( as_uchar4(aux_args.mean_obs[data_ptr]) ) / 255.0f;  
    
    float cell_beta = 0.0f;
    float cell_vis  = 0.0f;
    barrier(CLK_LOCAL_MEM_FENCE);

    //calculate bayes ratio
    bayes_ratio_functor( d, 
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
  if(aux_args.ray_bundle_array[llid].y==1) 
  {
    int beta_int = convert_int_rte(cell_beta * SEGLEN_FACTOR);
    atom_add(&aux_args.beta_array[data_ptr], beta_int);  
    int vis_int  = convert_int_rte(cell_vis * SEGLEN_FACTOR); 
    atom_add(&aux_args.vis_array[data_ptr], vis_int);
  } 
#else
  //slow beta calculation ----------------------------------------------------
  float  alpha    = aux_args.alpha[data_ptr];
  float8 mixture  = convert_float8(aux_args.mog[data_ptr])/(float)NORM;
  float weight3   = (1.0f-mixture.s2-mixture.s5);
  
  //load aux data
  float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
  //float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
  //mean_obs = mean_obs/cum_len;
  float4 mean_obs = convert_float4( as_uchar4(aux_args.mean_obs[data_ptr]) ) / 255.0f;  
  
  float ray_beta, vis_cont; 
  bayes_ratio_ind( d, 
                   alpha,
                   mixture, 
                   weight3, 
                   cum_len, 
                   mean_obs.x, 
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
  //-------------------------------------------------------------------------- */          
#endif

  //reset cell_ptrs to -1 every time
  aux_args.cell_ptrs[llid] = -1;
}
#endif



#if 0
////////////////////////////////////////////////////////////////////////////////
//update_rgb_functors... these functors get pasted into cast_ray
////////////////////////////////////////////////////////////////////////////////

//Update step cell functor::seg_len
#ifdef SEGLEN
void step_cell_seglen(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
  //SLOW and accurate method
  int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
  atom_add(&aux_args.seg_len[data_ptr], seg_int);  
  
  //do cum obs for R G and B
  int cum_obsR = convert_int_rte(d * aux_args.obs.x * SEGLEN_FACTOR); 
  atom_add(&aux_args.mean_obsR[data_ptr], cum_obsR);
  int cum_obsG = convert_int_rte(d * aux_args.obs.y * SEGLEN_FACTOR); 
  atom_add(&aux_args.mean_obsG[data_ptr], cum_obsG);
  int cum_obsB = convert_int_rte(d * aux_args.obs.z * SEGLEN_FACTOR); 
  atom_add(&aux_args.mean_obsB[data_ptr], cum_obsB);

  //reset cell_ptrs to negative one every time (prevents invisible layer bug)
  //aux_args.cell_ptrs[llid] = -1;
}
#endif

#ifdef PREINF
////////////////////////////////////////////////////////////////////////////////
//preinf step cell functor
////////////////////////////////////////////////////////////////////////////////
void step_cell_preinf(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
    //keep track of cells being hit
    //cell data, i.e., alpha and app model is needed for some passes 
    //mixture is stored as uchar16 with the following components
    // float4 mu0, float4 sigma0 (last is w0), float4 mu1, float4 sigma0)
    float   alpha    = aux_args.alpha[data_ptr];
    float16 mixture  = convert_float16(aux_args.mog[data_ptr])/255.0f;
    
    //get cumulative length for this cell, and then mean_obs for all three channels
    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
   
    // if total length of rays is too small, do nothing 
    float PI = 0.0f;
    if (cum_len>1.0e-10f)
    {        
      // the mean intensity for the cell...
      float4 mean_obs = convert_float4( as_uchar4(aux_args.mean_obs[data_ptr]) ); 
      mean_obs = mean_obs/ (cum_len * 255.0f); 
    
    /*  PI = gauss_2_mixture_rgb_prob_density( mean_obs,
                                             mixture.s0123, 
                                             mixture.s4567, 
                                             mixture.s7,
                                             mixture.s89AB, 
                                             mixture.sCDEF, 
                                             1.0f - mixture.s7
                                            );  */
    /*  DEBUGGER FOR RGB The mean intensity for the cell */
    //                               float mu0, float sigma0, float w0,
    ///                               float mu1, float sigma1, float w1,
    //                               float mu2, float sigma2, float w2)
    PI = gauss_3_mixture_prob_density( mean_obs.x,
                                       mixture.s0, 
                                       mixture.s4, 
                                       mixture.s7,
                                       mixture.s8, 
                                       mixture.sC, 
                                       1.0f-mixture.s7,
                                       0.0f, 0.0f, 0.0f);/* PI */
    }
    // Calculate pre and vis infinity */
    // alpha integral += alpha * seg_len     
    //(*image_vect).y += alpha * seg_len;
    (*aux_args.alpha_int) += alpha * d; 

    // vis_prob_end 
    float vis_prob_end = exp(- (*aux_args.alpha_int)); 

    // updated pre                      Omega         *   PI  */
    //(*image_vect).w += ((*image_vect).z - vis_prob_end) *  PI;
    (*aux_args.pre_inf) += ( (*aux_args.vis_inf) - vis_prob_end) *  PI;
    
    // updated visibility probability 
    //(*image_vect).z = vis_prob_end;
    (*aux_args.vis_inf) = vis_prob_end;
   
}      
#endif



#ifdef BAYES
//bayes step cell functor
void step_cell_bayes(AuxArgs aux_args, int data_ptr, uchar llid, float d)
{
  //load aux data
  float   alpha    = aux_args.alpha[data_ptr];
  float16 mixture  = convert_float16(aux_args.mog[data_ptr])/255.0f;
  float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
   
  float PI = 0.0;
  if (d > 1.0e-10f) {    // if  too small, do nothing 
    float4 mean_obs = convert_float4( as_uchar4(aux_args.mean_obs[data_ptr]) ); 
    mean_obs = mean_obs/ (cum_len * 255.0f); 

    /*
    PI = gauss_2_mixture_rgb_prob_density( mean_obs,
                                           mixture.s0123, 
                                           mixture.s4567, 
                                           mixture.s7,
                                           mixture.s89AB, 
                                           mixture.sCDEF, 
                                           1.0f - mixture.s7
                                          ); 

    /*  DEBUGGER FOR RGB The mean intensity for the cell */
    //                               float mu0, float sigma0, float w0,
    ///                               float mu1, float sigma1, float w1,
    //                               float mu2, float sigma2, float w2)
    PI = gauss_3_mixture_prob_density( mean_obs.x,
                                       mixture.s0, 
                                       mixture.s4, 
                                       mixture.s7,
                                       mixture.s8, 
                                       mixture.sC, 
                                       1.0f-mixture.s7,
                                       0.0f, 0.0f, 0.0f);/* PI */
     /////////////////////////////////////////////////////////////////////

  }
  
  //calculate this ray's contribution to beta
  float ray_beta = ( (*aux_args.ray_pre) + PI * (*aux_args.ray_vis) ) * d / aux_args.norm;
  float vis_cont = (*aux_args.ray_vis) * d;  
                          
  //update ray_pre and ray_vis
  float temp  = exp(-alpha * d);
  
  // updated pre                      Omega         *  PI         
  (*aux_args.ray_pre) += (*aux_args.ray_vis) * (1.0f - temp) * PI;
  
  // updated visibility probability 
  (*aux_args.ray_vis) *= temp;

  //discretize and store beta and vis contribution
  int beta_int = convert_int_rte(ray_beta * SEGLEN_FACTOR);
  atom_add(&aux_args.beta_array[data_ptr], beta_int);  
  int vis_int  = convert_int_rte(vis_cont * SEGLEN_FACTOR); 
  atom_add(&aux_args.vis_array[data_ptr], vis_int);         
}
#endif


#endif
