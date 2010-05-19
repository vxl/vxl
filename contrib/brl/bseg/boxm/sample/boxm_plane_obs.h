#ifndef boxm_plane_obs_h_
#define boxm_plane_obs_h_
//:
// \file

#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <boxm/boxm_aux_traits.h>

template<class T>
class boxm_plane_obs
{
 public:
  boxm_plane_obs() : seg_len_(0.0f), plane_(vgl_homg_plane_3d<T>()) {}
  boxm_plane_obs(vgl_homg_plane_3d<T> plane, float seg_len) : seg_len_(seg_len), plane_(plane) {}
  ~boxm_plane_obs() {}
  static short version_no() { return 1; }
  void print(vcl_ostream& os) const { os << "(plane=" << plane_ << " seg_len=" << seg_len_ << ')'; }

  // public members
  float seg_len_;
  vgl_homg_plane_3d<T> plane_;
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
vcl_ostream& operator << (vcl_ostream& os, const boxm_plane_obs<T>& sample);


#endif // boxm_plane_obs_h_
