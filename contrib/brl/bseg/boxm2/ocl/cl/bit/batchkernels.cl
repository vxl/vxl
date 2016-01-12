#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics: enable
#if NVIDIA
 #pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#endif
#ifdef MOG_TYPE_16
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs=convert_ushort8_sat_rte(data);
    #define MOG_TYPE ushort8
    #define NORM 65535;
#endif
#ifdef MOG_TYPE_8
   #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs=convert_uchar8_sat_rte(data);
   #define MOG_TYPE uchar8
   #define NORM 255;
#endif


#ifdef CUMLEN
typedef struct
{
  __global int* seg_len;
  __global float * output;
  __constant RenderSceneInfo * linfo;

} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);
__kernel
void
cum_len_main(__constant  RenderSceneInfo    * linfo,
             __global    int4               * tree_array,       // tree structure for each block
             __global    int                * aux_array,        // aux data array (four aux arrays strung together)
             __constant  uchar              * bit_lookup,       // used to get data_index
             __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
             __global    uint4              * imgdims,          // dimensions of the input image
             __global    float              * output,
             __local     uchar16            * local_tree,       // cache current tree into local memory
             __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
{
  //get local id (0-63 for an 8x8) of this patch
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  //initialize pre-broken ray information (non broken rays will be re initialized)
  //----------------------------------------------------------------------------
  // get image coordinates and camera,
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  //grab input image value (also holds vis)
  float vis = 1.0f;
  //float vis = inImage.z;
  barrier(CLK_LOCAL_MEM_FENCE);

  // cases #of threads will be more than the pixels.
  if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
    return;

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image,
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
  calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

  //----------------------------------------------------------------------------
  // we know i,j map to a point on the image, have calculated ray
  // BEGIN RAY TRACE
  //----------------------------------------------------------------------------
  AuxArgs aux_args;
  aux_args.linfo    = linfo;
  aux_args.seg_len  = aux_array;
  aux_args.output = output;
  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                  //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);    //utility info
}
#endif


#ifdef UPDATE_HIST
typedef struct
{
    __global float  * alpha;
    __global int    * seg_len;
    __global int    * hist;  // int8
    float   obs;
    float   *vis;
    float   * output;
} AuxArgs;

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,__constant RenderSceneInfo*,
              __global int4*,local uchar16*,constant uchar *,local uchar *,float*,AuxArgs,float tnear, float tfar);
__kernel
void
update_hist_main(__constant  RenderSceneInfo    * linfo,
                 __global    int4               * tree_array,       // tree structure for each block
                 __global    float              * alpha_array,      // alpha for each block
                 __global    int                * aux_array,        // aux data array (four aux arrays strung together)
                 __global    int                * hist,
                 __constant  uchar              * bit_lookup,       // used to get data_index
                 __global    float16            * camera,           // camera orign and SVD of inverse of camera matrix
                 __global    uint4              * imgdims,          // dimensions of the input image
                 __global    float              * in_image,         // the input image
                 __global    float              * vis_image,        // Vis image to keep visibility over multiple blocks
                 __global    float              * output,
                 __local     uchar16            * local_tree,       // cache current tree into local memory
                 __local     uchar              * cumsum )          // cumulative sum for calculating data pointer
{
    //get local id (0-63 for an 8x8) of this patch
    uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));

    //----------------------------------------------------------------------------
    // get image coordinates and camera,
    // check for validity before proceeding
    //----------------------------------------------------------------------------
    int i=0,j=0;
    i=get_global_id(0);
    j=get_global_id(1);
    int imIndex = j*get_global_size(0) + i;
    float inImage = in_image[imIndex];

    //grab input image value (also holds vis)
    float obs = inImage;
    float vis = vis_image[imIndex];
    //float vis = inImage.z;
    barrier(CLK_LOCAL_MEM_FENCE);

    // cases #of threads will be more than the pixels.
    if (i>=(*imgdims).z || j>=(*imgdims).w || i<(*imgdims).x || j<(*imgdims).y)
        return;

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image,
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    float ray_ox, ray_oy, ray_oz, ray_dx, ray_dy, ray_dz;
    calc_scene_ray(linfo, camera, i, j, &ray_ox, &ray_oy, &ray_oz, &ray_dx, &ray_dy, &ray_dz);

    //----------------------------------------------------------------------------
    // we know i,j map to a point on the image, have calculated ray
    // BEGIN RAY TRACE
    //----------------------------------------------------------------------------
    AuxArgs aux_args;
    aux_args.vis              = &vis;
    aux_args.alpha            = alpha_array;
    aux_args.seg_len          = aux_array;
    aux_args.hist             = hist;
    aux_args.obs              = obs;
    cast_ray( i, j,
              ray_ox, ray_oy, ray_oz,
              ray_dx, ray_dy, ray_dz,
              linfo, tree_array,                                  //scene info
              local_tree, bit_lookup, cumsum, aux_args.vis , aux_args,0,MAXFLOAT);    //utility info
    vis_image[imIndex] = *(aux_args.vis);
}
#endif

#ifdef CLEAN_SEG_LEN
__kernel
void
clean_seg_len_main(__constant RenderSceneInfo * info,
                   __global float           * aux_array,
                   __global float           * output)
{
  int gid=get_global_id(0);
  int datasize = info->data_len;
  if (gid<datasize)
      aux_array[gid] = 0.0f;
}
#endif
#ifdef UPDATE_PROB
__kernel
void
update_prob_main(__constant RenderSceneInfo * info,
                 __constant uchar           * bit_lookup,
                 __global   int4              * tree_array,       // tree structure for each block
                 __global   float             * alpha_array,
                 __global   uchar8            * intensity_array,
                 __global   float8            * hist,
                 __global   float             * output,
                 __local    uchar             * local_tree,
                 __local    uchar             * cumsum
                )
{
    int gid=get_global_id(0);
    int MAX_INNER_CELLS=0;
      if (info->root_level == 1)     MAX_INNER_CELLS=1;
    else if (info->root_level == 2)  MAX_INNER_CELLS=9;
    else if (info->root_level == 3)  MAX_INNER_CELLS=73;

    if (gid<info->dims.x*info->dims.y*info->dims.z)
    {
        uchar16 tree=as_uchar16(tree_array[gid]);
        local_tree[0] =tree.s0;        local_tree[8] =tree.s8;
        local_tree[1] =tree.s1;        local_tree[9] =tree.s9;
        local_tree[2] =tree.s2;        local_tree[10]=tree.sa;
        local_tree[3] =tree.s3;        local_tree[11]=tree.sb;
        local_tree[4] =tree.s4;        local_tree[12]=tree.sc;
        local_tree[5] =tree.s5;        local_tree[13]=tree.sd;
        local_tree[6] =tree.s6;        local_tree[14]=tree.se;
        local_tree[7] =tree.s7;        local_tree[15]=tree.sf;

        cumsum[0] = local_tree[0];
        int cumIndex    = 1;

        int buff_index=(int)as_ushort((uchar2) (local_tree[13], local_tree[12]));
        int buff_offset=(int)as_ushort((uchar2) (local_tree[11], local_tree[10]));

        for (int i=0; i<MAX_INNER_CELLS; i++)
        {
            //if current bit is 0 and parent bit is 1, you're at a leaf
            int pi = i>0?(i-1)>>3:0;           //Bit_index of parent bit
            bool validParent = tree_bit_at(local_tree, pi) || (i==0);// special case for root

            if (validParent && tree_bit_at(local_tree, i)==0)
            {
                //find side length for cell of this level = block_len/2^currDepth
                int currDepth = get_depth(i);
                float side_len = info->block_len/(float) (1<<currDepth);
                int dataIndex = data_index_cached2(local_tree,i, bit_lookup, cumsum, &cumIndex);

                float8 hist8  = convert_float8(as_int8(hist[dataIndex]))/SEGLEN_FACTOR;
                float pbC0=(hist8.s0+hist8.s1+hist8.s2+hist8.s3);
                float pbC1=(hist8.s4+hist8.s5+hist8.s6);

                float chisqr1=0.0;
                if (pbC0>0)
                    chisqr1=(hist8.s0-0.125 *pbC0)*(hist8.s0-0.125*pbC0)/(0.125*pbC0*pbC0)+
                            (hist8.s1-0.47  *pbC0 )*(hist8.s1-0.47*pbC0 )/(0.47*pbC0*pbC0) +
                            (hist8.s2-0.28  *pbC0 )*(hist8.s2-0.28*pbC0 )/(0.28*pbC0*pbC0) +
                            (hist8.s3-0.121 *pbC0)*(hist8.s3-0.121*pbC0)/(0.121*pbC0*pbC0);

                float chisqr2=0.0;
                if (pbC1>0)
                    chisqr2=(hist8.s4-0.42*pbC1 )*(hist8.s4-0.42*pbC1 )/(0.42*pbC1*pbC1) +
                            (hist8.s5-0.378*pbC1)*(hist8.s5-0.378*pbC1)/(0.378*pbC1*pbC1)+
                            (hist8.s6-0.197*pbC1)*(hist8.s6-0.197*pbC1)/(0.197*pbC1*pbC1);
               // float chisqr1=0.0;
                //if (pbC0>0)
                //    chisqr1=(hist8.s0-pbC0)*(hist8.s0-pbC0)/pbC0+
                //    (hist8.s1-pbC0)*(hist8.s1-pbC0)/pbC0+
                //    (hist8.s2-pbC0)*(hist8.s2-pbC0)/pbC0+
                //    (hist8.s3-pbC0)*(hist8.s3-pbC0)/pbC0;

                //float chisqr2=0.0;
                //if (pbC1>0)
                //    chisqr2=(hist8.s4-pbC1)*(hist8.s4-pbC1)/pbC1+
                //    (hist8.s5-pbC1)*(hist8.s5-pbC1)/pbC1+
                //    (hist8.s6-pbC1)*(hist8.s6-pbC1)/pbC1;

                float max_intensity=0.0;
                float max_intensity_count=0.0;
                if (max_intensity_count<hist8.s0)max_intensity_count=hist8.s0,max_intensity=0.125;
                if (max_intensity_count<hist8.s1)max_intensity_count=hist8.s1,max_intensity=0.375;
                if (max_intensity_count<hist8.s2)max_intensity_count=hist8.s2,max_intensity=0.625;
                if (max_intensity_count<hist8.s3)max_intensity_count=hist8.s3,max_intensity=0.875;
                //if (max_intensity_count<hist8.s4)max_intensity_count=hist8.s4,max_intensity=0.25;
                //if (max_intensity_count<hist8.s5)max_intensity_count=hist8.s5,max_intensity=0.5;
                //if (max_intensity_count<hist8.s6)max_intensity_count=hist8.s6,max_intensity=0.75;
                float avg_intensity=0.0f;
                if (pbC0>0)
                 avg_intensity=(hist8.s0*(0.125)+hist8.s1*(0.375)+hist8.s2*(0.625)+hist8.s3*(0.875))/pbC0;
                intensity_array[dataIndex]=(uchar8)((unsigned char)floor(avg_intensity*255.0f),0,255,0,0,0,0,0);

                float chisqr=chisqr1+chisqr2;


                float P1=0.0;
                float P2=0.0;
                if(chisqr1<1e-10) P1=0.0;
                else if (chisqr1<0.1) P1=0.038*chisqr1*chisqr1;
                else                  P1=tanh(0.248*(chisqr1-0.1));

                if(chisqr2<1e-10) P2=0.0;
                //else if (chisqr2<3.8) P2=0.021*chisqr2*chisqr2;
                else                  P2=tanh(0.32*(chisqr2+0.2));

                float P=P1<P2?P1:P2;
                //P=P*P;

                //if(P<0.05) P=0.01;


                //float P=0.0;
                //if(chisqr<1e-10) P=0.0;
                //else if (chisqr<3.8) P=0.021*chisqr*chisqr;
                //else                  P=tanh(0.168*(chisqr-2));

                //P=P*P;
                //if(P<0.75) P=0.0;
                //get alpha value for this cell;
                alpha_array[dataIndex]=-(log(1-P))/side_len;
                hist[dataIndex]=(float8)as_float((int)0);
            }
        }
   }
}
#endif
