// This is oxl/vrml/vrml_out.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include <vcl_functional.h> //vxl_filter
#include <vcl_utility.h>    //vxl_filter
#include "vrml_out.h"

#include <vcl_fstream.h>
#include <vcl_map.h>
#include <vul/vul_printf.h>

// Default ctor
vrml_out::vrml_out()
{
  s_ = 0;
  own_ostream_ = false;
}

//: Point vrml output to this stream
vrml_out::vrml_out(vcl_ostream& s)
{
  s_ = &s;
  own_ostream_ = false;
}

//: Open filename for writing, write prologue, and on closure write epilogue
vrml_out::vrml_out(char const* filename)
{
  s_ = new vcl_ofstream(filename);
  own_ostream_ = true;
  if (!s_ || !(*s_)) {
    vcl_cerr << "Cannot open " << filename << " for writing\n";
    delete s_; s_ = 0; own_ostream_ = false;
  }
  else prologue();
}

//: Destructor.  If we own the vcl_ostream, write the epilogue
vrml_out::~vrml_out()
{
  if (own_ostream_) {
    epilogue();
    delete s_;
  }
}

#define SETUP if (s_ == 0) { vcl_cerr << "vrml_out -- vcl_ostream not set!\n"; return; } vcl_ostream& f = *s_

//: Write vrml_out header and an opening "Separator {"
void vrml_out::prologue()
{
  SETUP;
  f << "#VRML V1.0 ascii\n\n";
  f << "#TargetJr vrml_out output\n";
  f << "Separator {\n";
  f << "ShapeHints {\n";
  f << "\t vertexOrdering  CLOCKWISE\n";
  f << "\t shapeType       UNKNOWN_SHAPE_TYPE\n";
  //  f << "\t faceType        UNKNOWN_FACE_TYPE\n"; // Not necessarily convex
  // vrweb barfs on previous line if faces are triangles...
  f << "}\n";
}

void vrml_out::comment(char const* msg)
{
  SETUP;
  f << "# " << msg << vcl_endl;
}

void vrml_out::verbatim(char const* msg)
{
  SETUP;
  f << msg << vcl_endl;
}

//: Write closing "}"
void vrml_out::epilogue()
{
  SETUP;
  f << "}\n";
  f << "# End TargetJr vrml_out output\n";
}

#if 0
void vrml_out::write_vertices(vcl_list<Vertex*>& points)
{
  begin_pointset();
  for (points.reset(); points.next(); )
    point3d(points.value());
  end_pointset();
  display_pointset();
}
#endif

void vrml_out::display_pointset ()
{
  SETUP;
  f << "PointSet {}\n";
}

// -----------------------------------------------------------------------------

#if 0
struct VRML_IO_VertexRememberer
{
  vrml_out* vrml_;
  typedef vcl_map<void*, int, vcl_less<void*> > Map;
  Map vertex_ids;
  int current_vertex_id;

  VRML_IO_VertexRememberer(vrml_out* vrml, int /*approx_numverts*/):
    vrml_(vrml),
    current_vertex_id(0)
  {}
  bool send_vertex(void*);
  int vertex_id(void*);
};

//: Return true if it was a new vertex.
bool VRML_IO_VertexRememberer::send_vertex(void* v)
{
  Map::iterator p = vertex_ids.find(v);
  if (p != vertex_ids.end()) {
    // Has already been sent, ignore.
    return false;
  }

  // Not there already, write the vertex and give it an index
  vertex_ids.insert(Map::value_type(v, current_vertex_id++));
  vrml_->point3d(v);

  return true;
}

int VRML_IO_VertexRememberer::vertex_id(Vertex* v)
{
  Map::iterator p = vertex_ids.find(v);
  if (p != vertex_ids.end())
    return (*p).second;

  vcl_cerr << "VRML_IO_VertexRememberer::vertex_ids() WARNING! "
           << "This can't happen -- vertex " << v << " has no id.  Fnarrr.\n";

  return -1;
}

// -----------------------------------------------------------------------------

void vrml_out::write_edges(vcl_list<Edge*>& edges)
{
  // Start sending vertices
  begin_pointset();
  VRML_IO_VertexRememberer vertexer(this, edges.length() * 2);
  for (edges.reset(); edges.next(); ) {
    vertexer.send_vertex(edges.value()->GetV1());
    vertexer.send_vertex(edges.value()->GetV2());
  }
  end_pointset();

  begin_lineset();
  for (edges.reset(); edges.next(); ) {
    int i0 = vertexer.vertex_id(edges.value()->GetV1());
    int i1 = vertexer.vertex_id(edges.value()->GetV2());
    line(i0, i1);
  }
  end_lineset();
}

void vrml_out::write_faces(vcl_list<Face*>& triangles)
{
  VRML_IO_VertexRememberer vertexer(this, triangles.length() * 3/2);

  // Send vertices
  begin_pointset();
  for (triangles.reset(); triangles.next(); ) {
    Face* face = triangles.value();
    vcl_list<Vertex*>* vertices = face->Vertices();
    for (vertices->reset(); vertices->next();)
      vertexer.send_vertex(vertices->value());
  }
  end_pointset();

  // Now send triangles
  begin_faceset();
  for (triangles.reset(); triangles.next(); ) {
    Face* face = triangles.value();
    vcl_list<Vertex*>* vertices = face->Vertices();
    face_open();
    for (vertices->reset(); vertices->next();)
      face_index(vertexer.vertex_id(vertices->value()));
    face_close();
  }
  end_faceset();
}

void vrml_out::write_faces_textured(
        vcl_list<Face*>& triangles,
        char const* texfile,
        vrml_out_vertex_to_texture const& v2t
       )
{
  VRML_IO_VertexRememberer vertexer(this, triangles.length() * 3/2);

  vcl_ofstream phil3d("/tmp/pcptex.3d");
  vcl_ofstream phil2d("/tmp/pcptex.2d");

  begin_pointset();
  for (triangles.reset(); triangles.next(); ) {
    Face* face = triangles.value();
    vcl_list<Vertex*>* vertices = face->Vertices();
    for (vertices->reset(); vertices->next();) {
      Vertex *v = vertices->value();
      if (vertexer.send_vertex(v)) {
        // Save pcp3d
        phil3d << v->GetX() << " " << v->GetY() << " " << v->GetZ() << vcl_endl;
        // Save pcp texture coord
        int xsize = v2t.image_xsize;
        int ysize = v2t.image_ysize;
        double ix, iy;
        v2t.get_texture_coords(v, &ix, &iy);
        ix = ix / xsize;
        iy = 1.0 - iy / ysize;
        phil2d << ix << " " << iy << vcl_endl;
      }
    }
  }
  end_pointset();

  // Send texture coords
  int xsize = v2t.image_xsize;
  int ysize = v2t.image_ysize;

  begin_texture(texfile);
  int last_id = -1;
  for (triangles.reset(); triangles.next(); ) {
    Face* face = triangles.value();
    vcl_list<Vertex*>* vertices = face->Vertices();
    for (vertices->reset(); vertices->next();) {
      Vertex *v = vertices->value();
      int id = vertexer.vertex_id(v);
      if (id > last_id) {
        if (last_id + 1 != id)
          vcl_cerr << "vrml_out::write_faces_textured() -- texture buggered\n";
        ++last_id;

        double ix, iy;
        v2t.get_texture_coords(v, &ix, &iy);

        texture2_image_coords(ix, iy, xsize, ysize);
      }
    }
  }
  end_texture();

  // Now send triangles
  begin_faceset();
  for (triangles.reset(); triangles.next(); ) {
    Face* face = triangles.value();
    vcl_list<Vertex*>* vertices = face->Vertices();
    face_open();
    for (vertices->reset(); vertices->next();)
      face_index(vertexer.vertex_id(vertices->value()));
    face_close();
  }
  end_faceset();
}

struct Hack_VertexToTexture : public vrml_out_vertex_to_texture
{
  Hack_VertexToTexture(
        int xsize,
        int ysize
       ) :vrml_out_vertex_to_texture(xsize, ysize)
    {}

  void get_texture_coords(
        const Vertex* vertex,
        double* u, double* v) const
    {
      // Ugh - assume P matrix is [I 0];
      double tu = vertex->GetX() / vertex->GetZ();
      double tv = vertex->GetY() / vertex->GetZ();

      // Ugh ugh - assume C = [f 0 u; 0 f v; 0 0 1];
      double fhak = (image_xsize + image_ysize) * 0.5; // FIXME
      *u = tu * fhak + 0.5 * image_xsize; // 0..xsize
      *v = tv * fhak + 0.5 * image_ysize; // 0..ysize
    }
};

void vrml_out::write_faces_textured(
        vcl_list<Face*>& triangles,
        char const* imagefilename,
        int xsize,
        int ysize
       )
{
  vcl_cerr << "vrml_out::write_faces_textured() -- hacking image-world transform\n";
  Hack_VertexToTexture hack(xsize, ysize);
  write_faces_textured(triangles, imagefilename, hack);
}

class VTT : public vrml_out_vertex_to_texture
{
 public:
  VTT(int xsize, int ysize, CoolMatrix<double> const& m)
        : vrml_out_vertex_to_texture(xsize, ysize), Pmatrix(m) {}

  void get_texture_coords(
        const Vertex* vertex,
        double* u,
        double* v) const
      {
        double data[] = { vertex->GetX(), vertex->GetY(), vertex->GetZ(), 1.0};
        CoolVector<double> p3d(data,4);
        CoolVector<double> p2d = Pmatrix*p3d;
        *u = p2d.x() / p2d.z(); // ==> texture coordinate *u / image_xsize;
        *v = p2d.y() / p2d.z(); // ==> texture coordinate 1 - *v / image_ysize;
      }

  CoolMatrix<double> Pmatrix;
};

void vrml_out::write_faces_textured(
        vcl_list<Face*>& triangles,
        char const* imagefilename,
        int xsize,
        int ysize,
        CoolMatrix<double> const& Pmatrix
       )
{
  VTT vtt(xsize, ysize, Pmatrix);
  write_faces_textured(triangles, imagefilename, vtt);
}

void vrml_out::write_block(Block* block)
{
  write_faces(*block->Faces());
}

void vrml_out::write_topology(TopologyObject* topobj)
{
  SETUP;
  Block* block = topobj->CastToBlock();
  if (block) {
    begin_separator();
    write_faces(*block->Faces());
    end_separator();
    return;
  }

  Face* face = topobj->CastToFace();
  if (face) {
    begin_separator();
    vcl_list<Face*> faces; faces.push(face);
    write_faces(faces);
    end_separator();
    return;
  }

  Edge* edge = topobj->CastToEdge();
  if (edge) {
    begin_separator();
    vcl_list<Edge*> edges; edges.push(edge);
    write_edges(edges);
    end_separator();
    return;
  }

  Vertex* vertex = topobj->CastToVertex();
  if (vertex) {
    begin_separator();
    vcl_list<Vertex*> vertices; vertices.push(vertex);
    write_vertices(vertices);
    end_separator();
    return;
  }

  vcl_cerr << "VRML: not handling " << topobj << vcl_endl;
  f << "# vrml_out: Couldn't handle "
    << TopologyObject::TopoNames[topobj->GetTopologyType()] << vcl_endl;
}

void vrml_out::write_topology(vcl_list<TopologyObject*>& topobjs)
{
  for (topobjs.reset(); topobjs.next(); )
    write_topology(topobjs.value());
}

#endif // 0

// == IMPLEMENTORS ==
void vrml_out::begin_separator()
{
  SETUP;
  f << "Separator {\n";
}

void vrml_out::end_separator()
{
  SETUP;
  f << "}\n\n";
}

void vrml_out::begin_pointset()
{
  SETUP;
  f << "  Coordinate3 { point [\n";
}

void vrml_out::point3d(vnl_vector<double> const& p)
{
  point3d(p[0], p[1], p[2]);
}


void vrml_out::point3d(double x, double y, double z)
{
  SETUP;
  //f << "\t" << x << " " << y << " " << z << ",\n";
  vul_printf(f, "\t %10.4f %10.4f %10.4f,\n", x, y, z);
}

void vrml_out::point3d(double x, double y, double z, const char *format)
{
  SETUP;
  vul_printf(f, "\t ");
  vul_printf(f, format, x);
  vul_printf(f, " ");
  vul_printf(f, format, y);
  vul_printf(f, " ");
  vul_printf(f, format, z);
  vul_printf(f, ",\n");
}

void vrml_out::end_pointset()
{
  SETUP;
  f << "   ]}\n";
}

void vrml_out::begin_texture(char const* texture_filename)
{
  SETUP;
  f << "  Texture2 { filename \"" << texture_filename << "\" }\n";
  f << "  TextureCoordinate2 { point [\n";
}

void vrml_out::texture2(double u, double v)
{
  SETUP;
  f << "\t" << u << " " << v << ",\n";
}

void vrml_out::texture2_image_coords(double u, double v, int w, int h)
{
  texture2(u / w, 1.0 - v / h);
}

void vrml_out::end_texture()
{
  SETUP;
  f << "   ]}\n";
}

void vrml_out::begin_lineset()
{
  SETUP;
  f << "  IndexedLineSet { coordIndex [\n";
}

void vrml_out::line(int i0, int i1)
{
  SETUP;
  f << "\t" << i0 << ", " << i1 << ",-1,\n";
}

void vrml_out::end_lineset()
{
  SETUP;
  f << "  ]}\n";
}

void vrml_out::begin_faceset()
{
  SETUP;
  f << "  IndexedFaceSet { coordIndex [\n";
}

void vrml_out::face_open()
{
  SETUP;
  f << "\t";
}

void vrml_out::face_index(int i)
{
  SETUP;
  f << i << ", ";
}

void vrml_out::face_close()
{
  SETUP;
  f << "-1,\n";
}

void vrml_out::triangle(int i1, int i2, int i3)
{
  SETUP;
  f << "\t" << i1 << ", " << i2 << ", " << i3 << ",-1,\n";
}

void vrml_out::quad(int i1, int i2, int i3, int i4)
{
  SETUP;
  f << "\t" << i1 << ", " << i2 << ", " << i3 << ", " << i4 << ",-1,\n";
}

void vrml_out::face(const int* base, int n)
{
  SETUP;
  f << "\t";
  for (int i = 0; i < n; ++i)
    f << base[i] << ", ";
  f << "-1,\n";
}

void vrml_out::end_faceset()
{
  SETUP;
  f << "  ]}\n";
}
