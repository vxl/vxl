// for vipl_dilate_disk<...> instantiation:
#include <../Image/ImageProcessingBasics/section.h>
#include "../accessors/vipl_accessors_section.h"
#include <vipl/vipl_x_gradient.txx>
#include <vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG

#include <vil/vil_rgb.h>
typedef vil_rgb<unsigned char> rgbcell;
typedef section<rgbcell,2> img_type;

template class vipl_x_gradient<img_type,img_type,rgbcell,rgbcell VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
