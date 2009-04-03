#ifndef boxm_scene_txx_
#define boxm_scene_txx_
//:
// \file
#include "boxm_scene.h"
#include "io/boxm_scene_parser.h"

#include <vcl_cmath.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vsl/vsl_binary_io.h>

#define NULL 0

template <class T>
boxm_scene<T>::boxm_scene(const bgeo_lvcs& lvcs, const vgl_point_3d<double>& origin,
    const vgl_vector_3d<double>& block_dim, const vgl_vector_3d<double>& world_dim)
: lvcs_(lvcs), block_dim_(block_dim), origin_(origin), scene_path_(""), block_pref_(""),
active_block_(vgl_point_3d<int>(-1,-1,-1))
{
  // compute the dimensions of 3D array
  int x_dim = vcl_floor(world_dim.x()/block_dim.x());
  int y_dim = vcl_floor(world_dim.y()/block_dim.y());
  int z_dim = vcl_floor(world_dim.z()/block_dim.z());

  // pointers are initialized to NULL
  blocks_ =  vbl_array_3d<boxm_block<T>*>((unsigned)x_dim, (unsigned)y_dim, (unsigned)z_dim, (boxm_block<T>*)NULL);
}

template <class T>
boxm_scene<T>::boxm_scene( const vgl_point_3d<double>& origin,
    const vgl_vector_3d<double>& block_dim, const vgl_vector_3d<double>& world_dim)
: block_dim_(block_dim), origin_(origin), scene_path_(""), block_pref_(""),
active_block_(vgl_point_3d<int>(-1,-1,-1))
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
boxm_block<T>* boxm_scene<T>::get_active_block()
{
	if(valid_index(active_block_))
		return blocks_(active_block_.x(),active_block_.y(),active_block_.z());
	else{
		vcl_cerr << "index"<<active_block_<<"  is out of world " <<  vcl_endl;
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

  strm << x << '_' << y << '_' << z;
  vcl_string str(strm.str());
  vcl_string s = scene_path_ + '/' + block_pref_+ '_' + str + ".bin";
  return s;
}

template <class T>
void boxm_scene<T>::load_block(unsigned i, unsigned j, unsigned k)
{
  if (!valid_index(vgl_point_3d<int>(i,j,k)))
    return;

  // make sure the active one is saved first
  if (valid_index(active_block_)) {
    if (active_block_ == vgl_point_3d<int>(i,j,k))
      return;
    else {
      vcl_cout<<"Writing of the old block";vcl_cout.flush();
      int x=active_block_.x(), y=active_block_.y(), z=active_block_.z();
      vcl_string path = gen_block_path(x,y,z);
      vsl_b_ofstream os(path);
      blocks_(x,y,z)->b_write(os);
      // delete the block's data
      boxm_block<T>* block = blocks_(x,y,z);
      delete block;
    }
  }
  active_block_.set(i,j,k);
  
  vcl_string block_path = gen_block_path(i,j,k);
  vsl_b_ifstream os(block_path);

  vcl_cout<<"Block Path: "<<block_path<<vcl_endl;
  vcl_cout.flush();

  //if the binary block file is not found
  if (!os) {
    vcl_cout<<"Create a new block";vcl_cout.flush();
    create_block(i,j,k);
    return;
  }

  blocks_(i,j,k)->b_read(os);
  os.close();
}

template <class T>
bool boxm_scene<T>::valid_index(vgl_point_3d<int> idx)
{
  vgl_point_3d<int> min(0,0,0);
  vgl_point_3d<int> max(blocks_.get_row1_count(), blocks_.get_row2_count(), blocks_.get_row3_count());
  vgl_box_3d<int> bbox(min, max);
  if (bbox.contains(idx))
    return true;

  return false;
}


template <class T>
void boxm_scene<T>::b_read(vsl_b_istream & is)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vcl_string xml="";
      vsl_b_read(is, xml);
      vcl_cout << xml << vcl_endl;
      boxm_scene_parser* parser = parse_config(xml);
      if (parser)
      {
        parser->lvcs(lvcs_);
        origin_ = parser->origin();
        block_dim_ = parser->block_dim();
        vgl_vector_3d<unsigned> nums = parser->block_nums();
        blocks_ =  vbl_array_3d<boxm_block<T>*>(nums.x(), nums.y(), nums.z(), (boxm_block<T>*)NULL);
        parser->paths(scene_path_, block_pref_);
      }
      break;
#if 0
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_scene<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
#endif
  }
}

template <class T>
void boxm_scene<T>::b_write(vsl_b_ostream & s)
{
  // create an XML stream for the parameters
  vcl_stringstream strm;
  x_write(strm, *this, "scene");
  vcl_string str(strm.str());

  vsl_b_write(s, version_no());
  // write the XML as char stream
  vsl_b_write(s, str);
}

template <class T>
void x_write(vcl_ostream &os, boxm_scene<T>& scene, vcl_string name)
{
  vsl_basic_xml_element scene_elm(name);
  scene_elm.x_write_open(os);
  scene.lvcs().x_write(os, LVCS_TAG);
  x_write(os, scene.origin(), LOCAL_ORIGIN_TAG);
  x_write(os, scene.block_dim(), BLOCK_DIMENSIONS_TAG);

  vsl_basic_xml_element blocks(BLOCK_NUM_TAG);
  int x_dim, y_dim, z_dim;
  scene.block_num(x_dim, y_dim, z_dim);
  blocks.add_attribute("x_dimension", x_dim);
  blocks.add_attribute("y_dimension", y_dim);
  blocks.add_attribute("z_dimension", z_dim);
  blocks.x_write(os);
  vsl_basic_xml_element paths(SCENE_PATHS_TAG);
  paths.add_attribute("path", scene.path());
  paths.add_attribute("block_prefix", scene.block_prefix());
  paths.x_write(os);
  scene_elm.x_write_close(os);
}

template <class T>
boxm_scene_parser* boxm_scene<T>::parse_config(vcl_string xml)
{
  if (xml.size() == 0) {
    vcl_cerr << "XML string is empty\n";
    return 0;
  }

  boxm_scene_parser* parser = new boxm_scene_parser();
  if (!parser->parseString(xml.data())) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << vcl_endl;

    delete parser;
    return 0;
  }
  vcl_cout << "finished!" << vcl_endl;
  return parser;
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const &scene)
{}
template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const * &scene)
{}
template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> &scene)
{}
template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> *&scene)
{}

#define BOXM_SCENE_INSTANTIATE(T) \
template boxm_scene<T >; \
template void x_write(vcl_ostream&, boxm_scene<T >&, vcl_string);\
template void vsl_b_write(vsl_b_ostream & os, boxm_scene<T > const &scene);\
template void vsl_b_write(vsl_b_ostream & os, boxm_scene<T > const * &scene);\
template void vsl_b_read(vsl_b_istream & is, boxm_scene<T >  &scene);\
template void vsl_b_read(vsl_b_istream & is, boxm_scene<T > * &scene)

#endif
