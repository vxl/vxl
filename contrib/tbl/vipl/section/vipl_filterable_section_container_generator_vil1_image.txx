#ifndef vipl_filterable_section_container_generator_vil1_image_txx_
#define vipl_filterable_section_container_generator_vil1_image_txx_

#include <vil1/vil1_image.h>

#undef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#define FILTER_IMPTR_INC_REFCOUNT(v)
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

#include <vipl/section/vipl_section_container.txx>
#include <vipl/section/vipl_section_iterator.txx>
template <class DataType>
vipl_section_container<DataType>*
vipl_filterable_section_container_generator(const vil1_image& im, DataType*)
{
  vipl_section_container<DataType> *rtn = new vipl_section_container<DataType>((vipl_section_container<DataType>*)0);
  rtn->put_imgptr((const void*) &im);
  rtn->ref_imgsz()[0] = im.width();
  rtn->ref_imgsz()[1] = im.height();

  rtn->ref_secsz()[0] = im.width();  // should actually be block_width for block-buffered images
  rtn->ref_secsz()[1] = im.height(); // should actually be block_height for block-buffered images

  return rtn;
}

#endif // vipl_filterable_section_container_generator_vil1_image_txx_
