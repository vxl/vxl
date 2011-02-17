//Update step cell functor::seg_len
#ifdef SEGLEN
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
    if(aux_args.ray_bundle_array[llid].y==1) 
    {
      //scale!
      int seg_int = convert_int_rte(aux_args.cached_aux[llid].x * SEGLEN_FACTOR); 
      int cum_obs = convert_int_rte(aux_args.cached_aux[llid].y * SEGLEN_FACTOR);  
      
      //atomically update the cells
      atom_add(&aux_args.seg_len[data_ptr], seg_int); 
      atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
    }
    //------------------------------------------------------------------------ 
#else
    //SLOW and accurate method
    int seg_int = convert_int_rte(d * SEGLEN_FACTOR);
    atom_add(&aux_args.seg_len[data_ptr], seg_int);  
    int cum_obs = convert_int_rte(d * aux_args.obs * SEGLEN_FACTOR); 
    atom_add(&aux_args.mean_obs[data_ptr], cum_obs);
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
    float8 mixture  = convert_float8(aux_args.mog[data_ptr])/255.0f;
    float  weight3  = (1.0f-mixture.s2-mixture.s5);
    
    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
    float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
    mean_obs = mean_obs/cum_len;
   
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
    float8 mixture  = convert_float8(aux_args.mog[data_ptr])/255.0f;
    float weight3   = (1.0f-mixture.s2-mixture.s5);
    
    //load aux data
    float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
    float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
    mean_obs = mean_obs/cum_len;
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
  float8 mixture  = convert_float8(aux_args.mog[data_ptr])/255.0f;
  float weight3   = (1.0f-mixture.s2-mixture.s5);
  
  //load aux data
  float cum_len  = convert_float(aux_args.seg_len[data_ptr])/SEGLEN_FACTOR; 
  float mean_obs = convert_float(aux_args.mean_obs[data_ptr])/SEGLEN_FACTOR;
  mean_obs = mean_obs/cum_len;
  
  float ray_beta, vis_cont; 
  bayes_ratio_ind( d, 
                   alpha,
                   mixture, 
                   weight3, 
                   cum_len, 
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
  //-------------------------------------------------------------------------- */          
#endif

  //reset cell_ptrs to -1 every time
  aux_args.cell_ptrs[llid] = -1;
}
#endif
