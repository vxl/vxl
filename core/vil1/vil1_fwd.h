//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_fwd_h_
#define vil_fwd_h_

// classes
class vil_file_format;
class vil_image_impl;
class vil_stream;
class vil_image;
class vil_memory_image;
template <class T> class vil_memory_image_of;

// typedefs
typedef unsigned char vil_byte;
typedef vil_memory_image_of<bool>     vil_bool_buffer;
typedef vil_memory_image_of<vil_byte> vil_byte_buffer;
typedef vil_memory_image_of<int>      vil_int_buffer;
typedef vil_memory_image_of<float>    vil_float_buffer;

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_fwd.
