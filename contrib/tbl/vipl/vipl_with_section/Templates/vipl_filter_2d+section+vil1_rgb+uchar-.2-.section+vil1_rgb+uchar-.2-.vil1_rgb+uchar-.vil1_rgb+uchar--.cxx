#include <../Image/ImageProcessingBasics/section.h>
typedef unsigned char ubyte;
#include <vil/vil_rgb.h>
typedef vil_rgb<ubyte> rgbcell;

#define FILTER_IMPTR_DEC_REFCOUNT(v) ((v)=0)
#define FILTER_IMPTR_INC_REFCOUNT(v) 
#define FILTER_KERNPTR_DEC_REFCOUNT(v) 

// this must be here for filter-2d to work
#include <vipl/section/vipl_section_container.h>
#include <vipl/section/vipl_section_iterator.h>
vipl_section_container<rgbcell>*
vipl_filterable_section_container_generator(const section<rgbcell,2>& im, rgbcell*)
{
  vipl_section_container<rgbcell> *rtn = new vipl_section_container<rgbcell>((vipl_section_container<rgbcell>*)0);
  rtn->put_imgptr((void*) &im);
  rtn->ref_imgsz()[0] = im.Size(0);
  rtn->ref_imgsz()[1] = im.Size(1);
  
  rtn->ref_secsz()[0] = im.Size(0);
  rtn->ref_secsz()[1] = im.Size(1);
 
  return rtn;
}

#include <vcl_compiler.h> // for the definition of VCL_DFL_TMPL_ARG

#include <vipl/filter/vipl_filter.h>
#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<section<rgbcell,2>, section<rgbcell,2>, rgbcell, rgbcell, 2 VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;

#include <vipl/filter/vipl_filter_2d.h>
#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<section<rgbcell,2>, section<rgbcell,2>, rgbcell, rgbcell VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;

