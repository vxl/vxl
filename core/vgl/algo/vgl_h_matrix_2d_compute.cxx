//--*-c++-*---
#include <vgl/algo/vgl_h_matrix_2d_compute.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vcl_vector.h>

bool vgl_h_matrix_2d_compute::
compute_p(vcl_vector<vgl_homg_point_2d<double> > const&p1,
          vcl_vector<vgl_homg_point_2d<double> > const&p2,
          vgl_h_matrix_2d<double>&  H)
{
  return compute_p(p1, p2, H);
}

bool vgl_h_matrix_2d_compute::
compute_l(vcl_vector<vgl_homg_line_2d<double> > const&l1,
          vcl_vector<vgl_homg_line_2d<double> > const&l2,
          vgl_h_matrix_2d<double>& H)
{
  return compute_l(l1, l2, H);
}

bool vgl_h_matrix_2d_compute::
compute_pl(vcl_vector<vgl_homg_point_2d<double> > const& /*points1*/,
           vcl_vector<vgl_homg_point_2d<double> > const& /*points2*/, 
           vcl_vector<vgl_homg_line_2d<double> > const& /*lines1*/,
           vcl_vector<vgl_homg_line_2d<double> > const& /*lines2*/,
           vgl_h_matrix_2d<double>& /*H*/)
{
  vcl_cerr << "\nvgl_h_matrix_2d_compute::compute_pl() :\n"
           << "This is a virtual method which should have been\n"
           << "overridden by a class derived from vgl_h_matrix_2d_compute.\n"
           << "The derived class may have omitted to implement\n"
           << "enough of the methods compute_p(), compute_l() and\n"
           << "compute_pl().\n";
  vcl_abort();
  return false;
}

//--------------------------------------------------------------------------------
//
//  the remaining functions just call the above functions.
//
//--------------------------------------------------------------------------------

bool vgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_point_2d<double> > const& p1,
        vcl_vector<vgl_homg_point_2d<double> > const& p2,
        vgl_h_matrix_2d<double>&  H)
{
  return compute_p(p1, p2, H);
}

vgl_h_matrix_2d<double> vgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_point_2d<double> > const& p1,
        vcl_vector<vgl_homg_point_2d<double> > const& p2)
{
  vgl_h_matrix_2d<double>  H;
  compute_p(p1, p2, H);
  return H;
}

//---------------------------------------------------------------------------

bool vgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_line_2d<double> > const&l1,
        vcl_vector<vgl_homg_line_2d<double> > const&l2,
        vgl_h_matrix_2d<double>& H)
{
  return compute_l(l1, l2, H);
}

vgl_h_matrix_2d<double> vgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_line_2d<double> > const&l1,
        vcl_vector<vgl_homg_line_2d<double> > const&l2)
{
  vgl_h_matrix_2d<double>  H;
  compute_l(l1, l2, H);
  return H;
}

//-----------------------------------------------------------------------------

bool vgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_point_2d<double> > const& p1,
        vcl_vector<vgl_homg_point_2d<double> > const& p2,
        vcl_vector<vgl_homg_line_2d<double> > const& l1,
        vcl_vector<vgl_homg_line_2d<double> > const& l2,
        vgl_h_matrix_2d<double>& H)
{
  return compute_pl(p1, p2, l1, l2, H);
}

vgl_h_matrix_2d<double> vgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_point_2d<double> > const& p1,
        vcl_vector<vgl_homg_point_2d<double> > const& p2,
        vcl_vector<vgl_homg_line_2d<double> > const&  l1,
        vcl_vector<vgl_homg_line_2d<double> > const&  l2)
{
  vgl_h_matrix_2d<double>  H;
  compute_pl(p1, p2, l1, l2, H);
  return H;
}

