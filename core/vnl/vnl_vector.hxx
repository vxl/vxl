// This is core/vnl/vnl_vector.hxx
#ifndef vnl_vector_hxx_
#define vnl_vector_hxx_

////--------------------------------------------------------------------------------
//
//// The instantiation macros are split because some functions (angle, cos_angle)
//// shouldn't be instantiated for complex and/or integral types.
//
//#define VNL_VECTOR_INSTANTIATE_COMMON(T) \
//template class VNL_EXPORT vnl_vector<T >; \
///* element-wise */ \
//template VNL_EXPORT vnl_vector<T > element_product(vnl_vector<T > const &, vnl_vector<T > const &); \
//template VNL_EXPORT vnl_vector<T > element_quotient(vnl_vector<T > const &, vnl_vector<T > const &); \
///* dot products, angles etc */ \
//template VNL_EXPORT T inner_product(vnl_vector<T > const &, vnl_vector<T > const &); \
//template VNL_EXPORT T dot_product(vnl_vector<T > const &, vnl_vector<T > const &); \
//template VNL_EXPORT T bracket(vnl_vector<T > const &, vnl_matrix<T > const &, vnl_vector<T > const &); \
//template VNL_EXPORT vnl_matrix<T > outer_product(vnl_vector<T > const &,vnl_vector<T > const &); \
///* I/O */ \
//template VNL_EXPORT std::ostream & operator<<(std::ostream &, vnl_vector<T > const &); \
//template VNL_EXPORT std::istream & operator>>(std::istream &, vnl_vector<T >       &)
//
#define VNL_VECTOR_INSTANTIATE(T) \

//VNL_VECTOR_INSTANTIATE_COMMON(T); \
//template VNL_EXPORT T cos_angle(vnl_vector<T > const & , vnl_vector<T > const &); \
//template VNL_EXPORT double angle(vnl_vector<T > const & , vnl_vector<T > const &)
//
#define VNL_VECTOR_INSTANTIATE_COMPLEX(T) \

//VNL_VECTOR_INSTANTIATE_COMMON(T); \
//template VNL_EXPORT T cos_angle(vnl_vector<T > const & , vnl_vector<T > const &)

#endif // vnl_vector_hxx_
