// This avoids automatic instantiation of other smart pointers:
#define rgrl_transformation_sptr_h_
class rgrl_transformation_sptr {};
#define rgrl_estimator_sptr_h_
class rgrl_estimator_sptr {};

#include <rgrl/rgrl_view.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE( rgrl_view );
