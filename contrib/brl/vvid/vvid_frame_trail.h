//this-sets-emacs-to-*-c++-*-mode
#ifndef vvid_frame_trail_h_
#define vvid_frame_trail_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Maintains a trail of topology or spatial objects in time-order
// 
//    The idea is to display video segmentation for a given number 
//    of frames in a finite queue. The most recent frame result is 
//    added and the oldest result is removed. Creates the impression
//    of a feature-time trail in the current frame display
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy March 15, 2003    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_deque.h>
#include <vtol/vtol_topology_object_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

class vvid_frame_trail
{
 public:
  vvid_frame_trail();
  ~vvid_frame_trail();

  void set_window(int window){window_ = window;}
  void clear();
  //: add to queue
  void 
    add_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& sos);
  void 
    add_topology_objects(vcl_vector<vtol_topology_object_sptr> const & tos);

  //: get current display queue
  void get_topology_objects(vcl_vector<vtol_topology_object_sptr>& topo_objs);
  void get_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr>& spat_objs);

 private:
  //members
  int window_;//number of frames to maintain
  vcl_deque< vcl_vector<vtol_topology_object_sptr> > topo_queue_;
  vcl_deque< vcl_vector<vsol_spatial_object_2d_sptr> > spat_obj_queue_;
};

#endif // vvid_frame_trail_h_
