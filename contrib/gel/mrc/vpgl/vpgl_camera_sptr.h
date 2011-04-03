#ifndef vpgl_camera_sptr_h_
#define vpgl_camera_sptr_h_

template <class T> class vpgl_camera;
#include <vbl/vbl_smart_ptr.h>
typedef vbl_smart_ptr<vpgl_camera<double> > vpgl_camera_double_sptr;
typedef vbl_smart_ptr<vpgl_camera<float> > vpgl_camera_float_sptr;

#endif // vpgl_camera_sptr_h_
