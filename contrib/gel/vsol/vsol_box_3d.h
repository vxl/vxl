
#ifndef vsol_box_3d_H
#define vsol_box_3d_H


//
// .NAME        vsol_box_3d - A bounding box
// .LIBRARY     vsol
// .HEADER	vxl package
// .INCLUDE     vsol/vsol_box_3d.h
// .FILE        vsol_box_3d.cxx
//
// .SECTION Description
//  
//                     Note that the definition of width, depth and 
//                     height are such that the X-Y plane is considered
//                     as a Rectangle with a "width" and "height" 
//                     according to the usual definition. The figure 
//                     shows a right-handed coordinate system, but there
//                     is no commitment to that in the definitions
// \verbatim
//                       |<--width-->| 
//                 Z     O-----------O  ---
//                    | /           /|   ^
//                    |/           / |   |
//                    O-----------O  | depth
//                    |           |  |   |
//                    |  centroid |  |   v
//                    |  Y  o     |  O  ---
//                    | /         | /   /_____height
//                    |/          |/   /
//                    O-----------O  --- X
// \endverbatim


#include <vbl/vbl_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_bounding_box.h>

// \brief A bounding box for 3d spatial objects 

class vsol_box_3d : public vbl_ref_count , public vbl_timestamp
{
 public:
  
  vsol_box_3d();
  vsol_box_3d(vsol_box_3d const& b);
 
  ~vsol_box_3d();
  
  

  //: \brief accessors
  
  double get_min_x();
  double get_max_x();

  double get_min_y();
  double get_max_y();

  double get_min_z();
  double get_max_z();


  void set_min_x(const double& v);
  void set_max_x(const double& v);

  void set_min_y(const double& v);
  void set_max_y(const double& v);

  void set_min_z(const double& v);
  void set_max_z(const double& v);

  
 protected:
  
  vbl_bounding_box<double,3> _box;


};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vtol_topology_hierarchy_node.




