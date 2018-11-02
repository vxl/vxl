#ifndef boxm_plane_obs_h_
#define boxm_plane_obs_h_
//:
// \file

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <boxm/boxm_aux_traits.h>

template<class T>
class boxm_plane_obs
{
 public:
  boxm_plane_obs() : seg_len_(0.0f), plane_(vgl_homg_plane_3d<T>()) {}
  boxm_plane_obs(vgl_homg_plane_3d<T> plane, float seg_len) : seg_len_(seg_len), plane_(plane) {}
  ~boxm_plane_obs() = default;
  static short version_no() { return 1; }
  void print(std::ostream& os) const { os << "(plane=" << plane_ << " seg_len=" << seg_len_ << ')'; }

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
std::ostream& operator << (std::ostream& os, const boxm_plane_obs<T>& sample);


#endif // boxm_plane_obs_h_
