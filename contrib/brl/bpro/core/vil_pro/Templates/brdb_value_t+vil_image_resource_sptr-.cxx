#include <brdb/brdb_value.hxx>

#include <vil/vil_image_resource.h>
#include <vil/io/vil_io_smart_ptr.h>

typedef vil_image_resource_sptr img_res_sptr;

BRDB_VALUE_INSTANTIATE(img_res_sptr, "vil_image_resource_sptr");
