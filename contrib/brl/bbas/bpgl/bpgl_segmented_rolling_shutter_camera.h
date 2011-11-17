// This is bbas/bpgl/bpgl_segmented_rolling_shutter_camera.h
#ifndef bpgl_segmented_rolling_shutter_camera_h_
#define bpgl_segmented_rolling_shutter_camera_h_
//:
// \file
// \brief A rolling shutter camera model approximated a list of perspective cameras
// \author Vishal Jain
// \date February 08, 2011
//

#include <vgl/vgl_fwd.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <bpgl/bpgl_rolling_shutter_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

template <class T>
class  bpgl_segmented_rolling_shutter_camera : public bpgl_rolling_shutter_camera<T>
{
 public:
  typedef typename vcl_map<unsigned int ,vpgl_perspective_camera<T> >  maptype;
  typedef typename maptype::iterator  iterator;
  typedef typename maptype::const_iterator const_iterator;

  // === Iterators ===

  iterator begin() { return cam_map_.begin(); }
  iterator end  () { return cam_map_.end(); }

  const_iterator begin() const { return cam_map_.begin();}
  const_iterator end  () const { return cam_map_.end(); }

  //: default constructor
  bpgl_segmented_rolling_shutter_camera();

  //: Constructor from a rational camera and a geographic origin
  bpgl_segmented_rolling_shutter_camera(maptype camera_segments);

  virtual ~bpgl_segmented_rolling_shutter_camera() {}

  virtual vcl_string type_name() const { return "bpgl_segmented_rolling_shutter_camera"; }

  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  virtual bpgl_segmented_rolling_shutter_camera<T>* clone(void) const;

  // === Mutators/Accessors ===

 //: project function for a point
  void project(const T x, const T y, const T z, T& u, T& v);

  //: The generic camera interface. u represents image column, v image row.
  virtual void project(const T x, const T y, const T z, T& u, T& v) const;

  // === Interface for vnl ===

  //: Project a world point onto the image
  virtual vnl_vector_fixed<T, 2> project(vnl_vector_fixed<T, 3> const& world_point) const;

  // === Interface for vgl ===

  //: Project a world point onto the image
  virtual vgl_point_2d<T> project(vgl_point_3d<T> world_point) const;

  //: project function for a point
  bool cam_index(T v, unsigned int & index);

  unsigned int num_segments() const {return this->cam_map_.size();}

  //: function to add a new camera
  bool add_camera(unsigned int v, vpgl_perspective_camera<double> cam);

  //: function to update an existing camera
  bool update_camera(unsigned int v, vpgl_perspective_camera<double> cam);

  //: print the camera parameters
  virtual void print(vcl_ostream& s = vcl_cout) const;

  //: save to file (the lvcs is after the global rational camera parameters)
  virtual bool save(vcl_string cam_path);

  // === binary IO ===

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

 protected:
  maptype cam_map_;
};

//: Creates a local rational camera from a file
// \relatesalso bpgl_segmented_rolling_shutter_camera
template <class T>
bpgl_segmented_rolling_shutter_camera<T>* read_segmented_rolling_shutter_camera(vcl_string cam_path);

//: Creates a local rational camera from a file
// \relatesalso bpgl_segmented_rolling_shutter_camera
template <class T>
bpgl_segmented_rolling_shutter_camera<T>* read_segmented_rolling_shutter_camera(vcl_istream& istr);

//: Write to stream
// \relatesalso bpgl_segmented_rolling_shutter_camera
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const bpgl_segmented_rolling_shutter_camera<T>& p);

//: Read from stream
// \relatesalso bpgl_segmented_rolling_shutter_camera
template <class T>
vcl_istream& operator>>(vcl_istream& is, bpgl_segmented_rolling_shutter_camera<T>& p);

#define BPGL_SEGMENTED_ROLLING_SHUTTER_CAMERA_INSTANTIATE(T) extern "please include bpgl/bpgl_segmented_rolling_shutter_camera.txx first"

#endif // bpgl_segmented_rolling_shutter_camera_h_
