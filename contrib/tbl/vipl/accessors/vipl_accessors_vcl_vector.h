#ifndef vipl_accessors_vcl_vector_h_
#define vipl_accessors_vcl_vector_h_
#include <vcl_vector.h>

#undef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#define FILTER_IMPTR_INC_REFCOUNT(v)
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

template <class DataType> DataType fgetpixel(vcl_vector<DataType> const& i, int x, int y, DataType);
template <class DataType> void fsetpixel(vcl_vector<DataType>& i, int x, int y, DataType e);
template <class DataType> DataType getpixel(vcl_vector<DataType> const& i, int x, int y, DataType);
template <class DataType> void setpixel(vcl_vector<DataType>& i, int x, int y, DataType e);
#undef VIPL_DECLARE_1D_ACCESSORS
#define VIPL_DECLARE_1D_ACCESSORS(T) \
T fgetpixel(vcl_vector<T> const& i, int x, int y, T); \
void fsetpixel(vcl_vector<T>& i, int x, int y, T e); \
T getpixel(vcl_vector<T> const& i, int x, int y, T); \
void setpixel(vcl_vector<T>& i, int x, int y, T e)

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_accessors_vcl_vector.txx"
#endif

#endif // vipl_accessors_vcl_vector_h_
