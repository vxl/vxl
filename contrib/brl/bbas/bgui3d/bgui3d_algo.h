// This is basic/bgui3d/bgui3d_algo.h
#ifndef bgui3d_algo_h_
#define bgui3d_algo_h_
//:
// \file
// \brief Algorithms used by bgui3d that have no other home (for now)
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 9/28/04
//
// \verbatim
//  Modifications
// \endverbatim


#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>


//: Decompose the camera into internal and external params
bool
bgui3d_decompose_camera( const vnl_double_3x4& camera,
                               vnl_double_3x3& internal,
                               vnl_double_3x3& rotation,
                               vnl_double_3&   translation );


#endif // bgui3d_algo_h_
