#include "betr_edgel_factory.h"
#include <sdet/sdet_detector.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel.h>
#include <brip/brip_vil_float_ops.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <bsol/bsol_algs.h>
#include <vsol/vsol_polygon_2d.h>
void betr_edgel_factory::set_parameters(float sigma, float noise_multiplier, double gradient_range, unsigned nbins){
  sdet_detector_params params;
  params.smooth = sigma;
  params.noise_multiplier = noise_multiplier;
  params.aggressive_junction_closure=1;
  params.borderp = false;
  params_ = params;
  gradient_range_ = gradient_range;
  nbins_ = nbins;
}
bool betr_edgel_factory::add_image(std::string const& iname, vil_image_resource_sptr const& imgr){
  if (!imgr||!imgr->ni()||!imgr->nj())
  {
    std::cout << "empty image resource when adding image\n";
    return false;
  }
  images_[iname]=imgr;
  brip_roi_sptr broi = new brip_roi(imgr->ni(), imgr->nj());
  rois_[iname] = broi;
  return true;
}
bool betr_edgel_factory::add_region(std::string const& iname, std::string const& region_name,
                                    vsol_box_2d_sptr const& box){
  brip_roi_sptr broi = rois_[iname];
  if(!broi){
    std::cout << "null roi for image " << iname << '\n';
    return false;
  }
  unsigned n = broi->n_regions();
  broi->add_region(box);
  regions_[iname][region_name] = n;
  return true;
}
bool betr_edgel_factory::add_region(std::string const& iname, std::string const& region_name, vsol_polygon_2d_sptr const& poly){
  brip_roi_sptr broi = rois_[iname];
  if(!broi){
    std::cout << "null roi for image " << iname << '\n';
    return false;
  }
  unsigned n = broi->n_regions();
  vsol_box_2d_sptr box = poly->get_bounding_box();
  bool good = this->add_region(iname, region_name, box);
  if(good)
    polys_[iname][n] = poly;
  return good;
}
bool betr_edgel_factory::add_region_from_origin_and_size(std::string const& iname, std::string const& region_name,
                                                         unsigned i0, unsigned j0, unsigned ni, unsigned nj){
  brip_roi_sptr broi = rois_[iname];
  if(!broi){
    std::cout << "null roi for image " << iname << '\n';
    return false;
  }
  unsigned n = broi->n_regions();
  broi->add_region(i0, j0, ni, nj);
  regions_[iname][region_name] = n;
  return true;
}
bool betr_edgel_factory::process(std::string iname, std::string region_name){
  vil_image_resource_sptr imgr = images_[iname];
  if(!imgr){
    std::cout << "image " << iname << " not found in map \n";
    return false;
  }
  brip_roi_sptr roi = rois_[iname];
  if(!roi){
    std::cout << "roi for " << iname << " not found in map \n";
    return false;
  }
  unsigned region_id = regions_[iname][region_name];
  unsigned ni = roi->csize(region_id);
  unsigned nj = roi->rsize(region_id);
  if(ni == 0 || nj ==0 ){
    std::cout << "roi " << region_id <<  " for " << iname << " is empty \n";
    return false;
  }
  int imin = roi->cmin(region_id);
  int jmin = roi->rmin(region_id);
  brip_roi_sptr temp_roi = new brip_roi();
  temp_roi->add_region(imin, jmin, ni, nj);
  vil_image_resource_sptr clip_resc;
  if(!brip_vil_float_ops::chip(imgr, temp_roi, clip_resc)){
    std::cout << " clip function failed for (" << imin << ' ' << jmin << ' ' << ni << ' ' << nj << ") on " << iname << '\n';
    return false;
  }
  if(clip_resc->pixel_format()==VIL_PIXEL_FORMAT_FLOAT){
    vil_image_view<vxl_byte> view = brip_vil_float_ops::convert_to_byte(clip_resc);
    clip_resc = vil_new_image_resource_of_view(view);
  }
  sdet_detector det(params_);
    
  det.SetImage(clip_resc);

  if(!det.DoContour()){
    std::cout << "Edgel detection failed\n";
    return false;
  }
  std::vector<vdgl_digital_curve_sptr> vd_edges;
  if (!det.get_vdgl_edges(vd_edges)){
    std::cout << "Detection worked but returned no edgels\n";
    return false;
  }
  edgels_[iname][region_name] = vd_edges;

  unsigned region_index = regions_[iname][region_name];
  vsol_polygon_2d_sptr poly = polys_[iname][region_index];
    std::vector<double> gmags;
  if(!poly){
    std::cout << "warning - can't find region poly - using roi instead\n";
    this->grad_mags(iname, region_name, gmags);
  }else
    this->grad_mags(iname, region_name, poly, gmags);
  bsta_histogram<double> h(gradient_range_, nbins_);
  for( std::vector<double>::iterator git = gmags.begin();
       git != gmags.end();++git)
    h.upcount(*git, 1.0);
  grad_hists_[iname][region_name] = h;
  return true;
}
bool betr_edgel_factory::grad_mags(std::string iname, std::string region_name, std::vector<double>& mags){
  std::vector< vdgl_digital_curve_sptr >  vd_edges = edgels_[iname][region_name];
  if(!vd_edges.size()){
    std::cout << "no edgels for the specified region " << iname << ':' << region_name << '\n';
    return false;
  }
  for (std::vector<vdgl_digital_curve_sptr>::iterator eit = vd_edges.begin();
       eit != vd_edges.end(); ++eit)
  {
      //get the edgel chain
    vdgl_interpolator_sptr itrp = (*eit)->get_interpolator();
    vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();
    unsigned int n = ech->size();
    for (unsigned int i=0; i<n;i++)
    {
      vdgl_edgel ed = (*ech)[i];
      double grad_mag = ed.get_grad();
      mags.push_back(grad_mag);
    }
  }
  return true;
}
bool betr_edgel_factory::grad_mags(std::string iname, std::string region_name, vsol_polygon_2d_sptr const& poly,
                                   std::vector<double>& mags){
  vgl_polygon<double>  vpoly = bsol_algs::vgl_from_poly(poly);
  brip_roi_sptr broi = rois_[iname];
  if(!broi){
    std::cout << "no roi for " << iname << '\n';
    return false;
  }
  unsigned region_index = regions_[iname][region_name];
  std::vector< vdgl_digital_curve_sptr >  vd_edges = edgels_[iname][region_name];
  if(!vd_edges.size()){
    std::cout << "no edgels for the specified region " << iname << ':' << region_name << '\n';
    return false;
  }
  double x0 = static_cast<double>(broi->cmin(region_index));
  double y0 = static_cast<double>(broi->rmin(region_index));
  for (std::vector<vdgl_digital_curve_sptr>::iterator eit = vd_edges.begin();
       eit != vd_edges.end(); ++eit)
  {
      //get the edgel chain
    vdgl_interpolator_sptr itrp = (*eit)->get_interpolator();
    vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();
    unsigned int n = ech->size();
    for (unsigned int i=0; i<n;i++)
    {
      vdgl_edgel ed = (*ech)[i];
      double x = ed.get_x()+x0, y = ed.get_y()+y0;
      if(!vpoly.contains(x, y))
        continue;
      double grad_mag = ed.get_grad();
      mags.push_back(grad_mag);
    }
  }
  return true;
}
