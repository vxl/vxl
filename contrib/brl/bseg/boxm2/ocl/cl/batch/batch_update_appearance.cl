#define MAX_SAMPLES 200

// Update each cell using its aux data
//
__kernel
void
batch_update_appearance(__global RenderSceneInfo  * info,
                        __global MOG_TYPE         * mog,
                        __global uint             * sampleIndex,      //starting location for each cell in sample buffer
                        __global uchar            * sampleInt,        //intensities for each cell
                        __global uchar            * sampleVis,        //visibility vals for each cell
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
    uint start = sampleIndex[gid];
    uint end   = (gid==datasize-1) ? (*totalNumSamples) : sampleIndex[gid+1];
    int numSamples = (int) (end-start);
    if(numSamples <= 0)
      return;

    //create global float* buffers for stat calcs
    global char* obs = &sampleInt[start]; //sampleInt + start;
    global char* vis = &sampleVis[start]; // + start;

    //calculatea app model
    float min_sigma = .02;
    float8 mog3 = weighted_mog3_em( obs, vis, numSamples, min_sigma );

    //reset the cells in memory
    mog3 *= (float) NORM;
    CONVERT_FUNC_SAT_RTE(mog[gid], mog3);
  }
}
