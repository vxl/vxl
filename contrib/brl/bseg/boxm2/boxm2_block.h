#ifndef boxm2_block_h_
#define boxm2_block_h_
//:
// \file
// \brief  boxm2_block models the building block of a boxm2_scene
// \author Andrew Miller
// \date   26 Oct 2010
//
#include <boxm2/boxm2_block_id.h>
#include <boxm2/boxm2_array_1d.h>
#include <boxm2/boxm2_array_2d.h>
#include <boxm2/boxm2_array_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//ocl include
#include <bocl/bocl_cl.h>

//: NOTE BOXM2_BLOCK: 
//: boxm2_block is a fixed grid of octtrees, and composes a boxm2_scene.  
//: like boxm2_data, a boxm2_block will construct itself from a simple, flat
//: char stream, allocating no extra memory for itself.  This flat char stream 
//: will then be owned by the block, and the block will destroy it upon calling
//: it's destructor.  

//: block info that can be easily made into a buffer and sent to gpu
struct BlockInfo
{
  //world information  
  cl_float    scene_origin[4];          // scene origin (point)
  cl_int      scene_dims[4];            // number of blocks in each dimension
  cl_float    block_len;                // size of each block (can only be 1 number now that we've established blocks are cubes)
  cl_float    epsilon;                  // block_len/100.0 (placed here to avoid using a register)

  //tree meta information 
  cl_int      root_level;               // root_level of trees
  cl_int      num_buffer;               // number of buffers (both data and tree)
  cl_int      tree_buffer_length;       // length of tree buffer (number of cells/trees)
  cl_int      data_buffer_length;       // length of data buffer (number of cells)
}; 


class boxm2_block : public vbl_ref_count
{
  typedef vnl_vector_fixed<unsigned short, 2> ushort2;    //defines a block pointer
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;    //defines a bit tree
  typedef unsigned short                      ushort;     

  public:
    
    //: only available constructor - from directory and block_id 
    boxm2_block(char* buffer);    
    boxm2_block(); 

    //: default destructor
    ~boxm2_block() { }

    //: all IO manipulates char buffers
    bool b_read(char* buffer);
    bool b_write(char* buffer);
    
    //: filename for all block files stored by boxm2_scenes
    vcl_string              filename()          { return "block.bin"; }

    //: accessors 
    boxm2_block_id          block_id()          { return block_id_; }
    boxm2_array_3d<uchar16> trees()             { return trees_; }
    boxm2_array_2d<int>     tree_ptrs()         { return tree_ptrs_; }
    boxm2_array_1d<ushort>  trees_in_buffers()  { return trees_in_buffers_; }
    boxm2_array_1d<ushort2> mem_ptrs()          { return mem_ptrs_; }
    vgl_vector_3d<double>   sub_block_dim()     { return sub_block_dim_; }
    vgl_vector_3d<int>      sub_block_num()     { return sub_block_num_; }
    int                     num_buffers()       { return tree_ptrs_.rows(); }
    int                     tree_buff_length()  { return tree_ptrs_.cols(); }
    int                     init_level()        { return init_level_; }
    int                     max_level()         { return max_level_; }
    int                     max_mb()            { return max_mb_; }
    long                    byte_count()        { return byte_count_; }
    
    //: mutators 
    void set_block_id(boxm2_block_id id)  { block_id_ = id; }
    void set_init_level(int level)        { init_level_ = level; }
    void set_max_level(int level)         { max_level_ = level; }
    void set_max_mb(int mb)               { max_mb_ = mb; }
    void set_byte_count(long bc)          { byte_count_ = bc; }
    
  private:
  
    //: unique block id (currently 3D address)
    boxm2_block_id          block_id_; 
    
    //: number of bytes this block takes up (on disk and ram)
    long                    byte_count_;

    //: World dimensions of a block .e.g 1 meter x 1 meter x 1 meter
    vgl_vector_3d<double>   sub_block_dim_;
    vgl_vector_3d<int>      sub_block_num_; 
    
    //: high level arrays store sub block information
    boxm2_array_3d<uchar16> trees_;
    boxm2_array_2d<int>     tree_ptrs_;
    boxm2_array_1d<ushort>  trees_in_buffers_;
    boxm2_array_1d<ushort2> mem_ptrs_;
    
    //: info about block's trees
    int init_level_;   //each sub_blocks's init level (default 1)
    int max_level_;    //each sub_blocks's max_level (default 4)
    int max_mb_;       //each total block mb   
};


//Smart_Pointer typedef for boxm2_block
typedef vbl_smart_ptr<boxm2_block> boxm2_block_sptr;

vcl_ostream& operator <<(vcl_ostream &s, boxm2_block& scene);
void x_write(vcl_ostream &os, boxm2_block& scene, vcl_string name);

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream & os, boxm2_block const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_block* &p);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream & is, boxm2_block &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_block* p);

#endif // boxm2_block_h_
