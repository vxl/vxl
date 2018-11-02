// This is//projects/vxl/src/contrib/brl/bbas/bvrml/bvrml_write.h
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

#include <iostream>
#include <fstream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_line_segment_3d.h>
#include "bvrml_export.h"

class bvrml_color
{
 public:
  //: store the color scheme 'classic' to generate a heatmap
  static bvrml_EXPORT_DATA unsigned heatmap_classic_size;
  static bvrml_EXPORT_DATA unsigned char heatmap_classic[256][3];
};

class bvrml_write
{
 public:

  //: Write VRML 2.0 header
  static void write_vrml_header(std::ofstream& str);

  //: Write a vgl_line_segment_3d, specifying color and transparency
  template <class T>
  static void write_vrml_line_segment(std::ofstream& str, const vgl_line_segment_3d<T>& line,
                                      const float r = 0.0f, const float g=0.0f, const float b=1.0f,
                                      const float transparency=0.0f);

  //: Write a line by specifying starting point, direction magnitude and line color
  static void write_vrml_line(std::ofstream& str,vgl_point_3d<double> pt,vgl_vector_3d<double> dir,
                              const float length =1.0f,
                              const float r1=1.0f, const float g=1.0f, const float b=0.0f);

  //: Write a vgl_box_3d
  template <class T>
  static void write_vrml_box(std::ofstream& str, vgl_box_3d<T> const& box,
                             const float r = 1.0, const float g = 1.0,
                             const float b = 1.0,
                             const float transparency = 0.0);

  //: Write a vgl_box_3d as a  wireframe
  template <class T>
  static void write_vrml_wireframe_box(std::ofstream& str, vgl_box_3d<T> const& box,
                                       const float r = 1.0, const float g = 1.0,
                                       const float b = 1.0,
                                       const float transparency = 0.0);

  //: Write a vgl_sphere_3d
  static void write_vrml_sphere(std::ofstream& str,
                                vgl_sphere_3d<float> const& sphere,
                                const float r = 1.0, const float g =1.0,
                                const float b=1.0,
                                const float transparency = 0);

  static void write_vrml_disk(std::ofstream& str,
                              vgl_point_3d<double> const& pt,
                              vgl_vector_3d<double> const &dir,
                              float radius,
                              const float r, const float g, const float b);

  static void write_vrml_cylinder(std::ofstream& str,
                                  vgl_point_3d<double> const& pt,
                                  vgl_vector_3d<double> const &dir,
                                  float radius, float height,
                                  const float r, const float g, const float b,
                                  const unsigned side = 0);

  // From http://www.lighthouse3d.com/vrml/tutorial/index.shtml?view:
  // Orientation determines the direction at which the user is looking,
  // it specifies a rotation relative to the default orientation
  // which points along the Z axis in the negative direction.
  // default orientation is 0 0 1 0
  //
  // The default view will be set to the first viewpoint element encountered
  template <class T>
  static void write_viewpoint(std::ofstream& ofs,
                              const T cx, const T cy, const T cz,//position
                              const T ox, const T oy, const T oz, const T rad, //orientation
                              const std::string& description);


};


template <class T>
void bvrml_write::
write_viewpoint(std::ofstream& ofs,
                const T cx, const T cy, const T cz,
                const T ox, const T oy, const T oz, const T rad,
                const std::string& description)
{
  ofs << "Viewpoint {\n"
      << "  position     " << cx << ' ' << cy << ' ' << cz << '\n'
      << "  orientation  " << ox << ' ' << oy << ' ' << oz << ' ' << rad << '\n'
      << "  description  \"" << description << "\"\n"
      << "}\n";
}


template <class T>
void bvrml_write::write_vrml_box(std::ofstream& str, vgl_box_3d<T> const& box,
                                 const float r , const float g , const float b ,
                                 const float transparency )
{
  T x0 = box.centroid().x(), y0 = box.centroid().y(), z0 = box.centroid().z();
  T w = box.max_point().x() - box.min_point().x();
  T h = box.max_point().y() - box.min_point().y();
  T d = box.max_point().z() - box.min_point().z();

  str << "Transform {\n"
      << "translation " << x0 << ' ' << y0 << ' ' << z0 << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Box\n"
      <<   "{\n"
      << "  size " << w << ' ' << h << ' ' << d << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}

template <class T>
void bvrml_write::write_vrml_wireframe_box(std::ofstream& str, vgl_box_3d<T> const& box,
                                           const float r , const float g , const float b ,
                                           const float transparency )
{
  std::vector<vgl_point_3d<T> > vertices = box.vertices();

  str << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      emissiveColor " << r << ' ' << g << ' ' << b << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry IndexedLineSet\n"
      << " {\n"
      << "      coord Coordinate{\n"
      << "       point[\n";
  for (unsigned i = 0; i < vertices.size(); i++)
  {
    vgl_point_3d<T> pt = vertices[i];
    str << pt.x() << ' ' << pt.y() << ' ' << pt.z() << ", ";
  }
  str << "   ]\n      }"
      << "   coordIndex [\n"
      << "0, 1, 2, 3, 0, -1, 4, 5, 6, 7, 4, -1, 0, 4, -1, 1, 5, -1, 2, 6, -1, 3, 7,   ]\n }\n"
      << "}\n";
}

template <class T>
void bvrml_write::write_vrml_line_segment(std::ofstream& str, const vgl_line_segment_3d<T> &line, const float r, const float g, const float b,
                                          const float transparency)
{
  str << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      emissiveColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry IndexedLineSet\n"
      <<   "{\n"
      << "      coord Coordinate{\n"
      << "       point[\n"
      << line.point1().x() << ' ' << line.point1().y() << ' ' <<line.point1().z() << '\n'
      << line.point2().x() << ' ' << line.point2().y() << ' ' <<line.point2().z() << '\n'
      << "   ]\n\n }"
      << "   coordIndex [\n"
      << "0,1   ]\n  }\n}";
}
#endif
