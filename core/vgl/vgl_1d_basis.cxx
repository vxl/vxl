// This is core/vgl/vgl_1d_basis.cxx

#include "vgl_1d_basis.hxx"
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
