#ifndef boxm_ocl_scene_h_
#define boxm_ocl_scene_h_
//:
// \file
// \brief  Small-block scene optimized for opencl's memory scheme
// \author Andrew Miller
// \date   5 Jul 2010
//
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_1d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>


#include <vsl/vsl_binary_io.h>
#include <boxm/boxm_scene_parser.h>
#include <boxm/boxm_apm_traits.h>
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
    //initializes scene from data and tree arrays
    boxm_ocl_scene(vbl_array_3d<int4> blocks, 
                   vbl_array_2d<int4> tree_buffers, 
                   vbl_array_2d<float16> data_buffers, 
                   vbl_array_1d<int2> mem_ptrs, 
                   bgeo_lvcs lvcs,
                   vgl_point_3d<double> origin,
                   vgl_vector_3d<double> block_dim);
  
    //initializes scene from xmlFile
    boxm_ocl_scene(vcl_string filename);
    boxm_ocl_scene(){}
    //copy constructor
    boxm_ocl_scene(boxm_ocl_scene* scene);
    ~boxm_ocl_scene() { }
    
    static short version_no() { return 1; }

    int  init_level() { return init_level_; }
    int  max_level() { return max_level_; }
    void block_num(int &x, int &y, int &z){x=(int)blocks_.get_row1_count(); y=(int)blocks_.get_row2_count(); z=(int)blocks_.get_row3_count();}
    void block_dim(double &x, double &y, double &z){x=block_dim_.x(); y=block_dim_.y(); z=block_dim_.z();}
    void tree_buffer_shape(int &num, int &len){num=num_tree_buffers_; len=tree_buff_length_;}
    boxm_scene_parser parser() { return parser_; }
    vbl_array_1d<int2> mem_ptrs(){ return mem_ptrs_; }
    vbl_array_3d<int4> blocks(){ return blocks_; }
    vbl_array_2d<int4> tree_buffers(){ return tree_buffers_; }
    vbl_array_2d<float16> data_buffers(){ return data_buffers_; }
    vgl_vector_3d<double> block_dim(){ return block_dim_; }
    vgl_point_3d<double> origin(){return origin_;}
    bgeo_lvcs lvcs() { return lvcs_; }

    /* ocl_scene I/O */
    bool load_scene(vcl_string filename);   
    bool save_scene(vcl_string dir);
    bool save();

    void init_scene(vbl_array_3d<int4> blocks, 
                    vbl_array_2d<int4> tree_buffers, 
                    vbl_array_2d<float16> data_buffers, 
                    vbl_array_1d<int2> mem_ptrs, 
                    bgeo_lvcs lvcs,
                    vgl_point_3d<double> origin,
                    vgl_vector_3d<double> block_dim);

    //setters
    void set_max_level(int max_level) { max_level_ = max_level; }
    void set_init_level(int init_level) { init_level_ = init_level; }
    void set_path(vcl_string dir) { path_ = dir; }
    vcl_string path() { return path_; }
    vbl_array_3d<int4> blocks_;
    vbl_array_2d<int4> tree_buffers_;
    vbl_array_1d<int2> mem_ptrs_;
    vbl_array_2d<float16> data_buffers_;
    
    //setters from 1 d int and float arrays
    void set_blocks(int* block_ptrs);
    void set_tree_buffers(int* tree_buffers);
    void set_mem_ptrs(int* mem_ptrs);
    void set_alpha_values(float* alpha_buffer);
    void set_data_values(float* data_buffer);
    
  private:

    bool init_existing_scene(); 
    bool init_existing_data();
    bool init_empty_scene();
    bool init_empty_data();
    
    /* world scene information */
    bgeo_lvcs lvcs_;
    vgl_point_3d<double> origin_;
    vgl_point_3d<double> rpc_origin_;
    //: World dimensions of a block .e.g 1 meter x 1 meter x 1 meter
    vgl_vector_3d<double> block_dim_;
    vgl_box_3d<double> world_bb_;
    boxm_apm_type app_model_;
    vcl_string path_;
    int init_level_;   //each block's init level (default 1)
    int max_level_;    //each blocks max_level (default 4)

    //actual local scene structure and data
    int num_tree_buffers_, tree_buff_length_;
    
    //pointers to each block, multiple tree buffers, and mem_ptrs for each tree_buffer

    /* model xml information */
    boxm_scene_parser parser_;
    vcl_string xml_path_;
};


vcl_ostream& operator <<(vcl_ostream &s, boxm_ocl_scene& scene);
void x_write(vcl_ostream &os, boxm_ocl_scene& scene, vcl_string name);


#endif //boxm_ocl_scene_h_
