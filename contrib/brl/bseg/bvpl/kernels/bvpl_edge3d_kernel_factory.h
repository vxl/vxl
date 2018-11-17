// This is brl/bseg/bvpl/kernels/bvpl_edge3d_kernel_factory.h
#ifndef bvpl_edge3d_kernel_h_
#define bvpl_edge3d_kernel_h_
//:
// \file
// \brief A class to construct a 3d edge kernel
// \author Vishal Jain
// \date  August, 2009
//
// \verbatim
//  Modifications
//   9/30/09 Isabel Restrepo: Changed coordinate system to match that of kernels.
//           Within a subgrid the centroid is placed at the origin. The maximum
//           and minimum position are as shown below. The methods in this class must take care
//           of retrieving the appropriate index of the big grid (where z is inverted).
// \endverbatim
// \verbatim
//
//                                  (max_x, max_y, max_z)
//                       O-----------O
//                      /___________/|
//                     /           /||---->"-" Minus region
//                    O-----------O ||
//                    |           | ||
//                    |  +    +   |---->"+" Plus region (including Z=0 plane)   |
//                    |  +    +   | |O
//     Z              |  +    +   | /
//     |              |           |/
//     |              O-----------O
//     |       (min_x,min_y,min_z)
//     O-----Y
//    /
//   /
//  X - Canonical Axis
//
// \endverbatim

#include <iostream>
#include "bvpl_kernel_factory.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bxml/bxml_document.h>

class bvpl_edge3d_kernel_factory : public bvpl_kernel_factory
{
 public:
  //: Default Constructor
  bvpl_edge3d_kernel_factory();

  //: Constructs constructor from min and max positions on each axis
  bvpl_edge3d_kernel_factory(int min_x, int max_x, int min_y, int max_y, int min_z, int max_z, double voxel_length = 1.0);

  //: Copy Constructor
  bvpl_edge3d_kernel_factory(bvpl_edge3d_kernel_factory const& other)
  : bvpl_kernel_factory(other),
    min_x_(other.min_x_),max_x_(other.max_x_),
    min_y_(other.min_y_),max_y_(other.max_y_),
    min_z_(other.min_z_),max_z_(other.max_z_) {}

  ~bvpl_edge3d_kernel_factory() override = default;

  static std::string name() {return "edge3d"; }

  bvpl_kernel_factory_sptr self() override{return new bvpl_edge3d_kernel_factory(*this);}

  //: Return an xml element
  bxml_data_sptr xml_element() override;

  //: Parse an xml element
  static bvpl_kernel_sptr parse_xml_element(const bxml_data_sptr& d);

 private:

  //:Creates a 2d edge kernel
  void create_canonical() override;

  int min_x_;
  int max_x_;
  int min_y_;
  int max_y_;
  int min_z_;
  int max_z_;

  static const unsigned max_size_ = 71;
};

#endif
