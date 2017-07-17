//------------------------------------------------------------------------------
// Scene info struct - contains values (no pointers) for scene
// and world meta information.  can be passed into kernel to cut down on args
// NOTE that this is missing information, like mem pointers and number of
// blocks per buffer (info needed for update)
//------------------------------------------------------------------------------


//appearance model SIZE for the scene.
//note that merge and refine use this for both numobs and alpha
#ifdef MOG_TYPE_16
    #define MOG_TYPE int4
    #define CONVERT_FUNC(lhs,data) ushort8 lhs = as_ushort8(data)
    #define CONVERT_FUNC_FLOAT8(lhs,data) float8 lhs = convert_float8( as_ushort8(data) )
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = as_int4( convert_ushort8_sat_rte(data) )
    #define EXPECTED_INT(lhs, data) lhs =((data.s0) * (data.s2)+(data.s3) * (data.s5)+(data.s6) * (1 - data.s2 - data.s5));
    #define MOG_INIT(lhs) int4 lhs = {0,0,0,0}
    #define NORM 65535
#endif
#ifdef MOG_TYPE_8
    #define MOG_TYPE int2
    #define CONVERT_FUNC(lhs,data) uchar8 lhs = as_uchar8(data)
    #define CONVERT_FUNC_FLOAT8(lhs,data) float8 lhs = convert_float8( as_uchar8(data) )
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = as_int2( convert_uchar8_sat_rte(data) )
    #define EXPECTED_INT(lhs, data) lhs =((data.s0) * (data.s2)+(data.s3) * (data.s5)+(data.s6) * (1 - data.s2 - data.s5));
    #define MOG_INIT(lhs) int2 lhs = {0,0}
    #define NORM 255
#endif
#ifdef MOG_TYPE_4
    #define MOG_TYPE uchar4
    #define CONVERT_FUNC(lhs,data) uchar4 lhs = as_uchar4(data)
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = convert_uchar4_sat_rte(data)
    #define MOG_INIT(lhs) uchar4 lhs = {0, 0, 0, 0}
    #define NORM 65535
#endif
#ifdef MOG_TYPE_2
    #define MOG_TYPE uchar2
    #define CONVERT_FUNC(lhs,data) uchar2 lhs = as_uchar2(data)
    #define MOG_INIT(lhs) uchar2 lhs = {0,0}
    #define NORM 65535
#endif
#ifdef MOG_VIEW_DEP
    #define VIEW_NUM 8
    #define MOG_TYPE float16
    #define CONVERT_FUNC_FLOAT16(lhs,data) float16 lhs = convert_float16( as_uchar16(data) )
    #define NORM 255
#endif
#ifdef MOG_VIEW_DEP_COMPACT
    #define MOG_TYPE uchar16
    #define CONVERT_FUNC_FLOAT16(lhs,data) float16 lhs = convert_float16( as_uchar16(data) )
    #define EXPECTED_INT(lhs, data) lhs = (data.s0 + data.s2 + data.s4 + data.s6 +data.s8 + data.sA + data.sC + data.sE)/8.0f
    #define NORM 255.0f
#endif
#ifdef MOG_VIEW_DEP_COLOR
    #define MOG_TYPE int16
    #define CONVERT_FUNC_FLOAT16(lhs,data) float16 lhs = convert_float16( as_int16(data) )
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = ( convert_int16_sat_rte(data * 255.0f) )
    #define NORM 255
    #define NUM_OBS_TYPE float8
#endif
#ifdef MOG_VIEW_DEP_COLOR_COMPACT
    #define MOG_TYPE int8
    #define NORM 255
#endif

#ifdef NUM_OBS_VIEW_COMPACT
    #define NOBS_TYPE ushort8
    #define CONVERT_FUNC_FLOAT8(lhs,data) float8 lhs = convert_float8( data ) / 100.0f
    #define CONVERT_FUNC_USHORT8(lhs,data) lhs = convert_ushort8_sat_rte( data * 100.0f )
#endif
#ifndef NOBS_TYPE
    #define NOBS_TYPE float8
#endif

#ifdef FLOAT8
    #define MOG_TYPE float8
    #define CONVERT_FUNC(lhs,data) float8 lhs = data
    #define CONVERT_FUNC_FLOAT8(lhs,data) float8 lhs = convert_float8( data)
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = data;
    #define EXPECTED_INT(lhs, data) lhs = min(data.s7*data.s7,1/(data.s7*data.s7))
    #define NORM 1.0
#endif
#ifdef SHORT
    #define MOG_TYPE unsigned short
    #define CONVERT_FUNC(lhs,data) short lhs = convert_float8(data);
    #define CONVERT_FUNC_SAT_RTE(lhs,data) lhs = data;
    #define EXPECTED_INT(lhs, data) lhs = data.s0;
    #define NORM 65535
#endif

#ifndef MOG_TYPE
    #define MOG_TYPE float16
#endif
//pixel type (RGB or GREY)
#ifdef PIXEL_GREY
    #define PIXEL_TYPE float
#endif
#ifdef PIXEL_RGB
    #define PIXEL_TYPE float4
#endif

//SEG_LEN FACTOR used for at
#define SEGLEN_FACTOR 10000.0f   //Hack representation of int32.maxvalue/(root(3)*ni*nj)

typedef struct
{
  //world information
  float4    origin;                   // scene origin (point)
  int4      dims;                     // number of blocks in each dimension
  float     block_len;                // size of each block (can only be 1 number now that we've established blocks are cubes)
  float     epsilon;
  float pinit;
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
