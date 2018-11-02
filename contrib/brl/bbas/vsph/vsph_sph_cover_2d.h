//This is brl/bbas/vsph/vsph_sph_cover_2d.h
#ifndef vsph_sph_cover_2d_h_
#define vsph_sph_cover_2d_h_
//:
// \file
// \brief A set of vsph_sph_box_2d (axis-aligned spherical boxes) whose union covers an arbitrary spherical region defined by vsph_unit_sphere rays
//
// \author J.L. Mundy
// \date March 2, 2013
// \verbatim
//  Modifications
// None
// \endverbatim
// The parameter, min_area_fraction, determines if a box in the cover is to
// be subdivided. Boxes containing a fractional area of the spherical region
// rays less than min_area_fraction are subdivided until: 1) a limit on the
// extent of box dimensions is reached (box too small); or 2) the fractional
// area of the region contained in the box is greater than or equal to
// min_area_fraction.
//
#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsph/vsph_sph_box_2d.h>

// A struct associating an axis aligned box with an area fraction of coverage.
// Forms the elements of the cover.
class cover_el
{
 public:
  cover_el():frac_inside_(0.0) {}
  cover_el(vsph_sph_box_2d const& box, double frac_in)
  : frac_inside_(frac_in), box_(box) {}

  double frac_inside_;
  vsph_sph_box_2d box_;
};

class vsph_sph_cover_2d
{
 public:
  //: default constructor
  vsph_sph_cover_2d():min_area_fraction_(0.0) {}
  //: constructor from a parent box and the set of region rays
  vsph_sph_cover_2d(vsph_sph_box_2d const& region_bb,
                    std::vector<vsph_sph_point_2d> const& region_rays,
                    double area_per_ray,
                    double min_area_fraction = 0.9);

  //: accessors
  const vsph_sph_box_2d& cover_bb() const {return cover_bb_;}
  double min_area_fraction() const { return min_area_fraction_;}
  double actual_area_fraction() const { return actual_area_fraction_;}
  //: total area of the cover
  double area() const {return total_area_;}
  const std::vector<cover_el>& cover() const {return cover_;}

  //: set the members
  void set(double min_area_fraction, double total_area, double actual_area_fraction, std::vector<cover_el> const& cover);

  //: transform the cover on the unit sphere about center point (theta_c, phi_c), translation (\p t_theta, \p t_phi), and \p scale
  vsph_sph_cover_2d transform(double t_theta,
                              double t_phi, double scale,
                              double theta_c,double phi_c,
                              bool in_radians) const;

 private:
  //: compute area fraction inside the region
  double inside_area(vsph_sph_box_2d const& bb,
                     std::vector<vsph_sph_point_2d> const& region_rays,
                     double ray_area) const;
  //: the enclosing bounding box
  vsph_sph_box_2d cover_bb_;
  //: the minimum overall fractional area allowed
  double min_area_fraction_;
  //: the total area of the cover on the sphere
  double total_area_;
  //: the overall fractional area achieved. can be less than min_area_fraction_
  double actual_area_fraction_;
  //: the cover_2d as a set of boxes with associated fractional area
  std::vector<cover_el> cover_;
};

//: Return a cover that represents the intersection of two covers (could be empty.) Returns false if no intersection.
bool intersection(vsph_sph_cover_2d const& c1, vsph_sph_cover_2d const& c2,
                  vsph_sph_cover_2d& cover_inter);

//: Return the area of the intersection of two covers
double intersection_area(vsph_sph_cover_2d const& c1, vsph_sph_cover_2d const& c2);

#endif // vsph_sph_cover_2d_h_
