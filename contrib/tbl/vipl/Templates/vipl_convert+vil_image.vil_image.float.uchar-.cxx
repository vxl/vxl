#include <vil/vil_image.h>
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_convert.txx>
#include <vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG

template class vipl_convert<vil_image, vil_image, float, unsigned char VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
