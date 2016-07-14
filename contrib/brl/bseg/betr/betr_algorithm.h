#ifndef betr_algorithm_h_
#define betr_algorithm_h_
//:
// \file
// \brief The base class for betr algorithms
// \author J.L. Mundy
// \date July 4, 2016
//
#include <string>
#include <vcl_compiler.h>
#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
class betr_algorithm : public vbl_ref_count
{
 public:
 betr_algorithm():name_("no_name"), offset_(0.0), alpha_(1.0), verbose_(false){}
 betr_algorithm(std::string name): name_(name),offset_(0.0), alpha_(1.0), verbose_(false){}
 betr_algorithm(std::string name, double offset, double alpha):name_(name), offset_(offset), alpha_(alpha), verbose_(false){}
  // performance parameters
  void set_offset(double offset){offset_ = offset;}
  void set_alpha(double alpha){alpha_ = alpha;}
  //: data inputs
  void set_reference_image(vil_image_resource_sptr const& ref_imgr){ref_imgr_ = ref_imgr;}
  void set_event_image(vil_image_resource_sptr const& evt_imgr){evt_imgr_ = evt_imgr;}
  void set_proj_ref_ref_object(vsol_polygon_2d_sptr const& ref_poly){ref_ref_poly_ = ref_poly;}
  void set_proj_ref_evt_object(vsol_polygon_2d_sptr const& evt_poly){ref_evt_poly_ = evt_poly;}
  void set_proj_evt_ref_object(vsol_polygon_2d_sptr const& ref_poly){evt_ref_poly_ = ref_poly;}
  void set_proj_evt_evt_object(vsol_polygon_2d_sptr const& evt_poly){evt_evt_poly_ = evt_poly;}
  //: accessors
  std::string name() const {return name_;}
  //: procedural  methods
  virtual bool process(){return false;}
  virtual double prob_change() const {return 0.0;}
  //: debug 
  //======================
  void set_verbose(bool verbose){verbose_ = verbose;}
  virtual void clear(){
    ref_imgr_ = VXL_NULLPTR;
    evt_imgr_ = VXL_NULLPTR;
    ref_ref_poly_ = VXL_NULLPTR;
    ref_evt_poly_ = VXL_NULLPTR;
	evt_ref_poly_ = VXL_NULLPTR;
    evt_evt_poly_ = VXL_NULLPTR;
  }
  protected:
  std::string name_;//algorithm name
  vil_image_resource_sptr ref_imgr_;
  vil_image_resource_sptr evt_imgr_;
  vsol_polygon_2d_sptr ref_ref_poly_;
  vsol_polygon_2d_sptr ref_evt_poly_;
  vsol_polygon_2d_sptr evt_ref_poly_;
  vsol_polygon_2d_sptr evt_evt_poly_;
  //as in p_change = 1/(1+e^-alpha*(change-offset))
  double offset_;
  double alpha_;
  bool verbose_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_algorithm.
#include "betr_algorithm_sptr.h"
