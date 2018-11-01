// This is oxl/osl/osl_edge.h
#ifndef osl_edge_h_
#define osl_edge_h_
//:
// \file
// \author fsm

#include <osl/osl_edgel_chain.h>
#include <osl/osl_vertex.h>

class osl_edge : public osl_topology_base, public osl_edgel_chain
{
 public:
  // end-vertices (ref-counted)
  osl_vertex *v1, *v2;

  osl_edge(int n, osl_vertex *v1, osl_vertex *v2);
  osl_edge(osl_edgel_chain const &, osl_vertex *v1, osl_vertex *v2);
  ~osl_edge() override;

  // set new end-vertex pointers
  osl_vertex *GetV1() const { return v1; }
  osl_vertex *GetV2() const { return v2; }

  // replace end-vertices pointers
  void set_v1(osl_vertex *newv);
  void set_v2(osl_vertex *newv);

  // return coordinates of end-vertices
  float GetStartX() const;
  float GetStartY() const;

  float GetEndX() const;
  float GetEndY() const;

  // set coordinates of end-vertices
  void SetStartX(float v);
  void SetStartY(float v);
  void SetStart(float x, float y);

  void SetEndX(float v);
  void SetEndY(float v);
  void SetEnd(float x, float y);
};

#endif // osl_edge_h_
