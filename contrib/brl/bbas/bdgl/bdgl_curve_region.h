#ifndef bdgl_curve_region_h_
#define bdgl_curve_region_h_

//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
// \brief tracking of extracted vdgl_edgel_chains on sequences
//
// \verbatim
// Initial version February 12, 2003
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vgl/vgl_point_2d.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>


//-----------------------------------------------------------------------------
class bdgl_curve_region
{
 public:

  vgl_point_2d<double> center_;
  double               radius_;

  bdgl_curve_region()= default;
  bdgl_curve_region(vdgl_edgel_chain_sptr curve){ init(curve); }
  ~bdgl_curve_region()= default;

  void init(const vdgl_edgel_chain_sptr& curve);

  vgl_point_2d<double> get_center() const { return center_ ;}
  double get_radius() const { return radius_ ;}
  double x() const { return center_.x() ;}
  double y() const { return center_.y() ;}
  double r() const { return radius_ ;}
};

#endif // bdgl_curve_region_h_
