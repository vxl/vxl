#ifndef vipl_filterable_section_container_generator_vcl_vector_txx_
#define vipl_filterable_section_container_generator_vcl_vector_txx_

#include <vcl_vector.h>

#define FILTER_IMPTR_DEC_REFCOUNT(v) ((v)=0)
#define FILTER_IMPTR_INC_REFCOUNT(v)
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

#include <vipl/section/vipl_section_container.txx>
#include <vipl/section/vipl_section_iterator.txx>
template <class DataType>
vipl_section_container<DataType>*
vipl_filterable_section_container_generator(const vcl_vector<DataType>& vec, DataType*)
{
  vipl_section_container<DataType> *rtn = new vipl_section_container<DataType>((vipl_section_container<DataType>*)0);
  rtn->put_imgptr((const void*) &vec);
  rtn->ref_imgsz()[0] = vec.size();
  rtn->ref_imgsz()[1] = 1;

  rtn->ref_secsz()[0] = vec.size();
  rtn->ref_secsz()[1] = 1;

  return rtn;
}

#endif // vipl_filterable_section_container_generator_vcl_vector_txx_
