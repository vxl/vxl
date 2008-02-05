#include <brdb/brdb_value.txx>

#include <vil/vil_image_view_base.h>
#include <vil/io/vil_io_smart_ptr.h>


typedef vil_image_view_base_sptr img_sptr;

BRDB_VALUE_INSTANTIATE(img_sptr, "vil_image_view_base_sptr");
