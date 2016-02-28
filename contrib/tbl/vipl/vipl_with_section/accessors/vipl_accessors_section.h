#ifndef vipl_accessors_section_h_
#define vipl_accessors_section_h_
#include <section/section.h>

#undef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#undef FILTER_IMPTR_INC_REFCOUNT
#define FILTER_IMPTR_INC_REFCOUNT(v)
#undef FILTER_KERNPTR_DEC_REFCOUNT
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

template <class DataType> DataType fgetpixel(section<DataType,2> const& i, int x, int y, DataType);
template <class DataType> void fsetpixel(section<DataType,2>& i, int x, int y, DataType e);
template <class DataType> DataType getpixel(section<DataType,2> const& i, int x, int y, DataType);
template <class DataType> void setpixel(section<DataType,2>& i, int x, int y, DataType e);
#undef VIPL_DECLARE_ACCESSORS
#define VIPL_DECLARE_ACCESSORS(T) \
T fgetpixel(section<T,2> const& i, int x, int y, T); \
void fsetpixel(section<T,2>& i, int x, int y, T e); \
T getpixel(section<T,2> const& i, int x, int y, T); \
void setpixel(section<T,2>& i, int x, int y, T e)

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_accessors_section.hxx"
#endif

#endif // vipl_accessors_section_h_
