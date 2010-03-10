#ifndef boxm_line_samples_h_
#define boxm_line_samples_h_
//:
// \file

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>

//#include <vgl/vgl_infinite_line_3d.h>
//#include <vgl/vgl_line_3d_2_points.h>

//#include <boxm/opt/boxm_aux_traits.h>
//#include <bsta/bsta_kent.h>

#define SIZE 5

template<class T>
class boxm_line_samples
{
 public:
  boxm_line_samples() : num_obs_(vcl_vector<int>(SIZE,-1)), smallest_index_(-1),
    x_(vcl_vector<T>(SIZE)), y_(vcl_vector<T>(SIZE)), theta_(vcl_vector<T>(SIZE)) {}

  ~boxm_line_samples() {}

  void insert(int num_obs, T x, T y, T theta);

  static short version_no() { return 1; }

  void print(vcl_ostream& os);

  bool operator==(const boxm_line_samples &rhs) const;

  typedef boxm_line_samples<T> value_type;

public:
  vcl_vector<int> num_obs_;
  int smallest_index_;
  vcl_vector<T> x_;
  vcl_vector<T> y_;
  vcl_vector<T> theta_;
};

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_line_samples<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_line_samples<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_line_samples<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_line_samples<T> *&sample);

template <class T>
vcl_ostream& operator << (vcl_ostream& os, boxm_line_samples<T>& sample);


#endif // boxm_line_samples_h_
