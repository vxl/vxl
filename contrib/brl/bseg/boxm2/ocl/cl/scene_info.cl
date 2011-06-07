//------------------------------------------------------------------------------
// Scene info struct - contains values (no pointers) for scene 
// and world meta information.  can be passed into kernel to cut down on args
// NOTE that this is missing information, like mem pointers and number of 
// blocks per buffer (info needed for update)
//------------------------------------------------------------------------------

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
