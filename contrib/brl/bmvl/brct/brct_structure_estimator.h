#ifndef brct_structure_estimator_h_
#define brct_structure_estimator_h_

// \file
// \author Kongbin Kang
// \brief A class to increamentally compute 3D structure 
// from 2D masurements.

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <bugl/bugl_gaussian_point_2d.h>
#include <bugl/bugl_gaussian_point_3d.h>

class brct_structure_estimator
{
  private:
    //: state transit matrix
    vnl_double_3x3 A_;

    //: projective matrix
    vnl_double_3x4 P_;
    
  public:
    brct_structure_estimator() {}
    ~brct_structure_estimator(){}
};

#endif // structure_estimator_h_
