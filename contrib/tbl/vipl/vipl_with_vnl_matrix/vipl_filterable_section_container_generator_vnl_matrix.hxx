#ifndef vipl_filterable_section_container_generator_vnl_matrix_hxx_
#define vipl_filterable_section_container_generator_vnl_matrix_hxx_

#include <vnl/vnl_matrix.h>

#undef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#undef FILTER_IMPTR_INC_REFCOUNT
#define FILTER_IMPTR_INC_REFCOUNT(v)
#undef FILTER_KERNPTR_DEC_REFCOUNT
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

#include <vipl/section/vipl_section_container.hxx>
#include <vipl/section/vipl_section_iterator.hxx>
template <class DataType>
vipl_section_container<DataType>*
vipl_filterable_section_container_generator(const vnl_matrix<DataType>& im, DataType*)
{
  vipl_section_container<DataType> *rtn = new vipl_section_container<DataType>((vipl_section_container<DataType>*)nullptr);
  rtn->put_imgptr((const void*) &im);
  rtn->ref_imgsz()[0] = im.columns();
  rtn->ref_imgsz()[1] = im.rows();

  rtn->ref_secsz()[0] = im.columns();
  rtn->ref_secsz()[1] = im.rows();

  return rtn;
}

#endif // vipl_filterable_section_container_generator_vnl_matrix_hxx_
