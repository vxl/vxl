#ifndef vipl_filterable_section_container_generator_mil_image_txx_
#define vipl_filterable_section_container_generator_mil_image_txx_

#include <mil/mil_image_2d_of.h>

#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#define FILTER_IMPTR_INC_REFCOUNT(v)
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

#include <vipl/section/vipl_section_container.txx>
#include <vipl/section/vipl_section_iterator.txx>
template <class DataType>
vipl_section_container<DataType>*
vipl_filterable_section_container_generator(const mil_image_2d_of<DataType>& im, DataType*)
{
  vipl_section_container<DataType> *rtn = new vipl_section_container<DataType>((vipl_section_container<DataType>*)0);
  rtn->put_imgptr((const void*) &im);
  rtn->ref_imgsz()[0] = im.nx();
  rtn->ref_imgsz()[1] = im.ny();

  rtn->ref_secsz()[0] = im.nx();
  rtn->ref_secsz()[1] = im.ny();

  return rtn;
}

#endif // vipl_filterable_section_container_generator_mil_image_txx_
