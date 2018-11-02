// Example use of the vsol_point_* classes
//
// Author: Peter Tu sep 19 2000

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>

int main()
{
  // vsol is an abstract class so we need a concrete example

  std::cout << "2D example " << std::endl;

  vsol_point_2d_sptr p2d = new vsol_point_2d(5.0,10.0);
  std::cout << "X and Y" << std::endl;
  std::cout << p2d->x() << " " << p2d->y() << std::endl;

  vsol_point_2d_sptr p2d_1 = new vsol_point_2d(25.0,35.0);
  std::cout << "X and Y" << std::endl;
  std::cout << p2d_1->x() << " " << p2d_1->y() << std::endl;

  p2d_1->set_x(20.0);
  p2d_1->set_y(30.0);

  std::cout << "X and Y" << std::endl;
  std::cout << p2d_1->x() << " " << p2d_1->y() << std::endl;

  vsol_point_2d_sptr mid_2d = p2d_1->middle(*(p2d.ptr()));
  std::cout << "middle" << std::endl;
  std::cout << mid_2d->x() << " " << mid_2d->y() << std::endl;

  double dist=p2d_1->distance(p2d);
  std::cout << "Distance " << std::endl;
  std::cout << dist << std::endl;

  std::cout << "3D example " << std::endl;

  vsol_point_3d_sptr p3d = new vsol_point_3d(5.0,10.0,15.0);
  std::cout << "X and Y and Z" << std::endl;
  std::cout << p3d->x() << " " << p3d->y() << " " << p3d->z() << std::endl;

  vsol_point_3d_sptr p3d_1 = new vsol_point_3d(25.0,35.0,40.0);
  std::cout << "X and Y Z " << std::endl;
  std::cout << p3d_1->x() << " " << p3d_1->y() << " " << p3d_1->z() <<  std::endl;

  p3d_1->set_x(20.0);
  p3d_1->set_y(30.0);
  p3d_1->set_z(35.0);


  std::cout << "X and Y Z" << std::endl;
  std::cout << p3d_1->x() << " " << p3d_1->y() << " " << p3d_1->z() <<  std::endl;

  vsol_point_3d_sptr mid_3d = p3d_1->middle(*(p3d.ptr()));
  std::cout << "middle" << std::endl;
  std::cout << mid_3d->x() << " " << mid_3d->y() << " " << mid_3d->z() << std::endl;

  dist=p3d_1->distance(p3d);
  std::cout << "Distance " << std::endl;
  std::cout << dist << std::endl;

  return 0;
}
