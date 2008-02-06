#include <brdb/brdb_value.txx>

#include <vpgl/vpgl_camera.h>
#include <vpgl/io/vpgl_io_camera.h>

typedef vpgl_camera<double>* vpgl_camera_double_ptr;

BRDB_VALUE_INSTANTIATE(vpgl_camera_double_ptr, "vpgl_camera_double_ptr");
