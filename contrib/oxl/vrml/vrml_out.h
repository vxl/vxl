// This is oxl/vrml/vrml_out.h
#ifndef vrml_io_h_
#define vrml_io_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author awf@robots.ox.ac.uk
// \date   19 Mar 00

#include <vcl_iostream.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>

class vrml_out_vertex_to_texture;

//: A class to write to a VRML 1.0 file.
class vrml_out
{
 public:
  // Constructors/Destructors--------------------------------------------------

  vrml_out();
  vrml_out(vcl_ostream& s);
  vrml_out(char const* filename);
  ~vrml_out();

  // Operations----------------------------------------------------------------
  void open(vcl_ostream& s);
  void close();

  void prologue();
  void write_points(vcl_vector<double> const& x, vcl_vector<double> const& y, vcl_vector<double> const& z);
  void display_pointset();
#if 0 // NYI - TODO
  void write_vertices(vcl_vector<vgl_point_3d<double> > const&);
  void write_points(vcl_vector<vgl_point_3d<double> > const& l) { return write_vertices(l); }
  void write_edges(vcl_vector<vgl_line_segment_3d<double> > const&);
  void write_faces(vcl_list<vgl_polygon<float> > const& triangles);
  void write_faces_textured(vcl_list<v<float> gl_polygon> const& triangles, char const* imagefilename, int xsize, int ysize);
  void write_faces_textured(vcl_list<v<float> gl_polygon> const& triangles, char const* imagefilename, int xsize, int ysize,
                            vnl_matrix<double> const& Pmatrix);
  void write_faces_textured(vcl_list<v<float> gl_polygon> const& triangles, char const* imagefilename, int xsize, int ysize,
                            vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > >&);
  void write_faces_textured(vcl_list<v<float> gl_polygon> const& triangles, char const* imagefilename,
                            vrml_out_vertex_to_texture const& textures);
  void write_block(Block* topology);
  void write_topology(TopologyObject* topology);
  void write_topology(vcl_list<TopologyObject*>& topology);
#endif
  void comment(char const* msg);
  void verbatim(char const* msg);
  void epilogue();

  // Helpers-------------------------------------------------------------------
  void begin_separator();
  void end_separator();

  void begin_pointset();
  void point3d(double, double, double);
  void point3d(double, double, double, char const*);
#if 0 // NYI - TODO
  void point3d(vgl_point_3d<double> const& p) { point3d(p.x(), p.y(), p.z()); }
  void point3d(vnl_vector_fixed<double,3> const& p) { point3d(p[0], p[1], p[2]); }
#endif // 0
  void end_pointset();

  void begin_texture(char const* texture_filename);
  void texture2(double, double);
  void texture2_image_coords(double, double, int image_xsize, int image_ysize);
  void end_texture();

  void begin_lineset();
  void line(int i0, int i1);
  void end_lineset();

  void begin_faceset();
  void triangle(int, int, int);
  void quad(int, int, int, int);
  void face(const int* base, int n);
  void face_open();
  void face_index(int);
  void face_close();
  void end_faceset();

 protected:
  // Data Members--------------------------------------------------------------
  vcl_ostream* s_;
  bool own_ostream_;

 private:
  // private as don't want 2 people on same vcl_ostream
  vrml_out(const vrml_out& that);
  vrml_out& operator=(const vrml_out& that);
};

class vrml_out_vertex_to_texture
{
 public:
  // Store image size for conversion from image to texture coordinates.
  int image_xsize;
  int image_ysize;

 public:
  vrml_out_vertex_to_texture(int xsize, int ysize): image_xsize(xsize), image_ysize(ysize) {}
  virtual ~vrml_out_vertex_to_texture() {}

  virtual void get_texture_coords(const void* vertex, double* u, double* v) const = 0;
};

#endif // vrml_io_h_
