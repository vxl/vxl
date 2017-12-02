#ifndef vipl_accessors_vil_image_view_base_h_
#define vipl_accessors_vil_image_view_base_h_
#include <vil/vil_image_view_base.h>

#undef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#undef FILTER_IMPTR_INC_REFCOUNT
#define FILTER_IMPTR_INC_REFCOUNT(v)
#undef FILTER_KERNPTR_DEC_REFCOUNT
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

template <class DataType> DataType fgetpixel(vil_image_view_base const& i, int x, int y, DataType);
template <class DataType> void fsetpixel(vil_image_view_base& i, int x, int y, DataType const e);
template <class DataType> DataType getpixel(vil_image_view_base const& i, int x, int y, DataType);
template <class DataType> void setpixel(vil_image_view_base& i, int x, int y, DataType const e);
#undef VIPL_DECLARE_ACCESSORS
#define VIPL_DECLARE_ACCESSORS(T) \
T fgetpixel(vil_image_view_base const& i, int x, int y, T); \
void fsetpixel(vil_image_view_base& i, int x, int y, T const e); \
T getpixel(vil_image_view_base const& i, int x, int y, T); \
void setpixel(vil_image_view_base& i, int x, int y, T const e)

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_accessors_vil_image_view_base.hxx"
#endif

#endif // vipl_accessors_vil_image_view_base_h_
