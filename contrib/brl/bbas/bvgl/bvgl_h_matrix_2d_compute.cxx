//--*-c++-*---
#include "bvgl_h_matrix_2d_compute.h"

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

#include <bvgl/bvgl_h_matrix_2d.h>
#include <bvgl/bvgl_pair_match_set_corner.h>
#include <vcl_vector.h>

bool bvgl_h_matrix_2d_compute::
compute_p(vcl_vector<vgl_homg_point_2d<double> > const&p1,
          vcl_vector<vgl_homg_point_2d<double> > const&p2,
          bvgl_h_matrix_2d<double>&  H)
{
  return compute_p(p1, p2, H);
}

bool bvgl_h_matrix_2d_compute::
compute_l(vcl_vector<vgl_homg_line_2d<double> > const&l1,
          vcl_vector<vgl_homg_line_2d<double> > const&l2,
          bvgl_h_matrix_2d<double>& H)
{
  return compute_l(l1, l2, H);
}

bool bvgl_h_matrix_2d_compute::
compute_pl(vcl_vector<vgl_homg_point_2d<double> > const& points1,
           vcl_vector<vgl_homg_point_2d<double> > const& points2, 
           vcl_vector<vgl_homg_line_2d<double> > const& lines1,
           vcl_vector<vgl_homg_line_2d<double> > const& lines2,
           bvgl_h_matrix_2d<double>& H)
{
  vcl_cerr << vcl_endl;
  vcl_cerr << "bvgl_h_matrix_2d_compute::compute_pl() :" << vcl_endl;
  vcl_cerr << "This is a virtual method which should have been" << vcl_endl;
  vcl_cerr << "overridden by a class derived from bvgl_h_matrix_2d_compute." << vcl_endl;
  vcl_cerr << "The derived class may have omitted to implement" << vcl_endl;
  vcl_cerr << "enough of the methods compute_p(),compute_l() and" << vcl_endl;
  vcl_cerr << "compute_pl()." << vcl_endl;
  vcl_abort();
  return false;
}

//--------------------------------------------------------------------------------
//
//  the remaining functions just call the above functions.
//
//--------------------------------------------------------------------------------

bool bvgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_point_2d<double> > const& p1,
        vcl_vector<vgl_homg_point_2d<double> > const& p2,
        bvgl_h_matrix_2d<double>&  H)
{
  return compute_p(p1, p2, H);
}

bvgl_h_matrix_2d<double> bvgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_point_2d<double> > const& p1,
        vcl_vector<vgl_homg_point_2d<double> > const& p2)
{
  bvgl_h_matrix_2d<double>  H;
  compute_p(p1, p2, H);
  return H;
}

//---------------------------------------------------------------------------

bool bvgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_line_2d<double> > const&l1,
        vcl_vector<vgl_homg_line_2d<double> > const&l2,
        bvgl_h_matrix_2d<double>& H)
{
  return compute_l(l1, l2, H);
}

bvgl_h_matrix_2d<double> bvgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_line_2d<double> > const&l1,
        vcl_vector<vgl_homg_line_2d<double> > const&l2)
{
  bvgl_h_matrix_2d<double>  H;
  compute_l(l1, l2, H);
  return H;
}

//-----------------------------------------------------------------------------

bool bvgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_point_2d<double> > const& p1,
        vcl_vector<vgl_homg_point_2d<double> > const& p2,
        vcl_vector<vgl_homg_line_2d<double> > const& l1,
        vcl_vector<vgl_homg_line_2d<double> > const& l2,
        bvgl_h_matrix_2d<double>& H)
{
  return compute_pl(p1, p2, l1, l2, H);
}

bvgl_h_matrix_2d<double> bvgl_h_matrix_2d_compute::
compute(vcl_vector<vgl_homg_point_2d<double> > const& p1,
        vcl_vector<vgl_homg_point_2d<double> > const& p2,
        vcl_vector<vgl_homg_line_2d<double> > const&  l1,
        vcl_vector<vgl_homg_line_2d<double> > const&  l2)
{
  bvgl_h_matrix_2d<double>  H;
  compute_pl(p1, p2, l1, l2, H);
  return H;
}

