#include <section/section.h>
typedef unsigned char ubyte;

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_section.txx"

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<section<ubyte,2>, section<ubyte,2>, ubyte, ubyte, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<section<ubyte,2>, section<ubyte,2>, ubyte, ubyte, vipl_trivial_pixeliter>;

#include <vbl/vbl_smart_ptr.h>
template class vbl_smart_ptr<section<ubyte,2> >;
void vbl_smart_ptr<section<ubyte,2> >::ref(section<ubyte,2> *) {}
void vbl_smart_ptr<section<ubyte,2> >::unref(section<ubyte,2> *) {}
