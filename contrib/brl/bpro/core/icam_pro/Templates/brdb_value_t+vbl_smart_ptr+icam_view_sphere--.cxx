#include <brdb/brdb_value.txx>

#include <icam/icam_view_sphere.h>
#include <vbl/io/vbl_io_smart_ptr.h>

typedef vbl_smart_ptr<icam_view_sphere> icam_view_sphere_sptr;

BRDB_VALUE_INSTANTIATE(icam_view_sphere_sptr, "icam_view_sphere_sptr");
