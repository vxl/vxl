#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vipl/vipl_threshold.txx>

template class vipl_threshold<vil_image, vil_image, float, float, vipl_trivial_pixeliter>;
#endif
