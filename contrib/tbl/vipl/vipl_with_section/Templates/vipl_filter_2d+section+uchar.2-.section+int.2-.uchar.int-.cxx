#include <../Image/ImageProcessingBasics/section.h>
typedef unsigned char ubyte;

#define FILTER_IMPTR_DEC_REFCOUNT(v) ((v)=0)
#define FILTER_IMPTR_INC_REFCOUNT(v) 
#define FILTER_KERNPTR_DEC_REFCOUNT(v) 

// this must be here for filter-2d to work
#include <vipl/section/vipl_section_container.h>
#include <vipl/section/vipl_section_iterator.h>

vipl_section_container<ubyte>*
vipl_filterable_section_container_generator(const section<ubyte,2>& im, ubyte*);

vipl_section_container<int>*
vipl_filterable_section_container_generator(const section<int,2>& im, int*)
{
  vipl_section_container<int> *rtn = new vipl_section_container<int>((vipl_section_container<int>*)0);
  rtn->put_imgptr((void*) &im);
  rtn->ref_imgsz()[0] = im.Size(0);
  rtn->ref_imgsz()[1] = im.Size(1);
  
  rtn->ref_secsz()[0] = im.Size(0);
  rtn->ref_secsz()[1] = im.Size(1);
 
  return rtn;
}

#include <vcl/vcl_compiler.h> // for the definition of VCL_DFL_TMPL_ARG

#include <vipl/filter/vipl_filter.h>
#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<section<ubyte,2>, section<int,2>, ubyte, int, 2 VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;

#include <vipl/filter/vipl_filter_2d.h>
#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<section<ubyte,2>, section<int,2>, ubyte, int VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;

