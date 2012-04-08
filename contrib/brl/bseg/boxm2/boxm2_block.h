#ifndef boxm2_block_h_
#define boxm2_block_h_
//:
// \file
// \brief  boxm2_block models the building block of a boxm2_scene
//
//  boxm2_block is a fixed grid of octtrees, and composes a boxm2_scene.
//  like boxm2_data, a boxm2_block will construct itself from a simple, flat
//  char stream, allocating no extra memory for itself.  This flat char stream
//  will then be owned by the block, and the block will destroy it upon calling
//  its destructor.
//
// \author Andrew Miller
// \date   26 Oct 2010
//
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/basic/boxm2_array_1d.h>
#include <boxm2/basic/boxm2_array_2d.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_iosfwd.h>

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>


class boxm2_block : public vbl_ref_count
{
 public:
  typedef vnl_vector_fixed<unsigned short, 2> ushort2;    //defines a block pointer
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;    //defines a bit tree
  typedef unsigned short                      ushort;

 public:

  //: only available constructor - from directory and block_id
  boxm2_block(boxm2_block_id id, char* buffer);

  boxm2_block(boxm2_block_id id, boxm2_block_metadata data, char* buffer);

  //: creates empty block from metadata
  boxm2_block(boxm2_block_metadata data);

  //: initializes empty block from metadata
  bool init_empty_block(boxm2_block_metadata data);

  //: default destructor
  virtual ~boxm2_block() { if (buffer_) delete[] buffer_; }

  //: all IO manipulates char buffers
  bool b_read(char* buffer);
  bool b_write(char* buffer);

  //: filename for all block files stored by boxm2_scenes
  vcl_string              filename()          const { return "block.bin"; }

  //: accessors
  boxm2_block_id&           block_id()          { return block_id_; }         //somehow make this a const return..
  char*                     buffer()            { return buffer_; }
  boxm2_array_3d<uchar16>&  trees()             { return trees_; }
  vgl_vector_3d<double>&    sub_block_dim()     { return sub_block_dim_; }
  vgl_vector_3d<unsigned>&  sub_block_num()     { return sub_block_num_; }
  int                       num_buffers()       const { return 1; }
  int                       tree_buff_length()  const { return (int)trees_.size(); }
  int                       init_level()        const { return init_level_; }
  int                       max_level()         const { return max_level_; }
  int                       max_mb()            const { return max_mb_; }
  long                      byte_count()        const { return byte_count_; }

  //: mutators
  void set_block_id(boxm2_block_id id)  { block_id_ = id; }
  void set_init_level(int level)        { init_level_ = level; }
  void set_max_level(int level)         { max_level_ = level; }
  void set_max_mb(int mb)               { max_mb_ = mb; }
  void set_byte_count(long bc)          { byte_count_ = bc; }

  //: calculate the number of bytes a block will be given three parameters
  long calc_byte_count(int num_buffers, int trees_per_buffer, int num_trees);

  //: regardless of the way the instance is constructed, enable write
  void enable_write() { read_only_ = false; }
  bool read_only() const { return read_only_; }

 private:

  //: unique block id (currently 3D address)
  boxm2_block_id          block_id_;

  //: byte buffer
  char*                   buffer_;

  //: number of bytes this block takes up (on disk and ram)
  long                    byte_count_;

  //: high level arrays store sub block information
  boxm2_array_3d<uchar16> trees_;

  //: World dimensions of a block .e.g 1 meter x 1 meter x 1 meter
  vgl_vector_3d<double>   sub_block_dim_;
  vgl_vector_3d<unsigned> sub_block_num_;

  //: info about block's trees
  int init_level_;   //each sub_blocks's init level (default 1)
  int max_level_;    //each sub_blocks's max_level (default 4)
  int max_mb_;       //each total block mb

  bool read_only_;   // if the block existed already on the disc, do not write it back

  short version_;
};


//: Smart_Pointer typedef for boxm2_block
typedef vbl_smart_ptr<boxm2_block> boxm2_block_sptr;

//: output stream
vcl_ostream& operator <<(vcl_ostream &s, boxm2_block& block);

//: write to xml file
//void x_write(vcl_ostream &os, boxm2_block& scene, vcl_string name);

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, boxm2_block const& scene);
void vsl_b_write(vsl_b_ostream& os, const boxm2_block* &p);
void vsl_b_write(vsl_b_ostream& os, boxm2_block_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, boxm2_block_sptr const& sptr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, boxm2_block &scene);
void vsl_b_read(vsl_b_istream& is, boxm2_block* p);
void vsl_b_read(vsl_b_istream& is, boxm2_block_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, boxm2_block_sptr const& sptr);

#endif // boxm2_block_h_
