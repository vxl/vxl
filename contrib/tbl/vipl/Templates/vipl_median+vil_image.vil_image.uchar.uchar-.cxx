#include <vil/vil_image.h>
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_median.txx>
#include <vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG

template class vipl_median<vil_image, vil_image, unsigned char, unsigned char VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
