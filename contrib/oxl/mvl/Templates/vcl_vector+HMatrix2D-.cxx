// Instantiation of vcl_vector<HMatrix2D>
#include <vcl/vcl_vector.txx>
#include <mvl/HMatrix2D.h>

static bool operator==(HMatrix2D const& a, HMatrix2D const& b) { return &a == &b; }
VCL_VECTOR_INSTANTIATE(HMatrix2D);
