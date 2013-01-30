// This is bbas/bpgl/bpgl_segmented_rolling_shutter_camera.txx
#ifndef bpgl_segmented_rolling_shutter_camera_txx_
#define bpgl_segmented_rolling_shutter_camera_txx_
//:
// \file
#include "bpgl_segmented_rolling_shutter_camera.h"
#include <vcl_vector.txx>
#include <vcl_fstream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
//--------------------------------------
// Constructors
//

// Create an identity projection, i.e. (x,y,z) identically maps to (x,y)
template <class T>
bpgl_segmented_rolling_shutter_camera<T>::bpgl_segmented_rolling_shutter_camera():cam_map_()
{
    vpgl_perspective_camera<T> default_cam;
    cam_map_[0]=default_cam;
}

//: Constructor from a rational camera and an affine matrix
template <class T>
bpgl_segmented_rolling_shutter_camera<T>::bpgl_segmented_rolling_shutter_camera(maptype camera_segments):
cam_map_(camera_segments)
{
}

template <class T>
bpgl_segmented_rolling_shutter_camera<T>* bpgl_segmented_rolling_shutter_camera<T>::clone(void) const
{
  return new bpgl_segmented_rolling_shutter_camera<T>(*this);
}

// Base projection method, x, y, z are in local Cartesian coordinates
template <class T>
void bpgl_segmented_rolling_shutter_camera<T>::project(const T x, const T y, const T z,T& u, T& v) const
{
   this->project(x,y,z,u,v);
}

template <class T>
void bpgl_segmented_rolling_shutter_camera<T>::project(const T x, const T y, const T z,T& u, T& v)
{
  for (iterator iter=cam_map_.begin();iter!=cam_map_.end(); )
  {
    iter->second.project(x,y,z,u,v);
    ++iter;

    if (cam_map_.upper_bound((unsigned int)v)==iter)
      return;
  }
}

//vnl interface methods
template <class T>
vnl_vector_fixed<T, 2>
bpgl_segmented_rolling_shutter_camera<T>::project(vnl_vector_fixed<T, 3> const& world_point)const
{
  vnl_vector_fixed<T, 2> image_point;
  this->project(world_point[0],world_point[1],world_point[2],image_point[0],image_point[1]);
  return image_point;
}

//vgl interface methods
template <class T>
vgl_point_2d<T> bpgl_segmented_rolling_shutter_camera<T>::project(vgl_point_3d<T> world_point)const
{
  T u = 0, v = 0;
  this->project(world_point.x(),world_point.y(),world_point.z(),u,v);
  return vgl_point_2d<T>(u, v);
}

template <class T>
bool bpgl_segmented_rolling_shutter_camera<T>::cam_index(T v, unsigned int & index)
{
    index=0;
    for (iterator iter=cam_map_.begin();iter!=cam_map_.end();)
    {
        ++iter;
        if (cam_map_.upper_bound((unsigned int)v)==iter)
            return true;
        ++index;
    }

    return false;
}

template <class T>
bool bpgl_segmented_rolling_shutter_camera<T>::add_camera(unsigned int v, vpgl_perspective_camera<double> cam)
{
  if (cam_map_.find(v)==cam_map_.end())
  { cam_map_[v]=cam; return true; }
  else
    return false;
}

template <class T>
bool bpgl_segmented_rolling_shutter_camera<T>::update_camera(unsigned int v, vpgl_perspective_camera<double> cam)
{
  if (cam_map_.find(v)!=cam_map_.end())
  { cam_map_[v]=cam; return true; }
  else
    return false;
}

//: print the camera parameters
template <class T>
void bpgl_segmented_rolling_shutter_camera<T>::print(vcl_ostream& s) const
{
  s << "<bpgl_segmented_rolling_shutter_camera>";
}

template <class T>
bool bpgl_segmented_rolling_shutter_camera<T>::save(vcl_string cam_path)
{
  vcl_ofstream file_out;
  file_out.open(cam_path.c_str());
  if (!file_out.good()) {
    vcl_cerr << "error: bad filename: " << cam_path << vcl_endl;
    return false;
  }
  else
    return true;
}

// Binary I/O

//: Binary read self from stream
template <class T> void bpgl_segmented_rolling_shutter_camera<T>::
b_read(vsl_b_istream &is)
{
  return;
}


//: Binary save self to stream.
template <class T> void bpgl_segmented_rolling_shutter_camera<T>::
b_write(vsl_b_ostream &os) const
{
  return;
}

// read from a file
template <class T>
bpgl_segmented_rolling_shutter_camera<T>* read_segmented_rolling_shutter_camera(vcl_string cam_path)
{
  return new bpgl_segmented_rolling_shutter_camera<T>();
}

//: read from an open istream
template <class T>
bpgl_segmented_rolling_shutter_camera<T>* read_segmented_rolling_shutter_camera(vcl_istream& istr)
{
  return new bpgl_segmented_rolling_shutter_camera<T>();
}


//: Write to stream
template <class T>
vcl_ostream&  operator<<(vcl_ostream& s, const bpgl_segmented_rolling_shutter_camera<T>& c )
{
  c.print(s);
  return s;
}


// Code for easy instantiation.
#undef BPGL_SEGMENTED_ROLLING_SHUTTER_CAMERA_INSTANTIATE
#define BPGL_SEGMENTED_ROLLING_SHUTTER_CAMERA_INSTANTIATE(T) \
template class bpgl_segmented_rolling_shutter_camera<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const bpgl_segmented_rolling_shutter_camera<T >&); \
template bpgl_segmented_rolling_shutter_camera<T >* read_segmented_rolling_shutter_camera(vcl_string); \
template bpgl_segmented_rolling_shutter_camera<T >* read_segmented_rolling_shutter_camera(vcl_istream&)

#endif // bpgl_segmented_rolling_shutter_camera_txx_
