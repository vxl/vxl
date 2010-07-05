#ifndef boxm_ocl_scene_h_
#define boxm_ocl_scene_h_
//:
// \file
// \brief  Small block scene optimized for GPU
// \author Andrew Miller
// \date   5 Jul 2010
//
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_1d.h>
#include <vnl/vnl_vector_fixed.h>

#include <vsl/vsl_binary_io.h>
class boxm_ocl_scene 
{

  //int4 scene_block = (buff_index, offset_index, sizes, <none>)
  //int4 tree_cell = (parent, child, data, data2)
  //float16 data_cell = (alpha, ... ... ...);
  //int2 mem_ptr
  typedef vnl_vector_fixed<int, 4> int4;
  typedef vnl_vector_fixed<float, 16> float16;
  typedef vnl_vector_fixed<int, 2> int2;

  public: 
    //initializes an empty scene
    boxm_ocl_scene(int nb_x, int nb_y, int nb_z, 
                     int num_tree_buffs, int tb_len);
    ~boxm_ocl_scene();
    
    
    void b_read(vsl_b_istream &s);
    void b_write(vsl_b_ostream &s);
  
  private:
    int num_tree_buffers_, tree_buff_length_;
    int num_blocks_x_, num_blocks_y_, num_blocks_z_;
    
    //pointers to each block
    vbl_array_3d<int4> blocks_;
    
    //multiple tree buffers
    vbl_array_2d<int4> tree_buffers_;
    
    //bookkeeping: keep track of start and end pointers for each tree buffer
    vbl_array_1d<int2> mem_ptrs;
    
    //data buffers - data blocks are templated
    vbl_array_2d<float16> data_buffers_;
    
};

//scene file i/o
void vsl_b_write(vsl_b_ostream & os, boxm_ocl_scene const &scene);
void vsl_b_read(vsl_b_istream & is, boxm_ocl_scene &scene);


#endif //boxm_ocl_scene_h_
