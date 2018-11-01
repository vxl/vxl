// This is oxl/osl/osl_vertex.h
#ifndef osl_vertex_h_
#define osl_vertex_h_
//:
// \file
// \author fsm

#include <osl/osl_topology.h>

class osl_vertex : public osl_topology_base
{
 public:
  float x, y;
  osl_vertex(float x_, float y_, int id_ = 0);
  ~osl_vertex() override;
  float GetX() const { return x; }
  float GetY() const { return y; }
};

bool operator==(osl_vertex const &a, osl_vertex const &b);

// urgh
typedef osl_vertex osl_Vertex;

#endif // osl_vertex_h_
