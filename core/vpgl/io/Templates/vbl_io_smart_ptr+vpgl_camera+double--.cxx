#include <vpgl/vpgl_camera.h>
#include <vpgl/io/vpgl_io_camera.h>
#include <vbl/io/vbl_io_smart_ptr.hxx>

typedef vpgl_camera<double> vpgl_camera_double;
VBL_IO_SMART_PTR_INSTANTIATE(vpgl_camera_double);
