#ifndef vipl_accessors_vil1_image_h_
#define vipl_accessors_vil1_image_h_
#include <vil1/vil1_image.h>

#undef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#undef FILTER_IMPTR_INC_REFCOUNT
#define FILTER_IMPTR_INC_REFCOUNT(v)
#undef FILTER_KERNPTR_DEC_REFCOUNT
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

template <class DataType> DataType fgetpixel(vil1_image const& i, int x, int y, DataType);
template <class DataType> void fsetpixel(vil1_image& i, int x, int y, DataType e);
template <class DataType> DataType getpixel(vil1_image const& i, int x, int y, DataType);
template <class DataType> void setpixel(vil1_image& i, int x, int y, DataType e);
#undef VIPL_DECLARE_ACCESSORS
#define VIPL_DECLARE_ACCESSORS(T) \
T fgetpixel(vil1_image const& i, int x, int y, T); \
void fsetpixel(vil1_image& i, int x, int y, T e); \
T getpixel(vil1_image const& i, int x, int y, T); \
void setpixel(vil1_image& i, int x, int y, T e)

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_accessors_vil1_image.txx"
#endif

#endif // vipl_accessors_vil1_image_h_
