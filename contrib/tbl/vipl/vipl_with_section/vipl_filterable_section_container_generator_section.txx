#ifndef vipl_filterable_section_container_generator_section_txx_
#define vipl_filterable_section_container_generator_section_txx_

#include <section/section.h>

#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#define FILTER_IMPTR_INC_REFCOUNT(v)
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

#include <vipl/section/vipl_section_container.txx>
#include <vipl/section/vipl_section_iterator.txx>
template <class DataType>
vipl_section_container<DataType>*
vipl_filterable_section_container_generator(const section<DataType,2>& im, DataType*)
{
  vipl_section_container<DataType> *rtn = new vipl_section_container<DataType>((vipl_section_container<DataType>*)0);
  rtn->put_imgptr((const void*) &im);
  rtn->ref_imgsz()[0] = im.Size(0);
  rtn->ref_imgsz()[1] = im.Size(1);

  rtn->ref_secsz()[0] = im.Size(0);
  rtn->ref_secsz()[1] = im.Size(1);

  return rtn;
}

#endif // vipl_filterable_section_container_generator_section_txx_
