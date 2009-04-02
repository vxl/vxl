#ifndef boxm_scene_txx_
#define boxm_scene_txx_

#include "boxm_scene.h"

#include <vcl_cmath.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vsl/vsl_basic_xml_element.h>

#define NULL 0

template <class T>
boxm_scene<T>::boxm_scene(const bgeo_lvcs& lvcs, const vgl_point_3d<double>& origin, 
    const vgl_vector_3d<double>& block_dim, const vgl_vector_3d<double>& world_dim)
: lvcs_(lvcs), block_dim_(block_dim), origin_(origin), scene_path_(""), block_pref_("")
{
	// compute the dimensions of 3D array
	int x_dim = vcl_floor(world_dim.x()/block_dim.x());
	int y_dim = vcl_floor(world_dim.y()/block_dim.y());
	int z_dim = vcl_floor(world_dim.z()/block_dim.z());
	
	// pointers are initialized to NULL
	blocks_ =  vbl_array_3d<boxm_block<T>*>((unsigned)x_dim, (unsigned)y_dim, (unsigned)z_dim, (boxm_block<T>*)NULL);
	
}

template <class T>
void boxm_scene<T>::create_block(unsigned i, unsigned j, unsigned k)
{
  if (blocks_(i,j,k) == NULL) {
    vgl_point_3d<double> min(i*block_dim_.x(), j*block_dim_.y(), k*block_dim_.z());
    vgl_point_3d<double> max(min.x()+block_dim_.x(), min.y()+block_dim_.y(), min.z()+block_dim_.z());
    
    vgl_box_3d<double> bbox(min, max);
    blocks_(i,j,k) = new boxm_block<T>(bbox);
    }
}

//: returns the block this point resides in
template <class T>
boxm_block<T>* boxm_scene<T>::get_block(const vgl_point_3d<double>& p)
{
  vgl_box_3d<double> world(get_world_bbox());
  if (world.contains(p)) {
    // find the block index
    unsigned i = (p.x()-origin_.x())/block_dim_.x();
    unsigned j = (p.y()-origin_.y())/block_dim_.y();
    unsigned k = (p.z()-origin_.z())/block_dim_.z();
    return blocks_(i,j,k);
  } else {
    vcl_cerr << "Point " << p << " is out of world " << world << vcl_endl;
    return 0;
  }
}

template <class T>
vgl_box_3d<double> boxm_scene<T>::get_world_bbox()
{
  vgl_point_3d<double> min(origin_.x(), origin_.y(), origin_.z());
  vgl_point_3d<double> max(min.x()+block_dim_.x()*blocks_.get_row1_count(), 
                           min.y()+block_dim_.y()*blocks_.get_row2_count(), 
                           min.z()+block_dim_.z()*blocks_.get_row3_count());
    
  vgl_box_3d<double> bbox(min, max);
  return bbox;
}

template <class T>
vcl_string boxm_scene<T>::gen_block_path(int x, int y, int z)
{
  vcl_stringstream strm;

  strm << x << "_" << y << "_" << z;
  vcl_string str(strm.str());
  vcl_string s = block_pref_+ '_' + str + ".bin";
  return s;
}
  
template <class T>
void boxm_scene<T>::x_write(vcl_ostream &os)
{
    
  vsl_basic_xml_element scene("scene");
  scene.x_write_open(os);
  lvcs_.x_write(os, "lvcs");
  //x_write(os, (vgl_point_3d<double>)this->origin_, "local_origin");
  //x_write(os, (vgl_vector_3d<double>)this->block_dim_, "block_dimensions"); 
  
  vsl_basic_xml_element blocks("blocks");
  blocks.add_attribute("row1_count", (int) blocks_.get_row1_count());
  blocks.add_attribute("row2_count", (int) blocks_.get_row2_count());
  blocks.add_attribute("row3_count", (int) blocks_.get_row3_count());
  blocks.x_write_open(os);
  for (unsigned i=0; i<blocks_.get_row1_count(); i++) {
    for (unsigned j=0; j<blocks_.get_row2_count(); j++) {
      for (unsigned k=0; j<blocks_.get_row3_count(); k++) {
        vsl_basic_xml_element block(gen_block_path(i,j,k));
        block.append_cdata(gen_block_path(i,j,k));
        block.x_write(os);
      }
    }
  }
  blocks.x_write_close(os);      
  scene.x_write_close(os);
}

template <class T>
void boxm_scene<T>::b_read(vsl_b_istream & s)
{
}

template <class T>
void boxm_scene<T>::b_write(vsl_b_ostream & s)
{
}

#define BOXM_SCENE_INSTANTIATE(T) \
template boxm_scene<T>; 

#endif