#define MAX_SAMPLES 200

// Update each cell using its aux data
//
__kernel
void
batch_update_appearance(__global RenderSceneInfo  * info,
                        __global MOG_TYPE         * mog,
                        __global int              * sampleIndex,      //starting location for each cell in sample buffer
                        __global float            * sampleInt,        //intensities for each cell
                        __global float            * sampleVis,        //visibility vals for each cell
                        __global int              * totalNumSamples,       //total number of samples for this block
                        __global float            * output)
{
  //--- Create local memory for weighted_mog3_em sorting ----
  //__local float sampleMem1[MAX_SAMPLES]; 
  //__local float sampleMem2[MAX_SAMPLES]; 
  //--- end local memory instantiation ----------------------

  int gid = get_global_id(0);
  int datasize = info->data_len ;//* info->num_buffer;
  if (gid<datasize)
  {
    //calculate the number of samples for this cell
    int start = sampleIndex[gid]; 
    int end   = (gid==datasize-1) ? (*totalNumSamples) : sampleIndex[gid+1]; 
    int numSamples = end-start; 
    if(numSamples <= 0) 
      return; 
    
    //create global float* buffers for stat calcs
    global float* obs = &sampleInt[start]; //sampleInt + start; 
    global float* vis = &sampleVis[start]; // + start; 
    
    //calculatea app model
    float min_sigma = .02; 
    float8 mog3 = weighted_mog3_em( obs, vis, numSamples, min_sigma ); 
    //DEBUGGER MEAN_VAR
/*
    float2 mean_var = weighted_mean_var(obs, vis, numSamples); 
    float8 mog3 = (float8) (0.0f); 
    mog3.s0 = mean_var.x; 
    mog3.s1 = max(mean_var.y, .02f);
    mog3.s1 = .06f; 
    mog3.s2 = .99f; 
*/
    
    //reset the cells in memory
    mog3 *= (float) NORM; 
    CONVERT_FUNC_SAT_RTE(mog[gid], mog3);
  }
}
