// Ingest label map kernel,
// MOG type: boxm2_label_short
// assign the label of the ray to all voxels along the ray, when rendering boxm2_label_short, low prob ones aren't displayed anyways

#ifdef INGEST_LABEL_MAP

//need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global MOG_TYPE * label_data;
  __global uchar label;
} AuxArgs;

/* previous when label is directly put to the voxel
void step_cell_ingest_label_map(AuxArgs aux_args, int data_ptr)
{
  aux_args.label_data[data_ptr] = (MOG_TYPE)aux_args.label;
}*/


/* NEW ingest label step cell, keep the previous value in the upper two bits (bit 7 and bit 6) */
void step_cell_ingest_label_map(AuxArgs aux_args, int data_ptr)
{
  MOG_TYPE current = aux_args.label_data[data_ptr];
  MOG_TYPE invalid = 100;
  if (current == invalid) // invalid
    current = 0;
  else if (current >= 2)  // 0 is invalid, 1 is horizontal, 2 and up are vertical
    current = 2;     // make all types of vertical 2, so we have 3 values as orientation
  //current << 6;      // shift 6 bits to the left, so upper 2 bits are orientation bits
  current *= 64;
  current += (MOG_TYPE)aux_args.label;   // add the land type, so the lower 6 bits will represent the land type

  aux_args.label_data[data_ptr] = current;
}

//forward declare cast ray (so you can use it)
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void ingest_label_map(__constant  RenderSceneInfo    * linfo,
                       __global    uint4              * image_dims,
                       __global    float4             * ray_origin_buff,
                       __global    int4               * tree_array,
                       __global    ushort             * data_array,
                       __global    uchar              * data_buff,
                       __constant  uchar              * bit_lookup,
                       __local     uchar16            * local_tree,
                       __local     uchar              * cumsum,        // cumulative sum helper for data pointer
                       __local     int                * imIndex)
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;
  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*image_dims).z || j>=(*image_dims).w)
    return;

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origin_buff[ imIndex[llid] ];
  uchar data = data_buff[ imIndex[llid] ];

  float4 ray_d = (float4)( 0.001,  0.001, -1.0, 1.0);

  float ray_ox = 0.0f;float ray_oy = 0.0f;float ray_oz = 0.0f;
  float ray_dx = 0.0f;float ray_dy = 0.0f;float ray_dz = 0.0f;

  calc_scene_ray_generic_cam(linfo, ray_o, ray_d,
                             &ray_ox, &ray_oy, &ray_oz,
                             &ray_dx, &ray_dy, &ray_dz);

  ////----------------------------------------------------------------------------
  //// we know i,j map to a point on the image, have calculated ray
  //// BEGIN RAY TRACE
  ////----------------------------------------------------------------------------

  AuxArgs aux_args;
  //float out1 =0.0f;
  aux_args.label_data  = data_array;
  //aux_args.outimg = out1;
  aux_args.label = data;
  float vis =1.0;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

}


__kernel
void ingest_label_map_with_dir(__constant  RenderSceneInfo    * linfo,
                       __global    uint4              * image_dims,
                       __global    float4             * ray_origin_buff,
                       __global    float4             * ray_dir_buff,
                       __global    int4               * tree_array,
                       __global    ushort             * data_array,
                       __global    uchar              * data_buff,
                       __constant  uchar              * bit_lookup,
                       __local     uchar16            * local_tree,
                       __local     uchar              * cumsum,        // cumulative sum helper for data pointer
                       __local     int                * imIndex)
{
  //----------------------------------------------------------------------------
  //get local id (0-63 for an 8x8) of this patch + image coordinates and camera
  // check for validity before proceeding
  //----------------------------------------------------------------------------
  uchar llid = (uchar)(get_local_id(0) + get_local_size(0)*get_local_id(1));
  int i=0,j=0;
  i=get_global_id(0);
  j=get_global_id(1);
  imIndex[llid] = j*get_global_size(0)+i;
  // check to see if the thread corresponds to an actual pixel as in some
  // cases #of threads will be more than the pixels.
  if (i>=(*image_dims).z || j>=(*image_dims).w)
    return;

  //----------------------------------------------------------------------------
  // Calculate ray origin, and direction
  // (make sure ray direction is never axis aligned)
  //----------------------------------------------------------------------------
  float4 ray_o = ray_origin_buff[ imIndex[llid] ];
  uchar data = data_buff[ imIndex[llid] ];

  float4 ray_d = ray_dir_buff[ imIndex[llid] ];

  float ray_ox = 0.0f;float ray_oy = 0.0f;float ray_oz = 0.0f;
  float ray_dx = 0.0f;float ray_dy = 0.0f;float ray_dz = 0.0f;

  calc_scene_ray_generic_cam(linfo, ray_o, ray_d,
                             &ray_ox, &ray_oy, &ray_oz,
                             &ray_dx, &ray_dy, &ray_dz);

  ////----------------------------------------------------------------------------
  //// we know i,j map to a point on the image, have calculated ray
  //// BEGIN RAY TRACE
  ////----------------------------------------------------------------------------

  AuxArgs aux_args;
  //float out1 =0.0f;
  aux_args.label_data  = data_array;
  //aux_args.outimg = out1;
  aux_args.label = data;
  float vis =1.0;

  cast_ray( i, j,
            ray_ox, ray_oy, ray_oz,
            ray_dx, ray_dy, ray_dz,
            linfo, tree_array,                                    //scene info
            local_tree, bit_lookup, cumsum, &vis, aux_args,0,MAXFLOAT);      //utility info

}


#endif // INGEST_LABEL_MAP
