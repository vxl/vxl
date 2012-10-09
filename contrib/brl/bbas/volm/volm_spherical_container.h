//This is brl/bbas/volm/volm_spherical_container.h
#ifndef volm_spherical_container_h_
#define volm_spherical_container_h_
//:
// \file
// \brief  A class to represent a spherical container with a fixed voxelization and a polar index to access its voxels
//
// \author
// \date October 07, 2012
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>

class volm_voxel
{
 public:
  enum ATTRIBUTES { SKY, PLANAR_HORIZONTAL, PLANAR_VERTICAL, PLANAR_OTHER, NON_PLANAR };

  double resolution_;
  vgl_point_3d<double>  center_;
};


class volm_spherical_container
{
 public:
  // REPRESENT voxels maybe ? vcl_vector<volm_voxel> voxels_;
  unsigned fetch_voxel(double dist, double theta, double phi);

  void draw_template(vcl_string vrml_file_name);

 protected:
  vcl_vector<volm_voxel> voxels_;
};

#endif // volm_spherical_container_h_
