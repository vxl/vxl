#include <iostream>
#include <vector>
#include "vgl/vgl_point_3d.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vbl/vbl_array_3d.hxx"
typedef std::vector<vgl_point_3d<double> > vpt;
VBL_ARRAY_3D_INSTANTIATE(vpt );
