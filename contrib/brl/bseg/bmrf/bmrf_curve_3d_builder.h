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
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vcl_set.h>
#include <vcl_list.h>
#include <vcl_vector.h>
#include <vcl_map.h>

//: A 3D curve builder
class bmrf_curve_3d_builder 
{
 public:
  //: Constructor
  bmrf_curve_3d_builder();
  bmrf_curve_3d_builder(bmrf_network_sptr);
  //: Destructor
  ~bmrf_curve_3d_builder() {}

  //: Set the network
  void set_network(const bmrf_network_sptr& network);

  //: Build The curves
  bool build();

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
  //: curves with less than \p min projections are removed
  vcl_list<bmrf_curvel_3d_sptr> build_curvels(double alpha, int min = 3);

  //: Find all curves that intersect \p alpha in \p frame
  vcl_map<double, bmrf_node_sptr> find_curves_at(double alpha, int frame = -1);

  //: return the curvel in \p list that best matches \p node at \p alpha
  bmrf_curvel_3d_sptr best_match(const bmrf_node_sptr& node, 
                                 const vcl_list<bmrf_curvel_3d_sptr>& list,
                                 double alpha) const;

  //: Reconstruct the 3d location of a curvel from its projections
  void reconstruct_3d(bmrf_curvel_3d_sptr curvel) const;

  //: Match the \p curvels to the ends of the \p growing_curves
  void append_curvels(vcl_list<bmrf_curvel_3d_sptr> curvels, 
                      vcl_list<vcl_list<bmrf_curvel_3d_sptr>*>& growing_curves);\

  //: Return true if \p new_c is a reasonable match to \p prev_c 
  bool append_correct(const bmrf_curvel_3d_sptr& new_c, const bmrf_curvel_3d_sptr& prev_c) const;

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
