#include <vcl/vcl_map.h>
#include <vcl/vcl_vector.txx>

typedef vcl_map<int, int, vcl_less<int> > T;
VCL_VECTOR_INSTANTIATE(T);
