#ifndef vsl_edge_h_
#define vsl_edge_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vsl_edge
// .INCLUDE vsl/vsl_edge.h
// .FILE vsl_edge.cxx
// \author fsm@robots.ox.ac.uk

#include <vsl/vsl_edgel_chain.h>
#include <vsl/vsl_vertex.h>

class vsl_edge : public vsl_topology_base, public vsl_edgel_chain {
public:
  // end-vertices (ref-counted)
  vsl_vertex *v1, *v2;

  vsl_edge(int n, vsl_vertex *v1, vsl_vertex *v2);
  vsl_edge(vsl_edgel_chain const &, vsl_vertex *v1, vsl_vertex *v2);
  ~vsl_edge();

  // set new end-vertex pointers
  vsl_vertex *GetV1() const { return v1; }
  vsl_vertex *GetV2() const { return v2; }

  // replace end-vertices pointers
  void set_v1(vsl_vertex *newv);
  void set_v2(vsl_vertex *newv);

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

// urgh
typedef vsl_edge vsl_Edge;

#endif
