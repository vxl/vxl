#ifndef boxm_inf_line_sample_h_
#define boxm_inf_line_sample_h_
//:
// \file

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_line_3d_2_points.h>

#include <boxm/boxm_aux_traits.h>
#include <bsta/bsta_kent.h>

template<class T>
class boxm_inf_line_sample
{
 public:
  //: initially the residual is very big, it means the solution to this inf line is not found yet
  boxm_inf_line_sample() :
       residual_(1e6), line_(vgl_infinite_line_3d<T>(vgl_vector_2d<T>(0,0),
       vgl_vector_3d<T>(10,10,10))), num_obs_(0){}

  boxm_inf_line_sample(vgl_infinite_line_3d<T> line,unsigned num_obs=0) : residual_(1e6),line_(line),num_obs_(num_obs) {}
  ~boxm_inf_line_sample() = default;
  static short version_no() { return 1; }
  void print(std::ostream& os) { os << "(line=" << line_ << ')';  }
  bool operator==(const boxm_inf_line_sample &rhs) const;
  typedef boxm_inf_line_sample<T> value_type;
  // public members
  T residual_;
  vgl_infinite_line_3d<T> line_;
  vgl_line_3d_2_points<T> line_clipped_;
  unsigned num_obs_;
};

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_inf_line_sample<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_inf_line_sample<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_inf_line_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_inf_line_sample<T> *&sample);

template <class T>
std::ostream& operator << (std::ostream& os, boxm_inf_line_sample<T>& sample);


#endif // boxm_inf_line_sample_h_
