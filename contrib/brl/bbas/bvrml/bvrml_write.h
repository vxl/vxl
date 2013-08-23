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

class bvrml_color
{
 public:
  //: store the color scheme 'classic' to generate a heatmap
  static unsigned heatmap_classic_size;
  static unsigned char heatmap_classic[256][3];
};

class bvrml_write
{
 public:

  //: Write VRML 2.0 header
  static void write_vrml_header(vcl_ofstream& str);

  //: Write a vgl_line_segment_3d, specifying color and transparency
  template <class T>
  static void write_vrml_line_segment(vcl_ofstream& str, const vgl_line_segment_3d<T>& line,
                                      const float r = 0.0f, const float g=0.0f, const float b=1.0f,
                                      const float transparency=0.0f);

  //: Write a line by specifying starting point, direction magnitude and line color
  static void write_vrml_line(vcl_ofstream& str,vgl_point_3d<double> pt,vgl_vector_3d<double> dir,
                              const float length =1.0f,
                              const float r1=1.0f, const float g=1.0f, const float b=0.0f);

  //: Write a vgl_box_3d
  template <class T>
  static void write_vrml_box(vcl_ofstream& str, vgl_box_3d<T> const& box,
                             const float r = 1.0, const float g = 1.0,
                             const float b = 1.0,
                             const float transparency = 0.0);

  //: Write a vgl_box_3d as a  wireframe
  template <class T>
  static void write_vrml_wireframe_box(vcl_ofstream& str, vgl_box_3d<T> const& box,
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

  static void write_vrml_cylinder(vcl_ofstream& str,
                                  vgl_point_3d<double> const& pt,
                                  vgl_vector_3d<double> const &dir,
                                  float radius, float height,
                                  const float r, const float g, const float b,
                                  const unsigned side = 0);
};



template <class T>
void bvrml_write::write_vrml_box(vcl_ofstream& str, vgl_box_3d<T> const& box,
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
void bvrml_write::write_vrml_wireframe_box(vcl_ofstream& str, vgl_box_3d<T> const& box,
                                           const float r , const float g , const float b ,
                                           const float transparency )
{
  vcl_vector<vgl_point_3d<T> > vertices = box.vertices();

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
void bvrml_write::write_vrml_line_segment(vcl_ofstream& str, const vgl_line_segment_3d<T> &line, const float r, const float g, const float b,
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