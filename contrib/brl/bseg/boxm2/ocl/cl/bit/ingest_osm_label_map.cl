// Ingest osm (open street map) kernel
// MOG type: boxm2_label_short
// assign the lable of the ray to all voxels along the ray, when rendering boxm2_lable_short, low prob ones aren't displayed anyways

#ifdef INGEST_OSM_LABEL_MAP

// need to define a struct of type AuxArgs with auxiliary arguments
// to supplement cast ray args
typedef struct
{
  __global MOG_TYPE * label_data;
  __global uchar label;
} AuxArgs;

void step_cell_ingest_osm_label_map(AuxArgs aux_args, int data_ptr)
{
  aux_args.label_data[data_ptr] = (MOG_TYPE)aux_args.label;
}

// forward declare cast ray
void cast_ray(int,int,float,float,float,float,float,float,
              __constant RenderSceneInfo*, __global int4*,
              __local uchar16*, __constant uchar *,__local uchar *,
              float*, AuxArgs,float tnear, float tfar);
__kernel
void ingest_osm_label_map(__constant  RenderSceneInfo    * linfo,
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
#endif // INGEST_OSM_LABEL_MAP
