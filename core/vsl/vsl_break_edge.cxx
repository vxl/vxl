/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_break_edge.h"

#include <vcl_cassert.h>
#include <vsl/vsl_edge.h>
#include <vsl/vsl_ortho_regress.h>

void vsl_break_edge(vsl_edge const *in,
		    vcl_vector<unsigned> const &where,
		    vcl_list<vsl_edge*> *broken)
{
  assert(in);
  float const *x = in->GetX();
  float const *y = in->GetY();
  unsigned n = in->size();
  
  assert(broken);
  assert(!where.empty());
  assert(where.front() == 0);
  assert(where.back() == n-1);
  
  // make array of vertices, one for each break.
  vcl_vector<vsl_vertex*> verts;
  verts.push_back(in->GetV1()); // maintain topology.
  for (unsigned i=1; i<where.size()-1; ++i)
    verts.push_back(new vsl_vertex(x[where[i]], y[where[i]]));
  verts.push_back(in->GetV2()); // maintain topology.
  
  // make new edges and push them onto the given list.
  for (unsigned i=0; i<where.size()-1; ++i) {
    vsl_edge *fragment = new vsl_edge(where[i+1]-where[i] + 1, verts[i], verts[i+1]);
    for (unsigned j=0; j<fragment->size(); ++j) {
      fragment->SetX(x[j + where[i]], j);
      fragment->SetY(y[j + where[i]], j);
    }
    broken->push_back(fragment);
  }
}

void vsl_break_edge(vsl_edge const *in, 
		    vcl_list<vsl_edge*> *out_ptr,
		    double threshold VCL_DEFAULT_VALUE(0.2),
		    unsigned nbhd_size VCL_DEFAULT_VALUE(2))
{
  assert(in);
  
  float const *x = in->GetX();
  float const *y = in->GetY();
  unsigned n = in->size();

  vsl_ortho_regress fitter;

  // decide where to break the edge.
  vcl_vector<unsigned> breaks;

  breaks.push_back(0);   // first edgel
  for (int pos=nbhd_size; pos<n-nbhd_size; ++pos) {

    fitter.reset(); // we could make this more efficient.
    fitter.add_points(&x[pos-nbhd_size], &y[pos-nbhd_size], 2*nbhd_size+1);
    
    double a, b, c;
    fitter.fit(&a, &b, &c);
    
    if (fitter.rms_cost(a, b, c) > threshold)
      breaks.push_back(pos);
  }
  breaks.push_back(n-1); // last edgel
  
  vsl_break_edge(in, breaks, out_ptr);
}

//--------------------------------------------------------------------------------














// defunct
#if 0
  breaks.push_back(0);   // first edgel
  while (true) {
    int pos = breaks.back();
    if (pos >= n-1)
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
