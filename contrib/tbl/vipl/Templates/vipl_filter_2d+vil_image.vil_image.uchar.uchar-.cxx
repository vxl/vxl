#include <vil/vil_image.h>

// this must be here for filter-2d to work
#include <vipl/section/vipl_filterable_section_container_generator_vil_image.txx>

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<vil_image, vil_image, unsigned char, unsigned char, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<vil_image, vil_image, unsigned char, unsigned char, vipl_trivial_pixeliter>;

#include <vbl/vbl_smart_ptr.h>
template class vbl_smart_ptr<vil_image>;

//Wierd kludge
#if defined VCL_WIN32 && defined NDEBUG
void vbl_smart_ptr<vil_image>::ref(vil_image*) {}
void vbl_smart_ptr<vil_image>::unref(vil_image*) {}
#endif
