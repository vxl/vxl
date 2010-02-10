#ifndef boxm_edge_tangent_sample_h_
#define boxm_edge_tangent_sample_h_
//:
// \file

#include <vcl_iostream.h>
#include <vil/vil_rgb.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_infinite_line_3d.h>
#include <boxm/opt/boxm_aux_traits.h>

#include <vcl_vector.h>

template<class T>
class boxm_plane_obs
{
public:
  boxm_plane_obs() : seg_len_(0.0f), plane_(vgl_homg_plane_3d<T>()) {}
  boxm_plane_obs(vgl_homg_plane_3d<T> plane, float seg_len) : seg_len_(seg_len), plane_(plane) {}
  ~boxm_plane_obs(){}
  static short version_no() { return 1; }
  void print(vcl_ostream& os) { os << "(plane=" << plane_ << " seg_len=" << seg_len_ << ')'; }

  //: public members
  float seg_len_;
  vgl_homg_plane_3d<T> plane_;
  
};

template<class T>
class boxm_edge_tangent_sample
{
 public:
  boxm_edge_tangent_sample() {}
  ~boxm_edge_tangent_sample() {}
  static short version_no() { return 1; }
  void print(vcl_ostream& os);

  //: adds observations
  void insert(boxm_plane_obs<T> obs) { obs_.push_back(obs); /*vcl_cout << obs_.size() << vcl_endl;*/ }

  //: returns the number of observations
  unsigned num_obs() { return obs_.size(); }

  boxm_plane_obs<T> obs(unsigned int i) { return obs_[i]; }

  //: returns a basic value that represents this sample (mostly for raw file creation purposes)
  float basic_val(unsigned int i) { return obs_[i].seg_len_; }

private:
  vcl_vector<boxm_plane_obs<T> > obs_;
};

template<class T>
class boxm_inf_line_sample
{
public:
  boxm_inf_line_sample() : line_(vgl_infinite_line_3d<T>()) {}
  boxm_inf_line_sample(vgl_infinite_line_3d<T> line) : line_(line) {}
  ~boxm_inf_line_sample(){}
  static short version_no() { return 1; }
  void print(vcl_ostream& os) { os << "(line=" << line_ << ')'; }

  //: public members
  vgl_infinite_line_3d<T> line_;
  
};

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_plane_obs<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_plane_obs<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_plane_obs<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_plane_obs<T> *&sample);

template <class T>
vcl_ostream& operator << (vcl_ostream& os, boxm_plane_obs<T>& sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_tangent_sample<T>& sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_tangent_sample<T>* &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_tangent_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_tangent_sample<T> *&sample);

template <class T>
vcl_ostream& operator << (vcl_ostream& os, boxm_edge_tangent_sample<T> sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_inf_line_sample<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_inf_line_sample<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_inf_line_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_inf_line_sample<T> *&sample);

template <class T>
vcl_ostream& operator << (vcl_ostream& os, boxm_inf_line_sample<T>& sample);


#endif // boxm_edge_tangent_sample_h_
