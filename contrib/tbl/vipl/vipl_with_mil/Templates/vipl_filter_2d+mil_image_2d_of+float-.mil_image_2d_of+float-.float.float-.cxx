#include <mil/mil_image_2d_of.h>

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_mil_image.txx"

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<mil_image_2d_of<float>, mil_image_2d_of<float>, float, float, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<mil_image_2d_of<float>, mil_image_2d_of<float>, float, float, vipl_trivial_pixeliter>;

#include <vbl/vbl_smart_ptr.h>
template class vbl_smart_ptr<mil_image_2d_of<float> >;
void vbl_smart_ptr<mil_image_2d_of<float> >::ref(mil_image_2d_of<float> *) {}
void vbl_smart_ptr<mil_image_2d_of<float> >::unref(mil_image_2d_of<float> *) {}
