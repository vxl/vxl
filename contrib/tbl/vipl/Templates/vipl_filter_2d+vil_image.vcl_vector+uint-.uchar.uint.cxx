#include <vil/vil_image.h>
#include <vil/vil_byte.h>
#include <vcl_vector.h>

// this must be here for filter-2d to work
#include <vipl/section/vipl_filterable_section_container_generator_vil_image.txx>
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx>

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<vil_image, vcl_vector<unsigned>, vil_byte, unsigned, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<vil_image, vcl_vector<unsigned>, vil_byte, unsigned, vipl_trivial_pixeliter>;

#include <vbl/vbl_smart_ptr.h>
template class vbl_smart_ptr<vcl_vector<unsigned> >;
void vbl_smart_ptr<vcl_vector<unsigned> >::ref(vcl_vector<unsigned> *) {}
void vbl_smart_ptr<vcl_vector<unsigned> >::unref(vcl_vector<unsigned> *) {}
