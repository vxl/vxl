// This is vxl/vgl/vgl_1d_basis.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vgl_1d_basis.txx"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>

VGL_1D_BASIS_INSTANTIATE(vgl_point_2d<double>);
VGL_1D_BASIS_INSTANTIATE(vgl_point_3d<double>);
VGL_1D_BASIS_INSTANTIATE(vgl_homg_point_1d<double>);
VGL_1D_BASIS_INSTANTIATE(vgl_homg_point_2d<double>);
VGL_1D_BASIS_INSTANTIATE(vgl_homg_point_3d<double>);

