// for vipl_sobel<...> instantiations:
#include "../accessors/vipl_accessors_vbl_array_2d.h"
#include <vipl/vipl_sobel.txx>
#include <vcl/vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG
typedef vbl_array_2d<unsigned char> img_type;

template class vipl_sobel<img_type,img_type,unsigned char,unsigned char VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
