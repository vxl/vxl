#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vipl/vipl_histogram.txx>

template class vipl_histogram<vil_image, vil_image, double, double, vipl_trivial_pixeliter>;
#endif
