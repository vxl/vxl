// This is mul/vil3d/vil3d_fwd.h
#ifndef vil3d_fwd_h_
#define vil3d_fwd_h_

class vil3d_file_format;
class vil3d_image_resource;
class vil3d_image_view_base;
class vil3d_memory_image;
class vil_memory_chunk;
template <class T> class vil3d_image_view;
template <class T> class vil_pixel_traits;
template <class T> class vil_smart_ptr;
typedef vil_smart_ptr<vil3d_image_resource> vil3d_image_resource_sptr;
typedef vil_smart_ptr<vil3d_image_view_base> vil3d_image_view_base_sptr;
typedef vil_smart_ptr<vil_memory_chunk> vil_memory_chunk_sptr;

#endif // vil3d_fwd_h_
