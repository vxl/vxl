#ifndef brct_structure_estimator_h_
#define brct_structure_estimator_h_
//:
// \file
// \author Kongbin Kang
// \brief A class to incrementally compute 3D structure from 2D measurements.

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_2x3.h>
#include <vnl/vnl_double_3.h>
#include <bugl/bugl_gaussian_point_2d.h>
#include <bugl/bugl_gaussian_point_3d.h>

class brct_structure_estimator
{
 public:
    brct_structure_estimator(vnl_double_3x4 &P);
    ~brct_structure_estimator(){}

    //: go to the next step
    bugl_gaussian_point_3d<double> forward(
        bugl_gaussian_point_3d<double>& state,
        bugl_gaussian_point_2d<double>& observe);

    //: get measurement matrix
    vnl_double_2x3 get_H_matrix(vnl_double_3 &state);

 private:
    //: process variance
    vnl_double_3x3 Q_;

    //: state transit matrix
    vnl_double_3x3 A_;

    //: projective matrix
    vnl_double_3x4 P_;
};

#endif // brct_structure_estimator_h_
