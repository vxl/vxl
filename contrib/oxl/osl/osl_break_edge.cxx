// This is oxl/osl/osl_break_edge.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "osl_break_edge.h"

#include <vcl_cassert.h>
#include <osl/osl_edge.h>
#include <osl/osl_ortho_regress.h>

void osl_break_edge(osl_edge const *in,
                    vcl_vector<unsigned> const &where,
                    vcl_list<osl_edge*> *broken)
{
  assert(in!=0);
  float const *x = in->GetX();
  float const *y = in->GetY();
  unsigned n = in->size();

  assert(broken!=0);
  assert(!where.empty());
  assert(where.front() == 0);
  assert(where.back() == n-1);

  // make array of vertices, one for each break.
  vcl_vector<osl_vertex*> verts;
  verts.push_back(in->GetV1()); // maintain topology.
  for (unsigned int i=1; i+1<where.size(); ++i)
    verts.push_back(new osl_vertex(x[where[i]], y[where[i]]));
  verts.push_back(in->GetV2()); // maintain topology.

  // make new edges and push them onto the given list.
  for (unsigned int i=0; i+1<where.size(); ++i) {
    osl_edge *fragment = new osl_edge(where[i+1]-where[i] + 1, verts[i], verts[i+1]);
    for (unsigned int j=0; j<fragment->size(); ++j) {
      fragment->SetX(x[j + where[i]], j);
      fragment->SetY(y[j + where[i]], j);
    }
    broken->push_back(fragment);
  }
}

void osl_break_edge(osl_edge const *in,
                    vcl_list<osl_edge*> *out_ptr,
                    double threshold,
                    unsigned nbhd_size)
{
  assert(in!=0);

  float const *x = in->GetX();
  float const *y = in->GetY();
  unsigned n = in->size();

  osl_ortho_regress fitter;

  // decide where to break the edge.
  vcl_vector<unsigned> breaks;

  breaks.push_back(0);   // first edgel
  for (unsigned int pos=nbhd_size; pos+nbhd_size<n; ++pos) {

    fitter.reset(); // we could make this more efficient.
    fitter.add_points(&x[pos-nbhd_size], &y[pos-nbhd_size], 2*nbhd_size+1);

    double a, b, c;
    fitter.fit(&a, &b, &c);

    if (fitter.rms_cost(a, b, c) > threshold)
      breaks.push_back(pos);
  }
  breaks.push_back(n-1); // last edgel

  osl_break_edge(in, breaks, out_ptr);
}

//--------------------------------------------------------------------------------

// defunct
#if 0
  breaks.push_back(0);   // first edgel
  while (true) {
    int pos = breaks.back();
    if (pos+1 >= n)
      break; // reached the end - done.

    // else, there are at least two edgels available, pos and pos+1 :

    fitter.reset();
    fitter.add_point(x[pos], y[pos]); ++pos;
    fitter.add_point(x[pos], y[pos]); ++pos;
    for (; pos<n; ++pos) {
      fitter.add_point(x[pos], y[pos]);
      double a, b, c;
      fitter.fit(&a, &b, &c);
      if (fitter.rms_cost(a, b, c) > threshold)
        break;
    }

    if (pos<n)
      breaks.push_back(pos);
    else {
      breaks.push_back(n-1);
      break;
    }
  }
  //breaks.push_back(n-1); // last edgel
#endif
