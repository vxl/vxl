// for vipl_threshold<...> instantiation:
#include "../accessors/vipl_accessors_vnl_matrix.h"
#include <vipl/vipl_threshold.txx>
#include <vcl/vcl_compiler.h> // for definition of VCL_DFL_TMPL_ARG
typedef vnl_matrix<unsigned char> img_type;

template class vipl_threshold<img_type,img_type,unsigned char,unsigned char VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;
