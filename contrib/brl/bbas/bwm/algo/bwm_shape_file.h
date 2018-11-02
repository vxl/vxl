#ifndef bwm_shape_file_h_
#define bwm_shape_file_h_
//:
// \file
#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <shapefil.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_box_3d.h>

#if 0 // commented out
typedef enum { NULL = 0, POINT = 1, ARC = 3, POLYGON = 5, MULTIPOINT = 8, POINT3D = 11, ARC3D = 13, POLYGON3D = 15, MULTIPOINT3D = 18, POINTM = 21, ARCM = 23, POLYGONM = 25, MULTIPOINTM = 28, MULTIPATCHM = 31} BWM_SHAPE_TYPE;
typedef enum { NULL, POINT, ARC=3, POLYGON, MULTIPOINT, POINT3D, ARC3D, POLYGON3D, MULTIPOINT3D, POINTM, ARCM, POLYGONM, MULTIPOINTM, MULTIPATCHM} BWM_SHAPE_TYPE;
#endif // 0

class bwm_shape_file
{
 public :
  bwm_shape_file(){}
  ~bwm_shape_file(){}

  bool load(std::string filename);

  //: only one shape type is included, no MIX types
  int shape_type() const{ return nShapeType_; }

  //: number of entities in the file
  int num_items() const { return nEntities_; }

  //: returns the bounding box of the items included in the shape file
  vsol_box_3d bounding_box() const { return bb_; }

  std::vector<std::vector<vsol_point_3d_sptr> > vertices() {return vertices_; }

  void print();

 private:
  SHPHandle handle_;
  int nEntities_;
  int nShapeType_;
  double padfMinBound[4];
  double padfMaxBound[4];
  vsol_box_3d bb_;
  std::vector<std::vector<vsol_point_3d_sptr> > vertices_;
};

#endif
