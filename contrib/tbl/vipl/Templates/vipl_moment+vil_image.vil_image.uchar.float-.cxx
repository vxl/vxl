#include <vil/vil_image.h>
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_moment.txx>
#include <vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG

template class vipl_moment<vil_image, vil_image, unsigned char, float VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
