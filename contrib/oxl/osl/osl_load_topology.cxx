// This is oxl/osl/osl_load_topology.cxx
//:
// \file
// \author fsm

#include <cstring>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include "osl_load_topology.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void osl_load_topology(char const *f, std::list<osl_edge*> &e, std::list<osl_vertex*> &v)
{
  std::ifstream file(f);
  osl_load_topology(file, e, v);
}

#define streamok \
{ if (f.bad()) { std::cerr << __FILE__ ":" << __LINE__ << " stream bad at this point\n"; return; } }

void osl_load_topology(std::istream &f, std::list<osl_edge*> &es, std::list<osl_vertex*> &vs)
{
  es.clear();
  vs.clear();

  char buf[1024]; // line buffer
  char tmp[1024];

  // check version string
  f >> std::ws;
  f.getline(buf, sizeof(buf));
  if (std::strcmp("osl_save_topology 1.0", buf) != 0) {
    std::cerr << __FILE__ ": version string mismatch\n";
    return;
  }
  streamok;

  // read number of vertices :
  f >> std::ws;
  f.getline(buf, sizeof(buf));
  int numverts = -1;
  if (std::sscanf(buf, "%d%[ ]vertices", &numverts, tmp) != 2) {
    std::cerr << __FILE__ ": error reading number of vertices\n";
    return;
  }
  assert(numverts >= 0);
  streamok;
  // read vertices :
  std::cerr << "reading " << numverts << " vertices...\n";
  std::vector<osl_vertex*> vert(numverts+1, (osl_vertex*)nullptr);
  for (int i=0; i<numverts; ++i) {
    unsigned int stashid;
    int id;
    float x, y;
    f >> std::ws >> std::hex >> stashid >> std::dec >> id >> x >> y;
    assert(stashid<vert.size() && !vert[stashid]);
    vert[stashid] = new osl_vertex(x, y, id);

    vs.push_front(vert[stashid]);
  }
  streamok;

  // read number of edges :
  f >> std::ws;
  f.getline(buf, sizeof(buf));
  int numedges = -1;
  if (std::sscanf(buf, "%d%[ ]edges", &numedges, tmp) != 2) {
    std::cerr << __FILE__ ": error reading number of edges\n";
    return;
  }
  assert(numedges >= 0);
  streamok;
  // read edges :
  std::cerr << "reading " << numedges << " edges...\n";
  for (int i=0; i<numedges; ++i) {
    unsigned int stashid1 = vert.size(), stashid2 = vert.size();
    f >> std::ws >> std::hex >> stashid1 >> stashid2 >> std::dec;
    assert(stashid1<vert.size() && vert[stashid1]);
    assert(stashid2<vert.size() && vert[stashid2]);

    int id; // edge id
    f >> std::ws >> id;

    auto *e = new osl_edge(2/*dummy*/, vert[stashid1], vert[stashid2]);
    e->SetId(id);

    e->read_ascii(f);

    es.push_front(e);
  }
  streamok;

  // done
}
