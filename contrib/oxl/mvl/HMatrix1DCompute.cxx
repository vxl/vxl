// This is oxl/mvl/HMatrix1DCompute.cxx
#include "HMatrix1DCompute.h"

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

#include <mvl/HMatrix1D.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vcl_vector.h>

//--------------------------------------------------------------------------------
//
//  misc. stuff
//
//--------------------------------------------------------------------------------

void HMatrix1DCompute::verbose(bool v) { verbose_=v; }

//--------------------------------------------------------------------------------
//
//  implementations of virtual functions :
//
//--------------------------------------------------------------------------------

bool
HMatrix1DCompute::compute_array_dbl(const double p1[],
                                    const double p2[],
                                    int N,
                                    HMatrix1D *H)
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
HMatrix1DCompute::compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> >&,
                                    const vcl_vector<vgl_homg_point_1d<double> >&,
                                    HMatrix1D *)
{
  vcl_cerr << "\nHMatrix1DCompute::compute_cool_homg() :\n"
           << "This is a virtual method which should have been\n"
           << "overridden by a class derived from HMatrix1DCompute.\n";
  vcl_abort();
  return false;
}

//--------------------------------------------------------------------------------
//
//  the remaining functions just call the above functions.
//
//--------------------------------------------------------------------------------

bool
HMatrix1DCompute::compute(const vcl_vector<vgl_homg_point_1d<double> >&p1,
                          const vcl_vector<vgl_homg_point_1d<double> >&p2,
                          HMatrix1D *H)
{
  return compute_cool_homg(p1,p2,H);
}
