// This is core/vil/vil_fwd.h
#ifndef vil_fwd_h_
#define vil_fwd_h_

class vil_file_format;
class vil_stream;
class vil_image_resource;
class vil_image_list;
class vil_memory_image;
class vil_memory_chunk;
class vil_block_cache;
class vil_image_view_base;
template <class T> class vil_image_view;
template <class T> struct vil_rgb;
template <class T> struct vil_rgba;
template <class T> class vil_smart_ptr;
typedef vil_smart_ptr<vil_image_resource> vil_image_resource_sptr;
typedef vil_smart_ptr<vil_image_view_base> vil_image_view_base_sptr;
typedef vil_smart_ptr<vil_memory_chunk> vil_memory_chunk_sptr;

template <class imT> class vil_border;
template <class imT> class vil_border_accessor;

#endif // vil_fwd_h_
