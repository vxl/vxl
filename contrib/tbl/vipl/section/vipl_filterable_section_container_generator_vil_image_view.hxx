#ifndef vipl_filterable_section_container_generator_vil_image_view_hxx_
#define vipl_filterable_section_container_generator_vil_image_view_hxx_

#include <vil/vil_image_view.h>

#undef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) { delete (v); (v)=0; }
#undef FILTER_IMPTR_INC_REFCOUNT
#define FILTER_IMPTR_INC_REFCOUNT(v)
#undef FILTER_KERNPTR_DEC_REFCOUNT
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

#include <vipl/section/vipl_section_container.hxx>
#include <vipl/section/vipl_section_iterator.hxx>
template <class DataType>
vipl_section_container<DataType>*
vipl_filterable_section_container_generator(const vil_image_view<DataType>& im, DataType*)
{
  vipl_section_container<DataType> *rtn = new vipl_section_container<DataType>((vipl_section_container<DataType>*)nullptr);
  rtn->put_imgptr((const void*) &im);
  rtn->ref_imgsz()[0] = im.ni();
  rtn->ref_imgsz()[1] = im.nj();

  rtn->ref_secsz()[0] = im.ni(); // should actually be block_width for block-buffered images
  rtn->ref_secsz()[1] = im.nj(); // should actually be block_height for block-buffered images

  return rtn;
}

#endif // vipl_filterable_section_container_generator_vil_image_view_hxx_
