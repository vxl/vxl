// for vipl_median<...> instantiations:
#include "../accessors/vipl_accessors_vnl_matrix.h"
#include <vipl/vipl_median.txx>
#include <vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG
typedef vnl_matrix<unsigned char> img_type;

template class vipl_median<img_type,img_type,unsigned char,unsigned char VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
