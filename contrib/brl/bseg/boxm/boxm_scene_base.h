#ifndef boxm_scene_base_h_
#define boxm_scene_base_h_
//:
// \file
// \brief  The main class to keep the 3D world data and pieces
//
// \author Gamze Tunali
// \date Apr 14, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_smart_ptr.h>

#include "boxm_sample.h"

class boxm_scene_base : public vbl_ref_count
{
public:
  boxm_scene_base() : apperance_model_(boxm_apm_type::BOXM_APM_UNKNOWN) {}
  virtual ~boxm_scene_base(){}
  boxm_apm_type appearence_model() { return apperance_model_; }
  void set_appearence_model(boxm_apm_type model) { apperance_model_ = model; }

protected:
  boxm_apm_type apperance_model_;
};

typedef vbl_smart_ptr<boxm_scene_base> boxm_scene_base_sptr;


#endif