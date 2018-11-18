#ifndef betr_edgel_factory_h_
#define betr_edgel_factory_h_
//:
// \file
// \brief betr_edgel_factory provides housekeeping for processing edgel segmentations
// \author J.L. Mundy
// \date June 25, 2016
//
//
#include <string>
#include <vector>
#include <map>
#include <vil/vil_image_resource_sptr.h>
#include <brip/brip_roi_sptr.h>
#include <betr_edgel_factory_params.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <bsta/bsta_histogram.h>
class betr_edgel_factory{
 public:
  betr_edgel_factory()= default;
  //: set parameters (same for all images and rois)
  void set_params(betr_edgel_factory_params const& params){
    params_ = params;
  }
  //: modify parameters
  betr_edgel_factory_params& params(){return params_;}

  // add images and rois
  bool add_image(std::string const& iname, vil_image_resource_sptr const& imgr);
  bool add_region(std::string const& iname, std::string const& region_name, vsol_box_2d_sptr const& box);
  bool add_region(std::string const& iname, std::string const& region_name, vsol_polygon_2d_sptr const& poly);
  bool add_region_from_origin_and_size(std::string const& iname, std::string const& region_name,
                                       unsigned i0, unsigned j0, unsigned ni, unsigned nj);
  bool add_region_from_bounds(std::string const& iname, std::string const& region_name,
                              unsigned imin, unsigned jmin, unsigned imax, unsigned jmax);
  //: remove all images and regions
  void clear();
  //: remove all regions on a given image roi
  void clear(std::string iname);
  //: remove a region on a given image roi
  void clear(std::string iname, std::string region_name);
  //: process edgels on all images and rois
  bool process();
  //: process edgels on all rois for a given image
  bool process(std::string iname);
  //: process edgels on a roi for a given image
  bool process(const std::string& iname, const std::string& region_name);
  //: gradient magnitudes for a region
  bool grad_mags(const std::string& iname, const std::string& region_name, std::vector<double>& mags);
  bool grad_mags(const std::string& iname, const std::string& region_name, vsol_polygon_2d_sptr const& poly,
                 std::vector<double>& mags);
  const bsta_histogram<double>& hist(std::string iname, std::string region_name){return grad_hists_[iname][region_name];}
  bool save_edgels(std::string const& dir) const;
  bool save_edgels_in_poly(std::string const& identifier, std::string const& dir);
  vil_image_resource_sptr edgel_image(const std::string& iname, const std::string& region_name, unsigned& i_offset, unsigned& j_offset);

 private:
  std::map<std::string, vil_image_resource_sptr> images_;
  std::map<std::string, brip_roi_sptr> rois_;
  std::map<std::string, std::map<std::string, unsigned> > regions_;
  std::map<std::string, std::map<unsigned, vsol_polygon_2d_sptr> > polys_;
  std::map<std::string, std::map<std::string, std::vector< vdgl_digital_curve_sptr > > > edgels_;
  std::map<std::string, std::map<std::string, bsta_histogram<double> > > grad_hists_;
  betr_edgel_factory_params params_;
};
#endif //guard
