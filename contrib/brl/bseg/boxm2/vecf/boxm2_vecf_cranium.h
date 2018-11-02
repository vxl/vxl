#ifndef boxm2_vecf_cranium_h_
#define boxm2_vecf_cranium_h_
//:
// \file
// \brief  The cranium portion of the skull
//
// \author J.L. Mundy
// \date   6 Nov 2015
//
#include <iostream>
#include <string>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_box_3d.h>
#include <bvgl/bvgl_grid_index_3d.h>
#include "boxm2_vecf_geometry_base.h"
#include "boxm2_vecf_cranium_params.h"
class boxm2_vecf_cranium : public boxm2_vecf_geometry_base{
 public:

 boxm2_vecf_cranium(): nbins_(25){
  }
  boxm2_vecf_cranium(std::string const& geometry_file, unsigned nbins = 25);

 boxm2_vecf_cranium(vgl_pointset_3d<double>  ptset,unsigned nbins = 25): ptset_(std::move(ptset)), nbins_(nbins){}

 void read_cranium(std::istream& istr);

 vgl_box_3d<double> bounding_box() const {return index_.bounding_box();}
 double distance(vgl_point_3d<double> const& p) const {return index_.distance(p);}
 //: the functor operator for surface distance. dist_thresh is the distance a closest point on the normal plane
 // can be away from the closest point in the cross-section pointset.
 double operator() (vgl_point_3d<double> const& p) const override{ return index_.distance(p);}
 bool inverse_vector_field(vgl_point_3d<double> const& p, vgl_vector_3d<double>& inv_vf) const override;

 //: accessors
 void set_params(boxm2_vecf_cranium_params const& params){params_ = params;}
 //:for debug purposes
 void display_vrml(std::ofstream& ostr) const;

 private:
 unsigned nbins_;
 bvgl_grid_index_3d<double> index_;
 vgl_pointset_3d<double> ptset_;
 boxm2_vecf_cranium_params params_;
};
std::ostream&  operator << (std::ostream& s, boxm2_vecf_cranium const& pr);
std::istream&  operator >> (std::istream& s, boxm2_vecf_cranium& pr);

#endif// boxm2_vecf_cranium
