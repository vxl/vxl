#include <vcl_compiler.h>
#include <vbl/vbl_smart_ptr.txx>
#include <rgrl/rgrl_view.h>

// ICC complains on incomplete types of the following classes
#ifdef VCL_ICC 
#include <rgrl/rgrl_estimator.h>
#include <rgrl/rgrl_transformation.h>
#endif

VBL_SMART_PTR_INSTANTIATE( rgrl_view );
