// This is brl/bseg/bmrf/bmrf_curve_3d_builder.cxx
//:
// \file

#include "bmrf_curve_3d_builder.h"
#include "bmrf_curvel_3d.h"
#include "bmrf_node.h"

#include <vnl/vnl_identity_3x3.h>


//: Constructor
bmrf_curve_3d_builder::bmrf_curve_3d_builder() 
 : network_(NULL)
{
}

//: Constructor
bmrf_curve_3d_builder::bmrf_curve_3d_builder(bmrf_network_sptr network)
 : network_(network)
{
}


//: Set the network
void 
bmrf_curve_3d_builder::set_network(const bmrf_network_sptr& network)
{
  network_ = network;
}


//: Build The curves
vcl_set<vcl_list<bmrf_curvel_3d_sptr> > 
bmrf_curve_3d_builder::build()
{
  vcl_set<vcl_list<bmrf_curvel_3d_sptr> > curves;
  vcl_cout << "Building curves" << vcl_endl;
  vcl_list<bmrf_curvel_3d_sptr> c1, c2;
  c1.push_back( new bmrf_curvel_3d(10.0, 10.0, 10.0, vnl_identity_3x3()) );
  c1.push_back( new bmrf_curvel_3d(5.0, 15.0, 10.0, vnl_identity_3x3()) );
  c1.push_back( new bmrf_curvel_3d(0.0, 15.0, 20.0, vnl_identity_3x3()) );
  curves.insert(c1);

  c2.push_back( new bmrf_curvel_3d(5.0, 15.0, 30.0, vnl_identity_3x3()) );
  c2.push_back( new bmrf_curvel_3d(10.0, 15.0, 40.0, vnl_identity_3x3()) );
  c2.push_back( new bmrf_curvel_3d(15.0, 10.0, 40.0, vnl_identity_3x3()) );
  c2.push_back( new bmrf_curvel_3d(20.0, 10.0, 40.0, vnl_identity_3x3()) );
  curves.insert(c2);

  return curves;
}
