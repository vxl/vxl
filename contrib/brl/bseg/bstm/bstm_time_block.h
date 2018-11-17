#ifndef bstm_time_block_h_
#define bstm_time_block_h_
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
//
#include <iostream>
#include <iosfwd>
#include <bstm/bstm_time_tree.h>
#include <bstm/bstm_block_metadata.h>
#include <bstm/basic/bstm_block_id.h>
#include <boxm2/basic/boxm2_array_1d.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>


class bstm_time_block : public vbl_ref_count
{
 public:
  typedef vnl_vector_fixed<unsigned char, 8> uchar8;    //defines a time tree

 public:

  //: creates possibly refined time block from existing data
  bstm_time_block(const bstm_block_id& id, bstm_block_metadata data, char* buffer, std::size_t length);

  //: creates possibly refined time block with specified number of elements
  bstm_time_block(const bstm_block_id& id, bstm_block_metadata data, unsigned num_el);

  //: creates empty block from metadata
  bstm_time_block(bstm_block_metadata data);

  //: default destructor
  ~bstm_time_block() override { if (buffer_) delete[] buffer_;}

  //: returns a cell's the time tree containing the specified local time t.
  uchar8&                   get_cell_tt(int cell_data_offset, double local_t);

  //: returns all time trees corresponding to a cell (returns a reference, does not duplicate data.)
  boxm2_array_1d<uchar8>    get_cell_all_tt(int cell_data_offset);

  //: sets all time trees corresponding to a cell
  void                      set_cell_all_tt(int cell_data_offset, const boxm2_array_1d<uchar8>& input);

  //: sets a given time tree corresponding to a cell
  void                      set_cell_tt(int cell_data_offset, const uchar8& input, double local_t);

  //: returns the index of the time tree that contains the given local time [0,sub_block_num_t_)
  unsigned int             tree_index(double local_time);

  //: all IO manipulates char buffers
  bool                      b_read(char* buffer);
  bool                      b_write(char* buffer);

  //: filename for all block files stored by bstm_scenes
  std::string                filename()          const { return "time_block.bin"; }

  //: accessors
  bstm_block_id&            block_id()          { return block_id_; }         //somehow make this a const return..
  char*                     buffer()            { return buffer_; }
  boxm2_array_1d<uchar8>&   time_trees()        { return time_trees_; }
  unsigned                 sub_block_num()     const { return sub_block_num_t_; }
  int                       tree_buff_length()  const { return (int)time_trees_.size(); }
  int                       init_level()        const { return init_level_t_; }
  int                       max_level()         const { return max_level_t_; }
  long                      byte_count()        const { return byte_count_; }



  //: mutators
  void set_block_id(bstm_block_id id)   { block_id_ = id; }
  void set_init_level(int level)        { init_level_t_ = level; }
  void set_max_level(int level)         { max_level_t_ = level; }
  void set_byte_count(long bc)          { byte_count_ = bc; }

  //: regardless of the way the instance is constructed, enable write
  void enable_write() { read_only_ = false; }
  bool read_only() const { return read_only_; }

 private:

  //: helper to calc number of cells given metadata
  void octree_num_cell_calc(bstm_block_metadata& data, long& num_cells);

  //: calculate the number of bytes in a time block
  long calc_byte_count(bstm_block_metadata& data);

  //: initializes empty block from metadata
  bool init_empty_block();

  //: unique block id (currently 3D address)
  bstm_block_id          block_id_;

  //: byte buffer
  char*                   buffer_;

  //: high level arrays store sub block information
  boxm2_array_1d<uchar8> time_trees_;

  //: number of bytes this block takes up (on disk and ram)
  long                    byte_count_;

  //: Number of sub-blocks in time
  unsigned sub_block_num_t_;

  //: info about block's trees
  int  init_level_t_;  //time tree's initial level(default 1)
  int  max_level_t_;   //time tree's maximum level(default 6)

  bool read_only_;
};

//: Smart_Pointer typedef for bstm_block
typedef vbl_smart_ptr<bstm_time_block> bstm_time_block_sptr;

//: output stream
std::ostream& operator <<(std::ostream &s, bstm_time_block& block);

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, bstm_time_block const& scene);
void vsl_b_write(vsl_b_ostream& os, const bstm_time_block* &p);
void vsl_b_write(vsl_b_ostream& os, bstm_time_block_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bstm_time_block_sptr const& sptr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, bstm_time_block &scene);
void vsl_b_read(vsl_b_istream& is, bstm_time_block* p);
void vsl_b_read(vsl_b_istream& is, bstm_time_block_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bstm_time_block_sptr const& sptr);





#endif // bstm_time_block_h_
