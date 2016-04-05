#ifndef vipl_accessors_vnl_vector_h_
#define vipl_accessors_vnl_vector_h_
#include <vnl/vnl_vector.h>

#undef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#undef FILTER_IMPTR_INC_REFCOUNT
#define FILTER_IMPTR_INC_REFCOUNT(v)
#undef FILTER_KERNPTR_DEC_REFCOUNT
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

template <class DataType> DataType fgetpixel(vnl_vector<DataType> const& i, int x, int y, DataType);
template <class DataType> void fsetpixel(vnl_vector<DataType>& i, int x, int y, DataType e);
template <class DataType> DataType getpixel(vnl_vector<DataType> const& i, int x, int y, DataType);
template <class DataType> void setpixel(vnl_vector<DataType>& i, int x, int y, DataType e);
#undef VIPL_DECLARE_1D_ACCESSORS
#define VIPL_DECLARE_1D_ACCESSORS(T) \
T fgetpixel(vnl_vector<T> const& i, int x, int y, T); \
void fsetpixel(vnl_vector<T>& i, int x, int y, T e); \
T getpixel(vnl_vector<T> const& i, int x, int y, T); \
void setpixel(vnl_vector<T>& i, int x, int y, T e)

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_accessors_vnl_vector.hxx"
#endif

#endif // vipl_accessors_vnl_vector_h_
