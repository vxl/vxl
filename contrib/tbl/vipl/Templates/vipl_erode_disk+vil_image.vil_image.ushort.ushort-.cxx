#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vipl/vipl_erode_disk.txx>

template class vipl_erode_disk<vil_image, vil_image, unsigned short, unsigned short, vipl_trivial_pixeliter>;
#endif
