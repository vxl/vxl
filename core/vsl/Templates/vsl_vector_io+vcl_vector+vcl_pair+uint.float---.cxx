#include <utility>
#include <vcl_compiler.h>
#include <vsl/vsl_pair_io.h>
#include <vsl/vsl_vector_io.hxx>
typedef std::vector<std::pair<unsigned int,float> > vector_pair_uf;
VSL_VECTOR_IO_INSTANTIATE(vector_pair_uf);
