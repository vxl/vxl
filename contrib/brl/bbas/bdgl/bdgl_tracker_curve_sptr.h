
#ifndef bdgl_tracker_curve_sptr_h_
#define bdgl_tracker_curve_sptr_h_

//#include<bdgl\bdgl_tracker_curve.h>

class bdgl_tracker_curve;
class match_data;

#include <vbl/vbl_smart_ptr.h>

typedef vbl_smart_ptr<bdgl_tracker_curve> bdgl_tracker_curve_sptr;
typedef vbl_smart_ptr<match_data> match_data_sptr;
#endif


