/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_easy_canny.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>

#include <vsl/vsl_topology.h>

#include <vsl/vsl_canny_ox_params.h>
#include <vsl/vsl_canny_ox.h>

#include <vsl/vsl_canny_rothwell_params.h>
#include <vsl/vsl_canny_rothwell.h>

#include <vsl/vsl_edge_detector_params.h>
#include <vsl/vsl_edge_detector.h>

void vsl_easy_canny(int which_canny, 
		    vil_image const &image, 
		    vcl_list<vsl_edge*> *edges,
		    double sigma VCL_DEFAULT_VALUE(0))
{
  assert(edges);
  
  switch (which_canny) {
  case 0: {
    vsl_canny_ox_params params;
    if (sigma)
      params.sigma = sigma;
    vsl_canny_ox filter(params);
    filter.detect_edges(image, edges);
  } break;
  
  case 1: {
    vsl_canny_rothwell_params params;
    if (sigma)
      params.sigma = sigma;
    vsl_canny_rothwell filter(params);
    filter.detect_edges(image, edges);
  } break;
  
  case 2: {
    vsl_edge_detector_params params;
    if (sigma)
      params._sigma = sigma;
    vsl_edge_detector filter(params);
    filter.detect_edges(image, edges);
  } break;

  default:
    vcl_cerr << __FILE__ ": unrecognised which_canny=" << which_canny << vcl_endl;
    break;
  }

}
