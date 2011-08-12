//------------------------------------------------------------------------------
// Scene info struct - contains values (no pointers) for scene 
// and world meta information.  can be passed into kernel to cut down on args
// NOTE that this is missing information, like mem pointers and number of 
// blocks per buffer (info needed for update)
//------------------------------------------------------------------------------


//apperance model SIZE for the scene. 
//note that merge and refine use this for both numobs and alpha
#ifdef MOG_TYPE_16 
    #define MOG_TYPE int4
    #define CONVERT_FUNC(lhs,data) ushort8 lhs = as_ushort8(data)
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = as_int4( convert_ushort8_sat_rte(data) )
    #define NORM 65535
#endif
#ifdef MOG_TYPE_8 
    #define MOG_TYPE int2
    #define CONVERT_FUNC(lhs,data) uchar8 lhs = as_uchar8(data)
    #define CONVERT_FUNC_FLOAT8(lhs,data) float8 lhs = convert_float8( as_uchar8(data) )
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = as_int2( convert_uchar8_sat_rte(data) )
    #define NORM 255
#endif
#ifdef MOG_TYPE_4
    #define MOG_TYPE uchar4
    #define CONVERT_FUNC(lhs,data) uchar4 lhs = as_uchar4(data)
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = convert_uchar4_sat_rte(data)
    #define NORM 65535
#endif
#ifdef MOG_TYPE_2
    #define MOG_TYPE uchar2
    #define CONVERT_FUNC(lhs,data) ushort8 lhs = as_ushort8(data)
    #define NORM 65535
#endif
#ifndef MOG_TYPE
    #define MOG_TYPE int2
    #define CONVERT_FUNC(lhs,data) uchar8 lhs = as_uchar8(data)
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = convert_uchar8_sat_rte(data)
    #define NORM 255
#endif

//pixel type (RGB or GREY)
#ifdef PIXEL_GREY
    #define PIXEL_TYPE float
#endif
#ifdef PIXEL_RGB
    #define PIXEL_TYPE float4
#endif


#define SEGLEN_FACTOR 64000000.0f   //Hack representation of int32.maxvalue/(ni*nj*block_length)

typedef struct
{
  //world information  
  float4    origin;                   // scene origin (point)
  int4      dims;                     // number of blocks in each dimension
  float     block_len;                // size of each block (can only be 1 number now that we've established blocks are cubes)
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
