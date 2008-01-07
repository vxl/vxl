#include "vvid_frame_trail.h"
//:
// \file
#include <vtol/vtol_topology_object.h>
#include <vsol/vsol_spatial_object_2d.h>

vvid_frame_trail::vvid_frame_trail()
{
  window_ = 0;
}

vvid_frame_trail::~vvid_frame_trail()
{
}

void vvid_frame_trail::clear()
{
  spat_obj_queue_.clear();
  topo_queue_.clear();
}

void vvid_frame_trail::
add_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr> const& sos)
{
  //if the window is full, eliminate oldest objects
  if (spat_obj_queue_.size()>window_)
    spat_obj_queue_.pop_back();

  spat_obj_queue_.push_front(sos);
}

void vvid_frame_trail::
add_topology_objects(vcl_vector<vtol_topology_object_sptr> const & tos)
{
  //update the spatial object queue

  //if the window is full, eliminate oldest objects
  if (topo_queue_.size()>window_)
    topo_queue_.pop_back();

  topo_queue_.push_front(tos);
}


void vvid_frame_trail::
get_topology_objects(vcl_vector<vtol_topology_object_sptr>& topo_objs)
{
  topo_objs.clear();
  for (vcl_deque< vcl_vector<vtol_topology_object_sptr> >::iterator tpt = topo_queue_.begin();
       tpt != topo_queue_.end(); tpt++)
    for (vcl_vector<vtol_topology_object_sptr>:: iterator tot = (*tpt).begin();
         tot != (*tpt).end(); tot++)
      topo_objs.push_back(*tot);
}

void vvid_frame_trail::
get_spatial_objects(vcl_vector<vsol_spatial_object_2d_sptr>& spat_objs)
{
  spat_objs.clear();
  for (vcl_deque<vcl_vector<vsol_spatial_object_2d_sptr> >::iterator sit =
       spat_obj_queue_.begin(); sit != spat_obj_queue_.end(); sit++)
    for (vcl_vector<vsol_spatial_object_2d_sptr>::iterator sot = (*sit).begin();
         sot != (*sit).end(); sot++)
      spat_objs.push_back(*sot);
}
