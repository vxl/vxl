// This is core/vcsl/examples/vcsl_tutor.cxx

//:
// \file
// \brief Demonstrates basic use of vcsl coordinate system library
// \author Rob.Campbell@att.net
//
// Abbreviations used in comments:
// - CS coordinate system
//
// Required libraries: vcl, vbl, vnl, vcsl

#include <vcsl/vcsl_cartesian_3d.h>
#include <vcsl/vcsl_rotation.h>
#include <vnl/vnl_math.h>
#include <vcsl/vcsl_graph.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

int main()
{
  //: Convenient indices for vectors representing points, etc.
  enum AXES { X, Y, Z, T };

  //: Graph of defined CS
  // All CS must be added to this graph as they are created.
  vcsl_graph_sptr graphCS = new vcsl_graph;

  //: World Coordinate System
  // Equivalent to the Top view, by definition.
  //
  // A CS for which no parent is defined is absolute.  Logically,
  // there must be one absolute CS which all other directly or
  // indirectly relate to.
  vcsl_spatial_sptr WCS = new vcsl_cartesian_3d;

  // Add WCS to the global map of coordinate systems.
  WCS->set_graph(graphCS);

  // New CS can be defined relative to any point (translation) or
  // axis (rotation) in the parent CS.  But it is so common to do
  // so relative to the X, Y, or Z axes, that it is convenient to
  // define the corresponding vectors:

  //: x-axis vector
  vnl_vector_fixed <double, 3> xA;
  xA[X]=1; xA[Y] = 0; xA[Z] = 0;

  //: y-axis vector
  vnl_vector_fixed <double, 3> yA;
  yA[X]=0; yA[Y] = 1; yA[Z] = 0;

  //: z-axis vector
  vnl_vector_fixed <double, 3> zA;
  zA[X]=0; zA[Y] = 0; zA[Z] = 1;

  //: 90 degree rotation about WCS y-axis
  // Transforms from WCS to right CS
  vcsl_rotation rightXF;
  rightXF.set_static(vnl_math::pi_over_2, yA.as_ref());

  //: WCS rotated 90 degrees about the y-axis to produce right hand view/CS
  vcsl_spatial_sptr right = new vcsl_cartesian_3d;
  right->set_graph(graphCS);
  right->set_unique(WCS, &rightXF);

  //: Corner of a box with opposite corner at origin of WCS.
  vnl_vector_fixed <double, 3> corner;
  corner[X] = 1; corner[Y] = 2; corner[Z] = 3;

  // By inspection, corner should be (-3,2,1) in 'right' CS
  vnl_vector<double> cornerXF = WCS->from_local_to_cs(corner.as_ref(), right, 0);

  vcl_cout << cornerXF[0] << ", " << cornerXF[1] << ", " << cornerXF[2] << '\n';
  assert(vcl_abs(cornerXF[0]+3) < 1e-6);
  assert(vcl_abs(cornerXF[1]-2) < 1e-6);
  assert(vcl_abs(cornerXF[2]-1) < 1e-6);

  // Note that at this point, none of the smart pointers should be deleted
  // since smart pointers, by definition, clean up themselves:
//delete right->parent(); delete right; // don't !!

  return 0;
}
