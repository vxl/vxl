// This is gel/pop/pop_homography_2d.h
#ifndef pop_homography_2d_h_
#define pop_homography_2d_h_
//:
// \file
// \brief this is a vertex/coordinate system in a coordinate system graph
//
// \author
//            Peter Tu April 2003
//            General Electric
//
//-------------------------------------------------------------------------------

#include<pop/pop_object.h>
#include<vgl/algo/vgl_h_matrix_2d.h>
#include<pop/pop_transform.h>
#include<pop/pop_vertex.h>
#include<std::vector.h>
#include<pop/pop_parameter.h>
#include<pop/pop_geometric_object.h>

//: A parameter that can be optimized
class pop_homography_2d : public pop_transform
{
 public:
  //: constructor
  // We have an arbitrary 3 by 3 matrix
  // |p1 p2 p3|
  // |p3 p4 p5|
  // |p6 p7 p8|
  //
  pop_homography_2d(std::vector<pop_parameter*> params,
                    pop_vertex *cs1, pop_vertex *cs2);

  //: destructor
  ~pop_homography_2d();

  //: transform a geometric object
  virtual pop_geometric_object* transform(pop_geometric_object *obj);

  //: update the transform based on the parameters
  virtual void update();

 private:
  vgl_h_matrix_2d<double> trans_;
};

#endif // pop_homography_2d_h_
