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

#include <bmrf/bmrf_curvel_3d_sptr.h>
#include <bmrf/bmrf_network_sptr.h>
#include <bmrf/bmrf_node_sptr.h>
#include <bmrf/bmrf_arc_sptr.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vcl_utility.h>
#include <vcl_set.h>
#include <vcl_list.h>
#include <vcl_vector.h>

//: A 3D curve builder
class bmrf_curve_3d_builder
{
 public:
  typedef vcl_pair<bmrf_arc_sptr, bmrf_curvel_3d_sptr> time_match;
  typedef vcl_vector<time_match> match_vector;

  //: Constructor
  bmrf_curve_3d_builder();
  bmrf_curve_3d_builder(bmrf_network_sptr);
  //: Destructor
  ~bmrf_curve_3d_builder() {}

  //: Set the network
  void set_network(const bmrf_network_sptr& network);

  //: Build The curves
  //  Curves with less than \p min_prj projections are removed
  bool build(int min_prj = 3, int min_len = 10, float sigma = 0.5);

  //: Return the constructed curves
  vcl_set<vcl_list<bmrf_curvel_3d_sptr> > curves() const;

 protected:
  //: Initialize the intrinsic camera parameters
  void init_intrinsic();

  //: Initialize the camera matrices
  void init_cameras();

  //: Determine the alpha bounds from the network
  void find_alpha_bounds();

  //: Build curvels by matching curves in all frames at \p alpha
  //  Curves with less than \p min projections are removed
  vcl_set<bmrf_curvel_3d_sptr> build_curvels(double alpha);

  //: Find all curves that intersect \p alpha in \p frame
  vcl_set<bmrf_node_sptr> find_curves_at(double alpha, int frame = -1);

  //: return the node iterator in \p choices that best matches \p curvel at \p alpha
  bmrf_node_sptr best_match( const bmrf_curvel_3d_sptr& curvel, 
                             vcl_set<bmrf_node_sptr>& choices,
                             double alpha, int frame ) const;

  //: return all valid matches from curvels to nodes in \p frame
  void all_matches( const bmrf_curvel_3d_sptr& curvel,
                    const vcl_set<bmrf_node_sptr>& choices,
                    int frame,
                    match_vector& matches) const;

  //: Reconstruct the 3d location of a curvel from its projections
  void reconstruct_point(bmrf_curvel_3d_sptr curvel) const;

  //: Simultaneously reconstruct all points in a 3d curve
  void reconstruct_curve(vcl_list<bmrf_curvel_3d_sptr>& curve, float sigma = 0.5) const;

  //: Attempt to fill in missing correspondences
  void fill_gaps(vcl_list<bmrf_curvel_3d_sptr>& curve);

  //: Trim the ends of the curve with few correspondences
  void trim_curve(vcl_list<bmrf_curvel_3d_sptr>& curve, int min_prj);

  //: Match the \p curvels to the ends of the \p growing_curves
  void append_curvels(vcl_set<bmrf_curvel_3d_sptr> curvels,
                      vcl_set<vcl_list<bmrf_curvel_3d_sptr>*>& growing_curves);\

  //: Return a measure (0.0 to 1.0) of how well \p new_c matches \p prev_c
  double append_correct(const bmrf_curvel_3d_sptr& new_c, const bmrf_curvel_3d_sptr& prev_c) const;

 protected:
  //: The network
  bmrf_network_sptr network_;

  //: Bounds on the alpha values in the network;
  double min_alpha_;
  double max_alpha_;

  vcl_set<vcl_list<bmrf_curvel_3d_sptr> > curves_;

  //: Camera intrinsic parameters
  vnl_double_3x3 K_;

  //: Vector of cameras
  vcl_vector<vnl_double_3x4> C_;
};

#endif // bmrf_curve_3d_builder_h_
