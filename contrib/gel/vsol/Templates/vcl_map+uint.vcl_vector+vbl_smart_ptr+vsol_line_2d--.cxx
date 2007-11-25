#include <vcl_map.txx>
#include <vcl_vector.h>
#include <vsol/vsol_line_2d_sptr.h>
typedef vcl_vector<vsol_line_2d_sptr> lines;
VCL_MAP_INSTANTIATE(unsigned int, lines, vcl_less<unsigned int>)
