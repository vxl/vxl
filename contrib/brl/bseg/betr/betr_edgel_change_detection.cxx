#include "betr_edgel_change_detection.h"
#include "betr_edgel_factory.h"
#include "betr_event_trigger.h"
#include <bsta/bsta_histogram.h>
bool betr_edgel_change_detection::process(){
  betr_edgel_factory ef;
  ef.set_parameters(sigma_, noise_mul_);
  ef.add_image("ref_image", ref_imgr_);
  ef.add_image("evt_image", evt_imgr_);
  ef.add_region("ref_image", "ref_ref_poly", ref_ref_poly_);
  ef.add_region("evt_image", "evt_ref_poly", evt_ref_poly_);
  ef.add_region("ref_image", "ref_evt_poly", ref_evt_poly_);
  ef.add_region("evt_image", "evt_evt_poly", evt_evt_poly_);
  bool good = ef.process("ref_image","ref_ref_poly");
  good = good && ef.process("ref_image","ref_evt_poly");
  good = good && ef.process("evt_image","evt_ref_poly");
  good = good && ef.process("evt_image","evt_evt_poly"); 
  //debug
  std::string dir =  "D:/tests/rajaei_test/trigger/";
  ef.save_edgels_in_poly(dir);
  // end debug
  const bsta_histogram<double>& h_ref_ref = ef.hist("ref_image","ref_ref_poly");
  const bsta_histogram<double>& h_ref_evt = ef.hist("ref_image","ref_evt_poly");
  std::cout << "h_ref_ref\n";
  h_ref_ref.print();
  std::cout << "\nh_ref_evt\n";
  h_ref_evt.print();
  const bsta_histogram<double>& h_evt_ref = ef.hist("evt_image","evt_ref_poly");
  const bsta_histogram<double>& h_evt_evt = ef.hist("evt_image","evt_evt_poly");
  std::cout << "h_evt_ref\n";
  h_evt_ref.print();
  std::cout << "\nh_evt_evt\n";
  h_evt_evt.print();
  // note need to find ref with highest gradients and then swap, but later
  double s = minimum_js_divergence_scale(h_ref_ref, h_evt_ref);
  bsta_histogram<double> scaled_evt_evt = scale(h_evt_evt, s);
  js_div_ = js_divergence(h_ref_evt, scaled_evt_evt);
  return true;
  
}
