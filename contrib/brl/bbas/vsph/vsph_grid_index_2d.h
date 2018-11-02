#ifndef vsph_grid_index_2d_h_
#define vsph_grid_index_2d_h_
//:
// \file
// \brief  A grid on the surface of a unit sphere for indexing points
// \author J. L. Mundy
// \date February 9, 2014
//  bin membership is defined as vi<=v<vi+1
//  except at the end of the range
//  for elevation two special bins are allocated for elev = 0 and pi
//  since the azimuth is not defined for these points on the sphere
//  for azimuth  -180 < v < -180+inc and 180-inc <= v <= p180
//  where a point with phi = -180 is put in the bin bounded by +180
//
#include <iostream>
#include <vector>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vsph_sph_point_2d.h"
#include "vsph_defs.h"//DIST_TOL

class vsph_grid_index_2d
{
 public:
  vsph_grid_index_2d();
  vsph_grid_index_2d(unsigned n_bins_theta, unsigned n_bins_phi,
                     bool in_radians = true);
  //: grid index corresponding to spherical point sp
  bool index(vsph_sph_point_2d const& sp, unsigned& th_idx, unsigned& ph_idx) const;
  //: insert a point in the index
  //  \returns false if a point already exists or coordinates are outside the range of the index
  bool insert(vsph_sph_point_2d const& sp, int id=-1);

  //: find if a point is near sp within tolerance
  //  \return the point id
  //  an id of -1 indicates that the point id was not defined or the
  //  point was not found
  bool find(vsph_sph_point_2d const& sp, unsigned& th_idx, unsigned& ph_idx,
            int& id, double tol = DIST_TOL )const;

  //: clear all points from the index. Maintain the bin structure
  void clear();

 private:
  double pye() const;//returns pi in the units of *this
  unsigned n_bins_theta_;
  unsigned n_bins_phi_;
  bool in_radians_;
  //  theta           phi
  std::vector<std::vector<std::vector<std::pair<vsph_sph_point_2d, int> > > > index_;
  double theta_inc_;//elevation angle
  double phi_inc_;  //azimuth angle
};

#endif
