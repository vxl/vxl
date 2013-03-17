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
// The cover_2d is constrained so that a min fractional area of the cover
// lies inside the spherical region
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vsph/vsph_sph_box_2d.h>

class cover_el
{
 public:
  cover_el():frac_inside_(0.0){}
  cover_el(vsph_sph_box_2d const& box, double frac_in)
  : frac_inside_(frac_in), box_(box) {}

  double frac_inside_;
  vsph_sph_box_2d box_;
};

class vsph_sph_cover_2d
{
 public:
 vsph_sph_cover_2d():min_area_fraction_(0.0){};
  vsph_sph_cover_2d(vsph_sph_box_2d const& region_bb,
                    vcl_vector<vsph_sph_point_2d> const& region_rays,
                    double area_per_ray,
                    double min_area_fraction = 0.9);

  //: accessors
  double min_area_fraction() const { return min_area_fraction_;}
  double actual_area_fraction() const { return actual_area_fraction_;}
  double area() const {return total_area_;}
  const vcl_vector<cover_el>& cover() const{
    return cover_;}
  
  //: set the members
  void set(double min_area_fraction, double total_area, double actual_area_fraction, vcl_vector<cover_el> const& cover);

  //: total area of the cover
  //: transform the cover on the unit sphere about center point (theta_c, phi_c)
  // translation  (t_theta, t_phi), and scale
  vsph_sph_cover_2d transform(double t_theta,
			      double t_phi, double scale,
			      double theta_c,double phi_c,
			      bool in_radians) const;

 private:
  //: compute area fraction inside the region
  double inside_area(vsph_sph_box_2d const& bb,
                     vcl_vector<vsph_sph_point_2d> const& region_rays,
                     double ray_area) const;
  //: the minimum overall fractional area allowed
  double min_area_fraction_;
  //: the total area of the cover on the sphere
  double total_area_;
  //: the overall fractional area achieved. can be less than min_area_fraction_
  double actual_area_fraction_;
  //: the cover_2d as a set of boxes with associated fractional area
  vcl_vector<cover_el> cover_;
};
//: return a cover that represents the intersection of two covers (could be empty)
bool intersection(vsph_sph_cover_2d const& c1, vsph_sph_cover_2d const& c2,
                  vsph_sph_cover_2d& cover_inter);

//: return the area of the intersection of two covers
double intersection_area(vsph_sph_cover_2d const& c1, vsph_sph_cover_2d const& c2);

#endif // vsph_sph_cover_2d_h_
