// for vipl_erode_disk<...> instantiation:
#include <../Image/ImageProcessingBasics/section.h>
#include "../accessors/vipl_accessors_section.h"
#include <vipl/vipl_erode_disk.txx>
#include <vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG
typedef section<unsigned char,2> img_type;

template class vipl_erode_disk<img_type,img_type,unsigned char,unsigned char VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
