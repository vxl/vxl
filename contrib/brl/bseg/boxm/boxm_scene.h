#ifndef boxm_scene_h_
#define boxm_scene_h_

#include "boxm_block.h"
#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vbl/vbl_array_3d.h>
#include <vpgl/bgeo/bgeo_lvcs.h>

template <class T>
class boxm_scene
{
public:
  
  boxm_scene() : scene_path_(""), block_pref_("") {}

  boxm_scene(const bgeo_lvcs& lvcs, const vgl_point_3d<double>& origin, 
    const vgl_vector_3d<double>& block_dim, const vgl_vector_3d<double>& world_dim);

  void set_paths(vcl_string scene_path, vcl_string block_prefix) 
  { scene_path_ = scene_path;  block_pref_=block_prefix; }

  ~boxm_scene() {}

  void b_read(vsl_b_istream & s);
  void b_write(vsl_b_ostream & s);

private:
  bgeo_lvcs lvcs_;
  vgl_point_3d<double> origin_;
  vgl_vector_3d<double> block_dim_;
  vbl_array_3d<boxm_block<T>*> blocks_;
  vcl_string scene_path_;
  vcl_string block_pref_;

  // private methods
  void create_block(unsigned i, unsigned j, unsigned k);

  boxm_block<T>* get_block(const vgl_point_3d<double>& p);

  vgl_box_3d<double> get_world_bbox();

  //: generates a name for the block binary file based on the 3D vector index
  vcl_string gen_block_path(int x, int y, int z);

  //: generates an XML file from the member variables
  void x_write(vcl_ostream &os);

};

#endif