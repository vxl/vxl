#ifndef vipl_filterable_section_container_generator_vnl_vector_hxx_
#define vipl_filterable_section_container_generator_vnl_vector_hxx_

#include <vnl/vnl_vector.h>

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
vipl_filterable_section_container_generator(const vnl_vector<DataType>& vec, DataType*)
{
  vipl_section_container<DataType> *rtn = new vipl_section_container<DataType>((vipl_section_container<DataType>*)nullptr);
  rtn->put_imgptr((const void*) &vec);
  rtn->ref_imgsz()[0] = vec.size();
  rtn->ref_imgsz()[1] = 1;

  rtn->ref_secsz()[0] = vec.size();
  rtn->ref_secsz()[1] = 1;

  return rtn;
}

#endif // vipl_filterable_section_container_generator_vnl_vector_hxx_
