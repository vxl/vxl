#ifndef vipl_accessors_vcl_vector_h_
#define vipl_accessors_vcl_vector_h_
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#undef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#undef FILTER_IMPTR_INC_REFCOUNT
#define FILTER_IMPTR_INC_REFCOUNT(v)
#undef FILTER_KERNPTR_DEC_REFCOUNT
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

template <class DataType> DataType fgetpixel(std::vector<DataType> const& i, int x, int y, DataType);
template <class DataType> void fsetpixel(std::vector<DataType>& i, int x, int y, DataType e);
template <class DataType> DataType getpixel(std::vector<DataType> const& i, int x, int y, DataType);
template <class DataType> void setpixel(std::vector<DataType>& i, int x, int y, DataType e);
#undef VIPL_DECLARE_1D_ACCESSORS
#define VIPL_DECLARE_1D_ACCESSORS(T) \
T fgetpixel(std::vector<T> const& i, int x, int y, T); \
void fsetpixel(std::vector<T>& i, int x, int y, T e); \
T getpixel(std::vector<T> const& i, int x, int y, T); \
void setpixel(std::vector<T>& i, int x, int y, T e)

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_accessors_vcl_vector.hxx"
#endif

#endif // vipl_accessors_vcl_vector_h_
