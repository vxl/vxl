// This is mul/vil2/vil2_fwd.h
#ifndef vil2_fwd_h_
#define vil2_fwd_h_

class vil2_file_format;
class vil2_stream;
class vil2_image_resource;
class vil2_image_view_base;
class vil2_memory_image;
class vil2_memory_chunk;
template <class T> class vil2_image_view;
template <class T> struct vil2_rgb;
template <class T> struct vil2_rgba;
template <class T> class vil2_pixel_traits;
template <class T> class vil2_smart_ptr;
typedef vil2_smart_ptr<vil2_image_resource> vil2_image_resource_sptr;
typedef vil2_smart_ptr<vil2_image_view_base> vil2_image_view_base_sptr;
typedef vil2_smart_ptr<vil2_memory_chunk> vil2_memory_chunk_sptr;

#endif // vil2_fwd_h_
