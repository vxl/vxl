//------------------------------------------------------------------------------
// Scene info struct - contains values (no pointers) for scene
// and world meta information.  can be passed into kernel to cut down on args
// NOTE that this is missing information, like mem pointers and number of
// blocks per buffer (info needed for update)
//------------------------------------------------------------------------------


//appearance model SIZE for the scene.
//note that merge and refine use this for both numobs and alpha

#ifdef MOG_TYPE_8
    #define MOG_TYPE int2
    #define CONVERT_FUNC(lhs,data) uchar8 lhs = as_uchar8(data)
    #define CONVERT_FUNC_FLOAT8(lhs,data) float8 lhs = convert_float8( as_uchar8(data) )
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = as_int2( convert_uchar8_sat_rte(data) )
    #define EXPECTED_INT(lhs, data) lhs =((data.s0) * (data.s2)+(data.s3) * (data.s5)+(data.s6) * (1 - data.s2 - data.s5));
    #define NORM 255
#endif

#ifdef MOG_VIEW_DEP
    #define MOG_TYPE int4
    #define CONVERT_FUNC_FLOAT16(lhs,data) float16 lhs = convert_float16( as_uchar16(data) )
    #define NORM 255
#endif


//pixel type (RGB or GREY)
#ifdef PIXEL_GREY
    #define PIXEL_TYPE float
#endif
#ifdef PIXEL_RGB
    #define PIXEL_TYPE float4
#endif

//SEG_LEN FACTOR used for at
#define SEGLEN_FACTOR 100000.0f   //Hack representation of int32.maxvalue/(root(3)*ni*nj)

typedef struct
{
  //world information
  float4    origin;                   // scene origin (point)
  int4      dims;                     // number of blocks in each dimension
  float     block_len;                // size of each block (can only be 1 number now that we've established blocks are cubes)
  float     time_block_len;           // size of time block (sub_dim_time_block)
  float     epsilon;

  
  //tree meta information
  int       root_level;               // root_level of trees
  int       num_buffer;               // number of buffers (both data and tree)
  int       tree_len;                 // length of tree buffer (number of cells/trees)
  int       data_len;                 // length of data buffer (number of cells)

} RenderSceneInfo;


typedef struct
{
    float x;
    float y;
    float z;
} point3d;


typedef struct
{
  float cum_len;
  float beta;
  uchar mean_obs;
  uchar cum_vis;
} AuxData;
