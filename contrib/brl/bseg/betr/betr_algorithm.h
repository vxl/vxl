#ifndef betr_algorithm_h_
#define betr_algorithm_h_
//:
// \file
// \brief The base class for betr algorithms
// \author J.L. Mundy
// \date July 4, 2016
//
#include <string>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include "betr_params.h"
class betr_algorithm : public vbl_ref_count
{
 public:
  betr_algorithm()
    : name_("no_name")
    , identifier_("null")
    , offset_(0.0)
    , alpha_(1.0)
    , verbose_(false)
    , params_(nullptr){}
 betr_algorithm(std::string  name)
    : name_(std::move(name))
    , offset_(0.0)
    , alpha_(1.0)
    , verbose_(false)
    , params_(nullptr)
   {}
 betr_algorithm(std::string  name, double offset, double alpha)
    : name_(std::move(name))
    , offset_(offset)
    , alpha_(alpha)
    , verbose_(false)
    , multiple_ref_(false)
    , params_(nullptr)
   {}
 betr_algorithm(std::string  name, betr_params_sptr const& params, double offset, double alpha)
    : name_(std::move(name))
    , offset_(offset)
    , alpha_(alpha)
    , verbose_(false)
    , multiple_ref_(false)
    , params_(params)
   {}

  //: sigmoid performance parameters - may be specialized for each algorithm
  void set_offset(double offset){offset_ = offset;}
  void set_alpha(double alpha){alpha_ = alpha;}

  //: algorithm parameters, e.g. edge detection threshold
  void set_params(betr_params_sptr const& params){params_ = params;}

  //: data inputs
  //: reference image(s)
  void set_reference_image(vil_image_resource_sptr const& ref_imgr){ref_rescs_.clear(); ref_rescs_.push_back(ref_imgr);}
  void set_reference_images(std::vector<vil_image_resource_sptr> const& ref_rescs){ref_rescs_ = ref_rescs;}
  //: event image - an algorithm processes one event region per event image at a time
  void set_event_image(vil_image_resource_sptr const& evt_imgr){evt_imgr_ = evt_imgr;}

  //: projected 3-d region(s) using the camera model for an image
  // the projected reference regions (one per reference image)
  void set_proj_ref_ref_object(vsol_polygon_2d_sptr const& ref_poly)
  {ref_ref_polys_.clear();ref_ref_polys_.push_back(ref_poly);}
  void set_proj_ref_ref_objects(std::vector<vsol_polygon_2d_sptr> const& ref_polys){ref_ref_polys_= ref_polys;}
  //: the event regions (one per reference image)
  void set_proj_ref_evt_object(vsol_polygon_2d_sptr const& evt_poly)
  {ref_evt_polys_.clear();ref_evt_polys_.push_back(evt_poly);}
  void set_proj_ref_evt_objects(std::vector<vsol_polygon_2d_sptr> const& evt_polys){ref_evt_polys_= evt_polys;}
  //: regions in the event image
  // reference region
  void set_proj_evt_ref_object(vsol_polygon_2d_sptr const& ref_poly){evt_ref_poly_ = ref_poly;}

  // event region
  void set_proj_evt_evt_object(vsol_polygon_2d_sptr const& evt_poly){evt_evt_poly_ = evt_poly;}

  //: accessors
  std::string name() const {return name_;}
  betr_params_sptr params(){return params_;}
  bool requires_multiple_ref_images() const { return multiple_ref_; }

  //: procedural  methods
  virtual bool process(){return false;}
  //: only a probability if calibrated using sigmoid parameters or other means
  virtual double prob_change() const {return 0.0;}
  //: An image of pixel change values (0:255) - used for change display purposes
  //  offset is with respect to the event image coordinate system
  virtual vil_image_resource_sptr change_image(unsigned& i_offset, unsigned& j_offset) const {return nullptr;}
  //: clear the algorithm state to process a new event region and/or new reference image(s)
  // subclasses may have additonal clear operations
  virtual void clear(){
    ref_rescs_.clear();
    evt_imgr_ = nullptr;
    ref_ref_polys_.clear();
    ref_evt_polys_.clear();
    evt_ref_poly_ = nullptr;
    evt_evt_poly_ = nullptr;
  }
    //: debug
  //======================
  void set_verbose(bool verbose){verbose_ = verbose;}
  //: an identifier for a particular execution run
  void set_identifier(std::string identifier){identifier_ = identifier;}

 protected:
  //:algorithm name
  std::string name_;

  //: unique id for identifying individual algorithm executions
  // For example,an algorithm executes once per event region in the case of
  // multiple event regions
  std::string identifier_;

  //:reference image resources
  std::vector<vil_image_resource_sptr> ref_rescs_;

  //: the event image resource
  vil_image_resource_sptr evt_imgr_;

  //: one 3-d reference region projected into multiple reference images
  //  same index order as ref_rescs
  std::vector< vsol_polygon_2d_sptr> ref_ref_polys_;

  //: projected event region in muliple reference images, one per reference image
  // same index order as ref_rescs
  std::vector< vsol_polygon_2d_sptr> ref_evt_polys_;

  //: the projected reference region in the event image
  vsol_polygon_2d_sptr evt_ref_poly_;

  //: the projected event region in the event image
  vsol_polygon_2d_sptr evt_evt_poly_;

  //:sigmoid parameters as in p_change = 1/(1+e^-alpha*(change-offset))
  double offset_;
  double alpha_;
  bool verbose_;
  bool multiple_ref_;//does the algorithm require multiple reference images
  betr_params_sptr params_;
};
#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS betr_algorithm.
#include "betr_algorithm_sptr.h"
