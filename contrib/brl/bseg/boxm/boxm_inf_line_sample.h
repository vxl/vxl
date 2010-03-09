#ifndef boxm_inf_line_sample_h_
#define boxm_inf_line_sample_h_
//:
// \file

#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_line_3d_2_points.h>

#include <boxm/opt/boxm_aux_traits.h>

template<class T>
class boxm_inf_line_sample
{
 public:
  boxm_inf_line_sample() : 
       line_(vgl_infinite_line_3d<T>(vgl_vector_2d<T>(0,0),vgl_vector_3d<T>(10,10,10))),line_clipped_(vgl_point_3d<T>(0,0,0),vgl_point_3d<T>(0,0,0)), num_obs_(0){}
  boxm_inf_line_sample(vgl_infinite_line_3d<T> line,unsigned num_obs=0) : line_(line),num_obs_(num_obs),line_clipped_(vgl_point_3d<T>(0,0,0),vgl_point_3d<T>(0,0,0)) {}
  ~boxm_inf_line_sample() {}
  static short version_no() { return 1; }
  void print(vcl_ostream& os) { os << "(line=" << line_ << ')';  }

  // public members
   
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
vcl_ostream& operator << (vcl_ostream& os, boxm_inf_line_sample<T>& sample);


#endif // boxm_inf_line_sample_h_
