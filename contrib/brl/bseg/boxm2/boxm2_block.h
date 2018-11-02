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
#include <iostream>
#include <iosfwd>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/basic/boxm2_array_1d.h>
#include <boxm2/basic/boxm2_array_2d.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_vector_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//smart pointer stuff
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
struct cell_info{
cell_info(): depth_(0),data_index_(0), side_length_(0.0){}
  vgl_point_3d<double> cell_center_;
  unsigned depth_;
  unsigned data_index_;
  double side_length_;
};

class boxm2_block : public vbl_ref_count
{
 public:
  typedef vnl_vector_fixed<unsigned short, 2> ushort2;    //defines a block pointer
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;    //defines a bit tree
  typedef unsigned short                      ushort;

 public:

  //: only available constructor - from directory and block_id
  boxm2_block(boxm2_block_id const& id, char* buffer);

  boxm2_block(boxm2_block_id const& id, boxm2_block_metadata const& data, char* buffer);

  //: creates empty block from metadata
  boxm2_block(boxm2_block_metadata const& data);

  //: initializes empty block from metadata
  bool init_empty_block(boxm2_block_metadata const& data);

  //: default destructor
  ~boxm2_block() override { if (buffer_) delete[] buffer_; }

  //: all IO manipulates char buffers
  bool b_read(char* buffer);
  bool b_write(char* buffer);

  //: filename for all block files stored by boxm2_scenes
  std::string              filename()          const { return "block.bin"; }

  //: accessors
  boxm2_block_id&           block_id()          { return block_id_; }         //somehow make this a const return..
  char*                     buffer()            { return buffer_; }
// User has only write access to a copy of the current trees  via trees_copy(); Use the set_trees method to put them back in the block
// this way, n_cells_ will always remain up to date.
  const boxm2_array_3d<uchar16>&  trees()       { return trees_; }
  vgl_vector_3d<double>&    sub_block_dim()     { return sub_block_dim_; }
  vgl_vector_3d<unsigned>&  sub_block_num()     { return sub_block_num_; }
  int                       num_buffers()       const { return 1; }
  int                       tree_buff_length()  const { return (int)trees_.size(); }
  int                       init_level()        const { return init_level_; }
  int                       max_level()         const { return max_level_; }
  int                       max_mb()            const { return max_mb_; }
  long                      byte_count()        const { return byte_count_; }
  unsigned                  num_cells()         const { return n_cells_; }
  vgl_point_3d<double>      local_origin()      const { return local_origin_;}
  //: mutators
  void set_block_id(boxm2_block_id const& id)  { block_id_ = id; }
  void set_init_level(int level)        { init_level_ = level; }
  void set_max_level(int level)         { max_level_ = level; }
  void set_max_mb(int mb)               { max_mb_ = mb; }
  void set_byte_count(long bc)          { byte_count_ = bc; }

  //: calculate the number of bytes a block will be given three parameters
  long calc_byte_count(int num_buffers, int trees_per_buffer, int num_trees);

  //: regardless of the way the instance is constructed, enable write
  void enable_write() { read_only_ = false; }
  // Sets the trees of the current block
  void set_trees(const boxm2_array_3d<uchar16>& that){
    if(that.get_row1_count()!=this->trees_.get_row1_count() || that.get_row2_count()!=this->trees_.get_row2_count() || that.get_row3_count()!=this->trees_.get_row3_count()){
      std::cout<<"Cannot assign tree array to block id "<<this->block_id_<<" ; sizes mismatch!"<<std::endl;
      return;
      }
    std::memcpy(this->trees_.data_block() , that.data_block(),that.size() * 16);
    n_cells_ = this->recompute_num_cells();
  }
  // Returns a deep copy of the current tree array. The caller will take ownership; Use this method to modify the current tree array
  boxm2_array_3d<uchar16> trees_copy(){
    uchar16 * copy_buff = new uchar16[trees_.size()];
    std::memcpy(copy_buff, this->trees_.data_block() ,trees_.size() * 16);
    return boxm2_array_3d<uchar16>(trees_.get_row1_count(),trees_.get_row2_count(),trees_.get_row3_count(),copy_buff);

  }
  bool read_only() const { return read_only_; }

  //: construct the bounding box for the block in scene coordinates
  vgl_box_3d<double> bounding_box_global() const;

  /////
  //: Global point methods. All test if the specified global point
  // is inside *this block and produce coordinates and indices, returning true.
  // If the point is not inside the block, the methods return false.
  //
  //: if the block contains the global_pt, compute the local real coordinates of the sub_block (tree)
  bool contains(vgl_point_3d<double> const& global_pt, vgl_point_3d<double>& local_pt) const;
  //: if the block contains the global_pt, compute the integer local coordinates of the sub_block (tree)
  bool contains(vgl_point_3d<double> const& global_pt, vgl_point_3d<int>& local_pt) const;

  //: retrieve local tree coordinates for global_pt and the coords for the cell containing the global point
  bool contains(vgl_point_3d<double> const& global_pt, vgl_point_3d<double>& local_tree_coords,
                vgl_point_3d<double>& cell_center, double& side_length) const;

  //: compute the data index for the cell containing the pt, also return cell octree depth and cell side length
  bool data_index(vgl_point_3d<double> const& global_pt, unsigned& index, unsigned& depth, double& side_length) const;

  //: just get the data index
  bool data_index(vgl_point_3d<double> const& global_pt, unsigned& index) const;

  //: retrieve a vector of cell centers and other info inside the specified bounding box, both in global world coordinates
  std::vector<cell_info> cells_in_box(vgl_box_3d<double> const& global_box);

  // find neigboring sub_block cell centers within a specified distance from probe, including the cell containing the probe
  std::vector<vgl_point_3d<double> > sub_block_neighbors(vgl_point_3d<double> const& probe, double distance) const;

  //: retrieve neighborhood of probe consisting of leaf cells. If relative_distance is true then the neighborhood distance
  // is scaled according to the size of the cell containing the probe. Otherwise the distance is absolute, e.g. meters.
  void leaf_neighbors(vgl_point_3d<double> const& probe, double distance, std::vector<vgl_point_3d<double> >& nbrs,
                      std::vector<double>& nbr_edge_lengths, std::vector<unsigned>& data_indices, bool relative_distance = true) const;
  /////
  //=== sub_block intersection methods ====//
  ////
  std::vector<vgl_point_3d<int> >  sub_blocks_intersect_box(vgl_box_3d<double> const& box) const;
  ////
 private:
  unsigned recompute_num_cells();
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
  vgl_point_3d<double> local_origin_;

  //: info about block's trees
  int init_level_;   //each sub_blocks's init level (default 1)
  int max_level_;    //each sub_blocks's max_level (default 4)
  int max_mb_;       //each total block mb
  unsigned n_cells_;
  bool read_only_;   // if the block existed already on the disc, do not write it back

  short version_;
};


//: Smart_Pointer typedef for boxm2_block
typedef vbl_smart_ptr<boxm2_block> boxm2_block_sptr;

//: output stream
std::ostream& operator <<(std::ostream &s, boxm2_block& block);

//: write to xml file
//void x_write(std::ostream &os, boxm2_block& scene, std::string name);

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
