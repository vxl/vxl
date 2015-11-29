#ifndef boxm2_vecf_cranium_h_
#define boxm2_vecf_cranium_h_
//:
// \file
// \brief  The cranium portion of the skull
//
// \author J.L. Mundy
// \date   6 Nov 2015
//
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_box_3d.h>
#include <bvgl/bvgl_grid_index_3d.h>
class boxm2_vecf_cranium{
 public:

 boxm2_vecf_cranium(): nbins_(25){
  }
  boxm2_vecf_cranium(vcl_string const& geometry_file, unsigned nbins = 25);

 boxm2_vecf_cranium(vgl_pointset_3d<double> const& ptset,unsigned nbins = 25): ptset_(ptset), nbins_(nbins){}

 void read_cranium(vcl_istream& istr);

 vgl_box_3d<double> bounding_box() const {return index_.bounding_box();}
 double surface_distance(vgl_point_3d<double> const& p) const {return index_.surface_distance(p);}
 //:for debug purposes
 void display_vrml(vcl_ofstream& ostr) const;

 private:
 unsigned nbins_;
 bvgl_grid_index_3d index_;
 vgl_pointset_3d<double> ptset_;
};
vcl_ostream&  operator << (vcl_ostream& s, boxm2_vecf_cranium const& pr);
vcl_istream&  operator >> (vcl_istream& s, boxm2_vecf_cranium& pr);

#endif// boxm2_vecf_cranium
