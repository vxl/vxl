#include <vnl/vnl_matrix.h>
typedef unsigned char ubyte;

#define FILTER_IMPTR_DEC_REFCOUNT(v) ((v)=0)
#define FILTER_IMPTR_INC_REFCOUNT(v) 
#define FILTER_KERNPTR_DEC_REFCOUNT(v) 

// this must be here for filter-2d to work
#include <vipl/section/vipl_section_container.h>
#include <vipl/section/vipl_section_iterator.h>
vipl_section_container<ubyte>*
vipl_filterable_section_container_generator(const vnl_matrix<ubyte>& im, ubyte*)
{
  vipl_section_container<ubyte> *rtn = new vipl_section_container<ubyte>((vipl_section_container<ubyte>*)0);
  rtn->put_imgptr((void*) &im);
  rtn->ref_imgsz()[0] = im.columns();
  rtn->ref_imgsz()[1] = im.rows();
  
  rtn->ref_secsz()[0] = im.columns();
  rtn->ref_secsz()[1] = im.rows();
 
  return rtn;
}

#include <vcl_compiler.h> // for the definition of VCL_DFL_TMPL_ARG

#include <vipl/filter/vipl_filter.h>
#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<vnl_matrix<ubyte>, vnl_matrix<ubyte>, ubyte, ubyte, 2 VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;

#include <vipl/filter/vipl_filter_2d.h>
#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<vnl_matrix<ubyte>, vnl_matrix<ubyte>, ubyte, ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>;

