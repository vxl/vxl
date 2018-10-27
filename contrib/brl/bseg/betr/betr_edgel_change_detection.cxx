#include <cmath>
#include "betr_edgel_change_detection.h"
#include "betr_edgel_factory.h"
#include "betr_event_trigger.h"
#include <bsta/bsta_histogram.h>
bool betr_edgel_change_detection::process(){
  betr_edgel_factory ef;
  auto* params = dynamic_cast<betr_edgel_change_detection_params*>(params_.ptr());
  auto* efparams_ptr = dynamic_cast<betr_edgel_factory_params*>(params->edgel_factory_params_.ptr());
  efparams_ptr->det_params_.smooth = params->sigma_;
  efparams_ptr->det_params_.noise_multiplier = params->noise_mul_;
  ef.set_params(*efparams_ptr);
  if(ref_rescs_.size()!=1){
    std::cout << "not exactly one reference image in edgel_change_detection\n";
    return false;
  }
  ef.add_image("ref_image", ref_rescs_[0]);
  ef.add_image("evt_image", evt_imgr_);
  if(ref_ref_polys_.size()!=1){
    std::cout << "not exactly one reference ref_poly in edgel_change_detection\n";
    return false;
  }
  ef.add_region("ref_image", "ref_ref_poly", ref_ref_polys_[0]);
  ef.add_region("evt_image", "evt_ref_poly", evt_ref_poly_);
  if(ref_evt_polys_.size()!=1){
    std::cout << "not exactly one reference evt_poly in edgel_change_detection\n";
    return false;
  }
  ef.add_region("ref_image", "ref_evt_poly", ref_evt_polys_[0]);
  ef.add_region("evt_image", "evt_evt_poly", evt_evt_poly_);
  bool good = ef.process("ref_image","ref_ref_poly");
  good = good && ef.process("ref_image","ref_evt_poly");
  good = good && ef.process("evt_image","evt_ref_poly");
  good = good && ef.process("evt_image","evt_evt_poly");
  if(!good){
    std::cout << "warning edgel factory failed" << std::endl;
    js_div_ = -1.0;
    return true;
  }
    //debug
 /* if(verbose_){
    std::string dir =  "D:/tests/rajaei_test/trigger/";
    ef.save_edgels_in_poly(identifier_, dir);
  }*/
  // end debug
  const bsta_histogram<double>& h_ref_ref = ef.hist("ref_image","ref_ref_poly");
  const bsta_histogram<double>& h_ref_evt = ef.hist("ref_image","ref_evt_poly");
  const bsta_histogram<double>& h_evt_ref = ef.hist("evt_image","evt_ref_poly");
  const bsta_histogram<double>& h_evt_evt = ef.hist("evt_image","evt_evt_poly");
  change_img_ = ef.edgel_image("evt_image","evt_evt_poly", i_offset_, j_offset_);
  if(verbose_){
    std::cout << "h_ref_ref\n";
    h_ref_ref.print();
    std::cout << "\nh_ref_evt\n";
    h_ref_evt.print();
    std::cout << "h_evt_ref\n";
    h_evt_ref.print();
    std::cout << "\nh_evt_evt\n";
    h_evt_evt.print();
  }
  double fraction = 0.1;//maybe make a parameter
  double vabove_ref_ref = h_ref_ref.value_with_area_above(fraction);
  double vabove_evt_ref = h_evt_ref.value_with_area_above(fraction);
  bsta_histogram<double> scaled_evt_evt = h_evt_evt, scaled_ref_evt = h_ref_evt;
  if(vabove_ref_ref == vabove_evt_ref){// no scaling needed
          js_div_ = js_divergence(scaled_ref_evt, scaled_evt_evt);
          return true;
  }else if(vabove_ref_ref>vabove_evt_ref){
   double s = minimum_js_divergence_scale(h_ref_ref, h_evt_ref);
    scaled_ref_evt = scale(h_ref_evt, s);
  }else{
    double s = minimum_js_divergence_scale(h_evt_ref, h_ref_ref);
    scaled_evt_evt = scale(h_evt_evt, s);
  }
  js_div_ = js_divergence(scaled_ref_evt, scaled_evt_evt);
  if(verbose_){
    std::cout << "\n scaled h_ref_evt\n";
    scaled_ref_evt.print();
    std::cout << "\nscaled h_evt_evt\n";
    scaled_evt_evt.print();
  }
  return true;

}
