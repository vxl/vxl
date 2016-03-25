#ifndef boxm2_vecf_geometry_base_h_
#define boxm2_vecf_geometry_base_h_
//:
// \file
// \brief  The geometry_base class
//
// \author J.L. Mundy
// \date   4 Dec 2015
//
#include <iostream>
#include <string>
#include <vector>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vbl/vbl_ref_count.h>
class boxm2_vecf_geometry_base: public vbl_ref_count{

  //: the functor operator for surface distance
 // can be away from the closest point in the cross-section pointset.
  virtual double operator() (vgl_point_3d<double> const& p) const = 0;

 //: inverse vector field
  virtual bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const = 0;
};
#endif // boxm2_vecf_geometry_base_h_
