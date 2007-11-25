#include <vcl_map.txx>
#include <vcl_vector.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
typedef vcl_vector<vsol_digital_curve_2d_sptr> curves;
VCL_MAP_INSTANTIATE(unsigned int, curves, vcl_less<unsigned int>)
