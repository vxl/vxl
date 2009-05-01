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

#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_3d.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <boct/boct_tree.h>
#include <vcl_iosfwd.h>


class boxm_scene_parser;
template <class T> class boxm_block_iterator;

template <class T>
class boxm_scene :public boxm_scene_base
{
 public:
  boxm_scene() : active_block_(vgl_point_3d<int>(-1,-1,-1)) {}

  boxm_scene(const bgeo_lvcs& lvcs,
             const vgl_point_3d<double>& origin,
             const vgl_vector_3d<double>& block_dim,
             const vgl_vector_3d<double>& world_dim);

  boxm_scene(const bgeo_lvcs& lvcs,
             const vgl_point_3d<double>& origin,
             const vgl_vector_3d<double>& block_dim,
             const vgl_vector_3d<double>& world_dim, 
             unsigned max_level, unsigned init_level);

  //: when lvcs is not avialable
  boxm_scene( const vgl_point_3d<double>& origin,
              const vgl_vector_3d<double>& block_dim,
              const vgl_vector_3d<double>& world_dim);

  ~boxm_scene() {}

  void load_block(unsigned i, unsigned j, unsigned k);

  void load_block(vgl_point_3d<int> i) { load_block(i.x(), i.y(), i.z()); }

  void write_active_block();

  bgeo_lvcs lvcs() { return lvcs_;}

  boxm_block<T>* get_active_block();

  vgl_point_3d<double> origin() {return origin_;}

  vgl_vector_3d<double> block_dim() {return block_dim_;}

  void block_num(int &x, int &y, int &z) {x=(int) blocks_.get_row1_count();
                                          y=(int) blocks_.get_row2_count();
                                          z=(int) blocks_.get_row3_count();}

  vcl_string path() { return scene_path_; }

  vcl_string block_prefix() { return block_pref_; }

  void b_read(vsl_b_istream & s);

  void b_write(vsl_b_ostream& s) const;

  boxm_block<T>* get_block(vgl_point_3d<double>& p);

  bool get_block_index(vgl_point_3d<double>& p, vgl_point_3d<int> & index);

  boxm_block<T>* get_block(unsigned i, unsigned j, unsigned k) { return blocks_(i,j,k); }

  boxm_block<T>* get_block(vgl_point_3d<int>& idx) {return blocks_(idx.x(), idx.y(), idx.z()); }

  void write_scene();

  void load_scene(vcl_string filename);

  static short version_no() { return 1; }

  boxm_block_iterator<T> iterator() { boxm_block_iterator<T> iter(this); return iter;}

  vgl_box_3d<double> get_world_bbox();
  bool valid_index(vgl_point_3d<int> idx);

 private:
  bgeo_lvcs lvcs_;
  vgl_point_3d<double> origin_;
  vgl_vector_3d<double> block_dim_;
  vbl_array_3d<boxm_block<T>*> blocks_;

  //: index of the blocks (3D array) that is active; only one active block at a time
  vgl_point_3d<int> active_block_;

  //************** private methods
  void create_block(unsigned i, unsigned j, unsigned k);


  vgl_box_3d<double> get_block_bbox(int x, int y, int z);

  //: generates a name for the block binary file based on the 3D vector index
  vcl_string gen_block_path(int x, int y, int z);


  

  boxm_scene_parser* parse_config(vcl_string xml, bool filename=false);
};

template <class T>
class boxm_block_iterator
{
 public:
  boxm_block_iterator(boxm_scene<T>* const scene): i_(0), j_(0), k_(0), scene_(scene) {}

  ~boxm_block_iterator(){}

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

  vgl_point_3d<int> index(){return vgl_point_3d<int>(i_,j_,k_);}

 private:

  int i_;
  int j_;
  int k_;

  boxm_scene<T>* const scene_;
};

//: generates an XML file from the member variables
template <class T>
void x_write(vcl_ostream &os, boxm_scene<T> &scene, vcl_string name="boxm_scene");

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const &scene);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const * &scene);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> &scene);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> *&scene);


#endif
