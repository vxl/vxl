// for vipl_threshold<...> instantiation:
#include <../Image/ImageProcessingBasics/section.h>
#include "../accessors/vipl_accessors_section.h"
#include <vipl/vipl_threshold.txx>
#include <vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG
typedef section<unsigned char,2> img_type;

template class vipl_threshold<img_type,img_type,unsigned char,unsigned char VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
