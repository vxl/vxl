#include <brdb/brdb_value.txx>

#include <vpgl/vpgl_camera.h>
#include <vpgl/io/vpgl_io_camera.h>
#include <vbl/io/vbl_io_smart_ptr.h>

typedef vbl_smart_ptr<vpgl_camera<double> > vpgl_camera_double_sptr;

BRDB_VALUE_INSTANTIATE(vpgl_camera_double_sptr, "vpgl_camera_double_sptr");
