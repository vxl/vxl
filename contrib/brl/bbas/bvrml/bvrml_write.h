// This is//projects/vxl/src/contrib/brl/bseg/bvxm/grid/io/bvrml_write.h
#ifndef bvrml_write_h
#define bvrml_write_h

//:
// \file
// \brief A class with vrml utilities
// \author Isabel Restrepo mir@lems.brown.edu
// \date  Dec 8, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_line_segment_3d.h>


class bvrml_write
{
public:
 
  //: Write VRML 2.0 header
  static void write_vrml_header(vcl_ofstream& str);
  
  //: Write a vgl_line_segment_3d, specifing color and transparancy
  static void write_vrml_line_segment(vcl_ofstream& str, const vgl_line_segment_3d<int>& line,
                                      const float r = 1.0f, const float g=1.0f, const float b=1.0f,
                                      const float transparency=0.0f);
  
  //: Write a line by specifin starting point, direction magnitude and line color
  static void write_vrml_line(vcl_ofstream& str,vgl_point_3d<double> pt,vgl_vector_3d<double> dir,
                              const float length =1.0f,
                              const float r1=1.0f, const float g=1.0f, const float b=1.0f);
 
  //: Write a vgl_box_3d
  static void write_vrml_box(vcl_ofstream& str, vgl_box_3d<int> const& box,
                             const float r = 1.0, const float g = 1.0,
                             const float b = 1.0,
                             const float transparency = 0.0);
  
  //: Write a vgl_sphere_3d
  static void write_vrml_sphere(vcl_ofstream& str,
                                vgl_sphere_3d<float> const& sphere,
                                const float r = 1.0, const float g =1.0,
                                const float b=1.0,
                                const float transparency = 0);
  
  static void write_vrml_disk(vcl_ofstream& str, 
                               vgl_point_3d<double> const& pt,
                               vgl_vector_3d<double> const &dir,
                               float radius,
                               const float r, const float g, const float b);
 

};
#endif
