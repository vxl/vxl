// This is brl/bbas/vgl/algo/vgl_h_matrix_1d_compute.cxx
#include "vgl_h_matrix_1d_compute.h"

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

#include <vgl/algo/vgl_h_matrix_1d.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vcl_vector.h>

//--------------------------------------------------------------------------------
//
//  misc. stuff
//
//--------------------------------------------------------------------------------

void vgl_h_matrix_1d_compute::verbose(bool v) { verbose_=v; }

//--------------------------------------------------------------------------------
//
//  implementations of virtual functions :
//
//--------------------------------------------------------------------------------

bool
vgl_h_matrix_1d_compute::compute_array_dbl(const double p1[],
                                    const double p2[],
                                    int N,
                                    vgl_h_matrix_1d<double>& H)
{
  vcl_vector<vgl_homg_point_1d<double> > pt1;
  vcl_vector<vgl_homg_point_1d<double> > pt2;
  pt1.reserve(N);
  pt2.reserve(N);
  for (int i=0;i<N;i++) {
    pt1.push_back(vgl_homg_point_1d<double>(p1[i],1.0));
    pt2.push_back(vgl_homg_point_1d<double>(p2[i],1.0));
  }
  return compute_cool_homg(pt1,pt2,H);
}

bool
vgl_h_matrix_1d_compute::compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> > &,
                                    const vcl_vector<vgl_homg_point_1d<double> > &,
                                    vgl_h_matrix_1d<double>& H)
{
  vcl_cerr << "\nvgl_h_matrix_1d_compute::compute_cool_homg() :\n";
  vcl_cerr << "This is a virtual method which should have been\n";
  vcl_cerr << "overridden by a class derived from vgl_h_matrix_1d_compute.\n";
  vcl_abort();
  return false;
}

//--------------------------------------------------------------------------------
//
//  the remaining functions just call the above functions.
//
//--------------------------------------------------------------------------------

bool
vgl_h_matrix_1d_compute::compute(const vcl_vector<vgl_homg_point_1d<double> > &p1,
                          const vcl_vector<vgl_homg_point_1d<double> > &p2,
                          vgl_h_matrix_1d<double>& H)
{
  return compute_cool_homg(p1,p2,H);
}
