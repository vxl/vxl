#ifndef bstm_block_h_
#define bstm_block_h_
//:
// \file
// \brief  bstm_block models the building block of a bstm_scene
//
//  bstm_block is a fixed grid of octtrees, and composes a bstm_scene.
//  like bstm_data, a bstm_block will construct itself from a simple, flat
//  char stream, allocating no extra memory for itself.  This flat char stream
//  will then be owned by the block, and the block will destroy it upon calling
//  its destructor.
//

#include <iostream>
#include <iosfwd>
#include <bstm/bstm_block_metadata.h>
#include <bstm/basic/bstm_block_id.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>


class bstm_block : public vbl_ref_count
{
 public:
  typedef bstm_block_metadata metadata_t;
  typedef bstm_block_id id_t;
  typedef vnl_vector_fixed<unsigned short, 2> ushort2;    //defines a block pointer
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;    //defines a bit tree
  typedef unsigned short                      ushort;

 public:

  bstm_block(const bstm_block_id& id, bstm_block_metadata data, char* buffer);

  //: creates empty block from metadata
  bstm_block(bstm_block_metadata data);

  //: initializes empty block from metadata
  bool init_empty_block(bstm_block_metadata data);

  //: default destructor
  ~bstm_block() override { if (buffer_) delete[] buffer_; }

  //: all IO manipulates char buffers
  bool b_read(char* buffer);
  bool b_write(char* buffer);

  //: filename for all block files stored by bstm_scenes
  std::string              filename()          const { return "block.bin"; }

  //: accessors
  bstm_block_id&            block_id()          { return block_id_; }         //somehow make this a const return..
  const bstm_block_id&      block_id()          const { return block_id_; }
  char*                     buffer()            { return buffer_; }
  boxm2_array_3d<uchar16>&  trees()             { return trees_; }
  const boxm2_array_3d<uchar16>&  trees()       const { return trees_; }
  vgl_vector_3d<double>&    sub_block_dim()     { return sub_block_dim_; }
  vgl_vector_3d<unsigned>&  sub_block_num()     { return sub_block_num_; }
  const vgl_vector_3d<unsigned>&  sub_block_num() const { return sub_block_num_; }
  int                       tree_buff_length()  const { return (int)trees_.size(); }
  int                       init_level()        const { return init_level_; }
  int                       max_level()         const { return max_level_; }
  int                       max_mb()            const { return max_mb_; }
  long                      byte_count()        const { return byte_count_; }
  int                       num_buffers()       const { return 1; } //legacy code...keep it for now.
  //: mutators
  void set_block_id(bstm_block_id id)   { block_id_ = id; }
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
  bstm_block_id           block_id_;

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

  bool read_only_;
};


//: Smart_Pointer typedef for bstm_block
typedef vbl_smart_ptr<bstm_block> bstm_block_sptr;

//: output stream
std::ostream& operator <<(std::ostream &s, bstm_block& block);

//: write to xml file
//void x_write(std::ostream &os, bstm_block& scene, std::string name);

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, bstm_block const& scene);
void vsl_b_write(vsl_b_ostream& os, const bstm_block* &p);
void vsl_b_write(vsl_b_ostream& os, bstm_block_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bstm_block_sptr const& sptr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, bstm_block &scene);
void vsl_b_read(vsl_b_istream& is, bstm_block* p);
void vsl_b_read(vsl_b_istream& is, bstm_block_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bstm_block_sptr const& sptr);

#endif // bstm_block_h_
