#ifndef vnl_rotation_matrix_h_
#define vnl_rotation_matrix_h_

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

class vnl_rotation_matrix {
 public:
  // returns exp([omega]_x), which is rotation by an angle ||omega||
  // about the axis given by the 3-vector omega.
  static vnl_matrix<double> exp_of_skew(const vnl_vector<double> &omega);
};

#endif
