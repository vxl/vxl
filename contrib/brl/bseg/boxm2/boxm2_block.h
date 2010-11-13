#ifndef boxm2_block_h_
#define boxm2_block_h_
//:
// \file
// \brief  boxm2_block models the building block of a boxm2_scene
//
// boxm2_block is a fixed grid of octtrees, and composes a boxm2_scene.
//
// This file also contains struct BlockInfo
//
// \author Andrew Miller
// \date   26 Oct 2010
//
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_1d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_iosfwd.h>

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//ocl include
#include <bocl/bocl_cl.h>


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
    typedef vnl_vector_fixed<unsigned short, 2> ushort2;
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    typedef unsigned short                      ushort;

  public:

    //: only available constructor - from directory and block_id
    boxm2_block(char* buffer);

    //: default destructor
    ~boxm2_block() { }

    /* all IO returns a char* buffer */
    bool b_read(char* buffer);
    bool b_write(char* buffer);

    //: filename for all block files stored by boxm2_scenes
    vcl_string            filename()          const { return "block.bin"; }

    // === accessors ===

    vgl_point_3d<int>     block_id()          const { return block_id_; }
    vbl_array_3d<ushort2> tree_ptrs()         const { return tree_ptrs_; }
    vbl_array_2d<uchar16> tree_buffers()      const { return tree_buffers_; }
    vbl_array_1d<ushort>  trees_in_buffers()  const { return trees_in_buffers_; }
    vgl_vector_3d<double> sub_block_dim()     const { return sub_block_dim_; }
    vgl_vector_3d<int>    sub_block_num()     const { return sub_block_num_; }
    int                   num_buffers()       const { return tree_buffers_.rows(); }
    int                   tree_buff_length()  const { return tree_buffers_.cols(); }
    int                   init_level()        const { return init_level_; }
    int                   max_level()         const { return max_level_; }
    int                   max_mb()            const { return max_mb_; }
#if 0
    // === flat accessors (this is to be replaced by vbl_array.data_block() ==

    unsigned short*       tree_ptrs_flat()    { return (unsigned short*) &tree_ptrs_[0][0][0]; }
    unsigned char *       tree_buffers_flat() { return (unsigned char*) &tree_buffers_[0][0]; }
    unsigned short*       trees_in_buffers_flat() { return (ushort*) &trees_in_buffers_[0]; }
#endif

  private:

    //: unique block id (currently 3D address)
    vgl_point_3d<int>     block_id_;

    //: World dimensions of a block .e.g 1 meter x 1 meter x 1 meter
    vgl_vector_3d<double> sub_block_dim_;
    vgl_vector_3d<int>    sub_block_num_;

    //: high level arrays store sub block information
    vbl_array_3d<ushort2> tree_ptrs_;
    vbl_array_2d<uchar16> tree_buffers_;
    vbl_array_1d<ushort>  trees_in_buffers_;

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
