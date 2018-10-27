// This is gel/vtol/tests/vtol_test_timing.cxx
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_face_2d.h>

static void vtol_test_timing()
{
  std::cout << "testing topology timing\n";

  vul_timer t;
  // make a network of s^2 vertices and edges on an s by s grid
  constexpr int s = 100;
  // make the vertices
  vertex_list verts(s*s);
  t.mark();
  for (int y = 0, off = 0; y<s; ++y)
  {
    for (int x = 0; x<s; ++x, ++off)
    {
      verts[off] = new vtol_vertex_2d(x,y);
    }
  }
  std::cout << "Time to construct " << s*s << " vertices " << t.real()
           << " msecs\n" << std::flush;

  // make the edges
  edge_list edges(2*s*s);
  int nedges = 0;
  t.mark();
  for (int y = 0, yoff = 1; y<s; ++y, yoff+=s)
  {
    for (int x = 1, off = yoff; x<s; ++x, ++off)
    {
      edges[nedges++] = new vtol_edge_2d(verts[off-1], verts[off]);
    }
  }
  for (int x = 0; x<s; ++x)
  {
    for (int y = 1, off = x+s; y<s; ++y, off+=s)
    {
      edges[nedges++] = new vtol_edge_2d(verts[off-s], verts[off]);
    }
  }
  std::cout << "Time to construct " << nedges << " edges " << t.real()
           << " msecs\n" << std::flush;
  TEST("number of edges", nedges, 2*(s-1)*s);

  // count the outgoing edges of all vertices
  t.mark();
  int n_vedges = 0;
  for (auto & vert : verts)
  {
    edge_list vedges; vert->edges(vedges);
    n_vedges += vedges.size();
  }
  std::cout << "Time to count edges per vertex " << t.real()
           << " msecs\n" << std::flush;
  TEST("number of edges per vertex", n_vedges, 4*(s-1)*s);

  // make the faces
  face_list faces(s*s);
  int nfaces = 0;
  t.mark();
  for (int y = 1; y<s; ++y)
  {
    int yoff = s*y;
    for (int x = 1; x<s; ++x)
    {
      int off = x+yoff;
      vertex_list l(4);
      l[0] = verts[off];
      l[1] = verts[off-s];
      l[2] = verts[off-s-1];
      l[3] = verts[off-1];
      faces[nfaces++]= new vtol_face_2d(l);
    }
  }
  std::cout << "Time to construct " << nfaces << " faces " << t.real()
           << " msecs\n" << std::flush;
  TEST("number of faces", nfaces, (s-1)*(s-1));

  // count the faces adjacent to all vertices
  t.mark();
  int n_vfaces = 0;
  for (auto & vert : verts)
  {
    face_list vfaces; vert->faces(vfaces);
    n_vfaces += vfaces.size();
  }
  std::cout << "Time to count faces per vertex " << t.real() << " msecs\n";
  TEST("number of faces per vertex", n_vfaces, 4*(s-1)*(s-1));
}

TESTMAIN(vtol_test_timing);
