#ifndef vipl_accessors_mil_image_h_
#define vipl_accessors_mil_image_h_
#include <mil/mil_image_2d_of.h>

#define FILTER_IMPTR_DEC_REFCOUNT(v) ((v)=0)
#define FILTER_IMPTR_INC_REFCOUNT(v)
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

template <class DataType> DataType fgetpixel(mil_image_2d_of<DataType> const& i, int x, int y, DataType);
template <class DataType> void fsetpixel(mil_image_2d_of<DataType>& i, int x, int y, DataType e);
template <class DataType> DataType getpixel(mil_image_2d_of<DataType> const& i, int x, int y, DataType);
template <class DataType> void setpixel(mil_image_2d_of<DataType>& i, int x, int y, DataType e);
#define VIPL_DECLARE_ACCESSORS(T) \
T fgetpixel(mil_image_2d_of<T> const& i, int x, int y, T); \
void fsetpixel(mil_image_2d_of<T>& i, int x, int y, T e); \
T getpixel(mil_image_2d_of<T> const& i, int x, int y, T); \
void setpixel(mil_image_2d_of<T>& i, int x, int y, T e)

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_accessors_mil_image.txx"
#endif

#endif // vipl_accessors_mil_image_h_
