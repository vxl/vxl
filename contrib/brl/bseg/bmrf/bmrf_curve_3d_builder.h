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
#include <vcl_set.h>

//: A 3D curve builder
class bmrf_curve_3d_builder 
{
 public:
  //: Constructor
  bmrf_curve_3d_builder();
  bmrf_curve_3d_builder(bmrf_network_sptr);
  //: Destructor
  ~bmrf_curve_3d_builder() {}

  //: Build The curves
  vcl_set<bmrf_curvel_3d_sptr> build();

 protected:
  //: The network
  bmrf_network_sptr network_;

};

#endif // bmrf_curve_3d_builder_h_
