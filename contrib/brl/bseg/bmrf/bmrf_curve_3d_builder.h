// This is brl/bseg/bmrf/bmrf_curve_3d_builder.h
#ifndef bmrf_curve_3d_builder_h_
#define bmrf_curve_3d_builder_h_
//:
// \file
// \brief A class to build 3D curves from a bmrf_network
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/23/04
//
// \verbatim
//  Modifications
// \endverbatim


#include <bmrf/bmrf_curve_3d_sptr.h>
#include <bmrf/bmrf_curvel_3d_sptr.h>
#include <bmrf/bmrf_network_sptr.h>
#include <bmrf/bmrf_node_sptr.h>
#include <bmrf/bmrf_arc_sptr.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_4x4.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_utility.h>
#include <vcl_set.h>
#include <vcl_map.h>
#include <vcl_vector.h>


//: A 3D curve builder
class bmrf_curve_3d_builder
{
 public:
  typedef vcl_pair<bmrf_arc_sptr, bmrf_curvel_3d_sptr> time_match;
  typedef vcl_vector<time_match> match_vector;

  //: Constructor
  bmrf_curve_3d_builder();
  bmrf_curve_3d_builder(const bmrf_network_sptr&);
  //: Destructor
  ~bmrf_curve_3d_builder() {}

  //: Set the network
  void set_network(const bmrf_network_sptr& network);

  //: Initialize the camera matrices (using a specified camera)
  // \param scale determines the separation between cameras
  void init_cameras(const vnl_double_3x4& C0, double scale = 1.0);
  
  //: Build The curves
  //  Curves with less than \p min_prj projections are removed
  bool build(int min_prj = 3, int min_len = 10);

  //: Reconstruct the 3D curves from the curvel chains
  void reconstruct(float sigma = 0.5);

  //: Compute the bounding box aligned with vehicle direction
  // \param inlier_fractions is an array of 3 float for inlier fraction in {X,Y,Z}
  bool compute_bounding_box(const float *inlier_fractions = NULL, bool align_ep = false);

  //: Return the constructed curves
  vcl_set<bmrf_curve_3d_sptr> curves() const;

  //: Return the cameras used in the reconstruction
  vcl_map<int,vnl_double_3x4> cameras() const;

  //: Return the 3D direction of motion of the curves
  vgl_vector_3d<double> direction() const;

  //: Return the bounding box transformation
  vnl_double_4x4 bb_xform() const;

 protected:
  //: Compute the relative change in spacing between cameras
  void compute_camera_offsets();
  
  //: Determine the alpha bounds from the network
  void find_alpha_bounds();
  
  //: Build curvels by linking across time through probable arcs
  vcl_set<bmrf_curvel_3d_sptr>
    build_curvels(vcl_set<bmrf_curvel_3d_sptr>& all_curvels, double alpha) const;

  //: extend all curves to the next alpha 
  vcl_set<bmrf_curvel_3d_sptr> 
    extend_curves( vcl_set<bmrf_curve_3d_sptr>& growing_curves, 
                   double alpha );

  //: Find all arcs where both nodes are valid at \p alpha
  vcl_vector<bmrf_arc_sptr> find_arcs_at(double alpha) const;

  //: Reconstruct the 3d location of a curvel from its projections
  void reconstruct_point(bmrf_curvel_3d_sptr curvel) const;

  //: Match the \p curvels to the ends of the \p growing_curves
  void append_curvels(vcl_set<bmrf_curvel_3d_sptr>& curvels,
                      vcl_set<bmrf_curve_3d_sptr>& growing_curves,
                      int min_prj);

  //: Return a measure (0.0 to 1.0) of how well \p new_c matches \p prev_c
  double append_correct(const bmrf_curvel_3d_sptr& new_c, const bmrf_curvel_3d_sptr& prev_c) const;

 protected:
  //: The network
  bmrf_network_sptr network_;

  //: Bounds on the alpha values in the network;
  double min_alpha_;
  double max_alpha_;

  vcl_set<bmrf_curve_3d_sptr> curves_;

  //: Map from frame numbers to cameras
  vcl_map<int,vnl_double_3x4> C_;

  //: Map from frame numbers to camera offsets
  vcl_map<int,double> offsets_;
  
  //: 3D direction unit vector
  vgl_vector_3d<double> direction_;

  //: This transform maps the unit cube into the vehicle aligned bounding box
  vnl_double_4x4 bb_xform_;
};

#endif // bmrf_curve_3d_builder_h_
