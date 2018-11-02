#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsol/vsol_point_3d_sptr.h>
#include <vbl/vbl_array_2d.hxx>
typedef std::vector<std::vector<vsol_point_3d_sptr> >* vptr;
VBL_ARRAY_2D_INSTANTIATE(vptr);
