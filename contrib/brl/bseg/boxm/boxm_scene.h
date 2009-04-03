#ifndef boxm_scene_h_
#define boxm_scene_h_

#include "boxm_block.h"
#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_3d.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <boct/boct_tree.h>

class boxm_scene_parser;

template <class T>
class boxm_scene:public vbl_ref_count
{
public:
  
  boxm_scene() : scene_path_(""), block_pref_("") {}

  boxm_scene(const bgeo_lvcs& lvcs, 
             const vgl_point_3d<double>& origin, 
             const vgl_vector_3d<double>& block_dim, 
             const vgl_vector_3d<double>& world_dim);
  //: when lvcs is not avialable 
  boxm_scene( const vgl_point_3d<double>& origin, 
              const vgl_vector_3d<double>& block_dim, 
              const vgl_vector_3d<double>& world_dim);


  void set_paths(vcl_string scene_path, vcl_string block_prefix) 
  { scene_path_ = scene_path;  block_pref_=block_prefix; }

  ~boxm_scene() {}
  bgeo_lvcs lvcs() { return lvcs_;}
  vgl_point_3d<double> origin() {return origin_;}
  vgl_vector_3d<double> block_dim() {return block_dim_;}
  void block_num(int &x, int &y, int &z) {x=(int) blocks_.get_row1_count();
                                          y=(int) blocks_.get_row2_count(); 
                                          z=(int) blocks_.get_row3_count();}
  vcl_string path() { return scene_path_; }
  vcl_string block_prefix() { return block_pref_; }

  void b_read(vsl_b_istream & s);
  void b_write(vsl_b_ostream & s);
  boxm_block<T>* get_block(const vgl_point_3d<double>& p);
  
  short version_no() { return 1; }

private:
  bgeo_lvcs lvcs_;
  vgl_point_3d<double> origin_;
  vgl_vector_3d<double> block_dim_;
  vbl_array_3d<boxm_block<T>*> blocks_;
  vcl_string scene_path_;
  vcl_string block_pref_;

  // private methods
  void create_block(unsigned i, unsigned j, unsigned k);


  void load_block_binary(unsigned i, unsigned j, unsigned k);

  vgl_box_3d<double> get_world_bbox();

  //: generates a name for the block binary file based on the 3D vector index
  vcl_string gen_block_path(int x, int y, int z);

  boxm_scene_parser* parse_config(vcl_string xml);
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

typedef vbl_smart_ptr<boxm_scene<boct_tree<short,vgl_point_3d<double> > > > boxm_scene_short_point_double_sptr;


#endif