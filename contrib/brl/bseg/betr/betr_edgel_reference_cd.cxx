#include <cmath>
#include "betr_edgel_reference_cd.h"
#include "betr_edgel_factory.h"
#include "betr_event_trigger.h"
#include <bsta/bsta_histogram.h>
bool betr_edgel_reference_cd::process(){
  betr_edgel_factory ef;
  betr_edgel_factory_params& efparams = ef.params();
  auto* params = dynamic_cast<betr_edgel_reference_cd_params*>(params_.ptr());
  efparams.det_params_.smooth = params->sigma_;
  efparams.det_params_.noise_multiplier = params->noise_mul_;
  ef.add_image("evt_image", evt_imgr_);
  ef.add_region("evt_image", "evt_ref_poly", evt_ref_poly_);
  ef.add_region("evt_image", "evt_evt_poly", evt_evt_poly_);
  bool good = ef.process("evt_image","evt_ref_poly");
  good = good && ef.process("evt_image","evt_evt_poly");
  if(!good){
    js_div_ = -1.0;
    return true;
  }
  //debug
  if(verbose_){
    std::string dir =  "D:/tests/rajaei_test/trigger/";
    ef.save_edgels_in_poly(identifier_, dir);
  }
  // end debug
  const bsta_histogram<double>& h_evt_ref = ef.hist("evt_image","evt_ref_poly");
  const bsta_histogram<double>& h_evt_evt = ef.hist("evt_image","evt_evt_poly");
  if(verbose_){
    std::cout << "h_evt_ref\n";
    h_evt_ref.print();
    std::cout << "\nh_evt_evt\n";
    h_evt_evt.print();
  }
  js_div_ = js_divergence(h_evt_ref, h_evt_evt);
  return true;
}
