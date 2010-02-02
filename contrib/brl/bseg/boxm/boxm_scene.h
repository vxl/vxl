#ifndef boxm_scene_h_
#define boxm_scene_h_
//:
// \file
// \brief  The main class to keep the 3D world data and pieces
//
// \author Gamze Tunali
// \date Apr 03, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "boxm_scene_base.h"
#include "boxm_block.h"
#include "boxm_sample.h"

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_3d.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <boct/boct_tree.h>
#include <vcl_string.h>
#include <vcl_iosfwd.h>


class boxm_scene_parser;
template <class T> class boxm_block_iterator;

template <class T>
class boxm_scene :public boxm_scene_base
{
 public:
  //: Default constructor
  boxm_scene() : active_block_(vgl_point_3d<int>(-1,-1,-1)), save_internal_nodes_(false) {}

  //: Constructor from lvcs, world origin, dimension of the block (in world coordinates) and number of blocks(world_dim)
  boxm_scene(const bgeo_lvcs& lvcs,
             const vgl_point_3d<double>& origin,
             const vgl_vector_3d<double>& block_dim,
             const vgl_vector_3d<unsigned>& world_dim,
             const bool save_internal_nodes_ = false);

  //: Constructor from lvcs, world origin, dimension of the block, number of blocks and initialization level
  boxm_scene(const bgeo_lvcs& lvcs,
             const vgl_point_3d<double>& origin,
             const vgl_vector_3d<double>& block_dim,
             const vgl_vector_3d<unsigned>& world_dim,
             unsigned max_level, unsigned init_level,
             const bool save_internal_nodes_ = false);

  //: Constructor when lvcs is not avialable. World origin, block dimensions and number of blocks
  boxm_scene( const vgl_point_3d<double>& origin,
              const vgl_vector_3d<double>& block_dim,
              const vgl_vector_3d<unsigned>& world_dim,
              const bool save_internal_nodes_ = false);

  //: Destructor
  ~boxm_scene();

  bool discover_block(unsigned i, unsigned j, unsigned k);

  bool load_block(unsigned i, unsigned j, unsigned k);

  void load_block(vgl_point_3d<int> i) { load_block(i.x(), i.y(), i.z()); }

  void write_active_block();

  bgeo_lvcs lvcs() const { return lvcs_; }

  boxm_block<T>* get_active_block();

  vgl_point_3d<double> origin() const { return origin_; }

  void set_origin(vgl_point_3d<double> o) { origin_=o; }

  vgl_vector_3d<double> block_dim() const { return block_dim_; }
  
  bool save_internal_nodes() const {return save_internal_nodes_;}

  void block_num(int &x, int &y, int &z) {
    x=(int) blocks_.get_row1_count();
    y=(int) blocks_.get_row2_count();
    z=(int) blocks_.get_row3_count();
  }

  vgl_vector_3d<unsigned> world_dim() const {
    unsigned x=(unsigned) blocks_.get_row1_count();
    unsigned y=(unsigned) blocks_.get_row2_count();
    unsigned z=(unsigned) blocks_.get_row3_count();
    return vgl_vector_3d<unsigned>(x,y,z);
  }

  vcl_string path() const { return scene_path_; }

  vcl_string block_prefix() const { return block_pref_; }

  void set_path(vcl_string path, vcl_string block_prefix) { scene_path_=path; block_pref_= block_prefix; }

  void b_read(vsl_b_istream & s);

  void b_write(vsl_b_ostream& s) const;

  boxm_block<T>* get_block(vgl_point_3d<double>& p);

  bool get_block_index(vgl_point_3d<double>& p, vgl_point_3d<int> & index);

  //: what is the use of this?
  boxm_block<T>* get_block(unsigned i, unsigned j, unsigned k) { return blocks_(i,j,k); }

  boxm_block<T>* get_block(vgl_point_3d<int>& idx) { return blocks_(idx.x(), idx.y(), idx.z()); }

  void set_block(vgl_point_3d<int> const& idx, boxm_block<T>* block)
  { blocks_(idx.x(),idx.y(),idx.z()) = block; active_block_=idx; }

  void write_scene(vcl_string filename = "/scene.xml");

  void load_scene(vcl_string filename);

  void load_scene(boxm_scene_parser& parser);

  static short version_no() { return 1; }

  boxm_block_iterator<T> iterator() { boxm_block_iterator<T> iter(this); return iter; }

  virtual vgl_box_3d<double> get_world_bbox();

  bool valid_index(vgl_point_3d<int> idx);

  vgl_box_3d<double> get_block_bbox(int x, int y, int z);

  //: generates a name for the block binary file based on the 3D vector index
  vcl_string gen_block_path(int x, int y, int z);
 
  void clean_scene();

  vgl_point_3d<double> rpc_origin() const { return rpc_origin_; }

  void set_rpc_origin(vgl_point_3d<double>& new_rpc_origin) {
    vgl_point_3d<double> old_origin = origin();
    vgl_point_3d<double> new_origin(
      old_origin.x() + new_rpc_origin.x() - rpc_origin_.x(),
      old_origin.y() + new_rpc_origin.y() - rpc_origin_.y(),
      old_origin.z() + new_rpc_origin.z() - rpc_origin_.z());
    set_origin(new_origin);
    rpc_origin_ = new_rpc_origin;
  }

 protected:
  bgeo_lvcs lvcs_;
  vgl_point_3d<double> origin_;
  vgl_point_3d<double> rpc_origin_;
  //: World dimensions of a block .e.g 1 meter x 1 meter x 1 meter
  vgl_vector_3d<double> block_dim_;
  vbl_array_3d<boxm_block<T>*> blocks_;

  //: index of the blocks (3D array) that is active; only one active block at a time
  vgl_point_3d<int> active_block_;

  //: Flag that indicates whether internal nodes of the trees should be saved
  bool save_internal_nodes_;
  //************** private methods
  void create_block(unsigned i, unsigned j, unsigned k);

  void create_blocks(const vgl_vector_3d<double>& block_dim, const vgl_vector_3d<unsigned>& world_dim);
  bool parse_config(boxm_scene_parser& parser);

  bool parse_xml_string(vcl_string xml, boxm_scene_parser& parser);
};

template <class T>
class boxm_block_iterator
{
 public:
  boxm_block_iterator(boxm_scene<T>* const scene): i_(0), j_(0), k_(0), scene_(scene) {}

  ~boxm_block_iterator() {}

  boxm_block_iterator<T>& begin();

  bool end();

  boxm_block_iterator<T>& operator=(const boxm_block_iterator<T>& that);

  bool operator==(const boxm_block_iterator<T>& that);

  bool operator!=(const boxm_block_iterator<T>& that);

  boxm_block_iterator<T>& operator++();  // pre-inc

  boxm_block_iterator<T> operator++(int); // post-inc

  boxm_block_iterator<T>& operator--();

  boxm_block<T>* operator*();

  boxm_block<T>* operator->();

  vgl_point_3d<int> index() const { return vgl_point_3d<int>(i_,j_,k_); }

 private:
  int i_;
  int j_;
  int k_;

  boxm_scene<T>* const scene_;
};

//: generates an XML file from the member variables
template <class T>
void x_write(vcl_ostream &os, boxm_scene<T>& scene, vcl_string name="boxm_scene");

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const &scene);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const * &scene);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> &scene);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> *&scene);


#endif // boxm_scene_h_
