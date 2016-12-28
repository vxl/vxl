// This is brl/bseg/sdet/sdet_region.h
#ifndef sdet_region_h_
#define sdet_region_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A region class originally for the watershed algorithm (updated 12/11/2016)
//
// \verbatim
//  Modifications
//   Initial version July 11, 2004
//   Upgraded to support graph-based segmentation and neigboring regions 
//   JLM December 11, 2016
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <iosfwd>
#include <vcl_compiler.h>
#include "sdet_region_sptr.h"
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vdgl/vdgl_digital_region.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_oriented_box_2d.h>
#include <set>
#include <bsta/bsta_histogram.h>
class sdet_region : public vdgl_digital_region
{
 public:
  sdet_region();
  //:constructor for vdgl_digital_region
  sdet_region(int npts, const float* xp, const float* yp,
              const unsigned short *pix);
  sdet_region(vdgl_digital_region const& reg);
  ~sdet_region() {}
  //:accessors
  void set_label(const unsigned int label){region_label_ = label;}
  unsigned int label() const {return region_label_;}
  void add_neighbor(const unsigned int nbr){nbrs_.insert(nbr);}
  void remove_neighbor(const unsigned int nbr){nbrs_.erase(nbr);}
  void increment_neighbors(unsigned delta);
  const std::set<unsigned>& nbrs() const{return nbrs_;}
  //: boundary as convex hull of region
  void set_boundary(vsol_polygon_2d_sptr const& boundary){boundary_ = boundary;}
  vsol_polygon_2d_sptr boundary();

  vgl_box_2d<float> bbox();
  vgl_oriented_box_2d<float> obox();


    //: old style cast
  vdgl_digital_region* cast_to_digital_region(){return (vdgl_digital_region*) this;}
  
  //:utilities
  bool compute_boundary();
  bool boundary_valid() const{ return boundary_valid_;}
  bool compute_obox();
  bool obox_valid() const{ return obox_valid_;}
  bool compute_bbox();
  bool bbox_valid() const{ return bbox_valid_;}
  // intersection over union of bb with respect to this->bbox_
  float int_over_union(vgl_box_2d<float> bb);
  //: must implement comparison since containers need this
  struct region_less
  {
    bool operator()(sdet_region_sptr const & l,
                    sdet_region_sptr const & r) const
    {
      if (!l||!r)
        return false;
      return l->Npix() < r->Npix();//Large areas are favored
    }
  };
  
 protected:
  unsigned int region_label_;
  bool bbox_valid_;
  bool obox_valid_;
  bool boundary_valid_;
  vsol_polygon_2d_sptr boundary_;
  vgl_box_2d<float> bbox_;//!< axis aligned box
  vgl_oriented_box_2d<float> obox_;//!< oriented box based on the pixel scatter matrix
  std::set<unsigned> nbrs_;
  bsta_histogram<float> hist_;
};
#include "sdet_region_sptr.h"
//: merge digital regions and neighbor sets. The combined region is assigned the label of r1
sdet_region_sptr merge(sdet_region_sptr const& r1,sdet_region_sptr const& r2, unsigned merged_label);
#endif // sdet_region_h_
