//------------------------------------------------------------------------------
// Scene info struct - contains values (no pointers) for scene 
// and world meta information.  can be passed into kernel to cut down on args
// NOTE that this is missing information, like mem pointers and number of 
// blocks per buffer (info needed for update)
//------------------------------------------------------------------------------
typedef struct
{
  //world information  
  float4    scene_origin;             // scene origin (point)
  int4      scene_dims;               // number of blocks in each dimension
  float     block_len;                // size of each block (can only be 1 number now that we've established blocks are cubes)

  //tree meta information 
  int       root_level;               // root_level of trees
  int       num_buffer;               // number of buffers (both data and tree)
  int       tree_buffer_length;       // length of tree buffer (number of cells/trees)
  int       data_buffer_length;       // length of data buffer (number of cells)
} RenderSceneInfo;   



