// for vipl_histogram<...> instantiations:
#include <../Image/ImageProcessingBasics/section.h>
#include "../accessors/vipl_accessors_section.h"
#include <vipl/vipl_histogram.txx>
#include <vcl/vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG

template class vipl_histogram<section<unsigned char,2>,section<int,2>,unsigned char,int VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
