#include <brdb/brdb_value.hxx>

#include <vil/vil_image_view_base.h>
#include <vil/io/vil_io_smart_ptr.h>
#include <vil/io/vil_io_image_view_base.h>

typedef vil_image_view_base_sptr img_sptr;

BRDB_VALUE_INSTANTIATE(img_sptr, "vil_image_view_base_sptr");
