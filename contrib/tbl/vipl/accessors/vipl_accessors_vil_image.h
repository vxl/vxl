#ifndef vipl_accessors_vil_image_h_
#define vipl_accessors_vil_image_h_
#include <vil/vil_image.h>

template <class DataType> DataType fgetpixel(vil_image const& i, int x, int y, DataType);
template <class DataType> void fsetpixel(vil_image& i, int x, int y, DataType e);
template <class DataType> DataType getpixel(vil_image const& i, int x, int y, DataType);
template <class DataType> void setpixel(vil_image& i, int x, int y, DataType e);
#define VIPL_DECLARE_ACCESSORS(T) \
T fgetpixel(vil_image const& i, int x, int y, T); \
void fsetpixel(vil_image& i, int x, int y, T e); \
T getpixel(vil_image const& i, int x, int y, T); \
void setpixel(vil_image& i, int x, int y, T e)

#endif // vipl_accessors_vil_image_h_
