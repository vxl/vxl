#include <vil/vil_image.h>

#define FILTER_IMPTR_DEC_REFCOUNT(v) ((v)=0)
#define FILTER_IMPTR_INC_REFCOUNT(v) 
#define FILTER_KERNPTR_DEC_REFCOUNT(v) 

// this must be here for filter-2d to work
#include <vipl/section/vipl_section_container.h>
#include <vipl/section/vipl_section_iterator.h>
vipl_section_container<short>*
vipl_filterable_section_container_generator(const vil_image& im, short*)
{
  vipl_section_container<short> *rtn = new vipl_section_container<short>((vipl_section_container<short>*)0);
  rtn->put_imgptr((void*) &im);
  rtn->ref_imgsz()[0] = im.width();
  rtn->ref_imgsz()[1] = im.height();
  
  rtn->ref_secsz()[0] = im.width();  // should actually be block_width for block-buffered images
  rtn->ref_secsz()[1] = im.height(); // should actually be block_height for block-buffered images
 
  return rtn;
}
vipl_section_container<unsigned char>*
vipl_filterable_section_container_generator(const vil_image& im, unsigned char*);

#include <vcl/vcl_compiler.h> // for the definition of VCL_DFL_TMPL_ARG

#include <vipl/filter/vipl_filter.h>
#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<vil_image, vil_image, short, unsigned char, 2 VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;

#include <vipl/filter/vipl_filter_2d.h>
#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<vil_image, vil_image, short, unsigned char VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;

