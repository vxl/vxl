/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "osl_easy_canny.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <osl/osl_topology.h>

#include <osl/osl_canny_ox_params.h>
#include <osl/osl_canny_ox.h>

#include <osl/osl_canny_rothwell_params.h>
#include <osl/osl_canny_rothwell.h>

#include <osl/osl_edge_detector_params.h>
#include <osl/osl_edge_detector.h>

void osl_easy_canny(int which_canny,
                    vil_image const &image,
                    vcl_list<osl_edge*> *edges,
                    double sigma)
{
  assert(edges!=0);

  switch (which_canny) {
  case 0: {
    osl_canny_ox_params params;
    if (sigma)
      params.sigma = sigma;
    osl_canny_ox filter(params);
    filter.detect_edges(image, edges);
  } break;

  case 1: {
    osl_canny_rothwell_params params;
    if (sigma)
      params.sigma = sigma;
    osl_canny_rothwell filter(params);
    filter.detect_edges(image, edges);
  } break;

  case 2: {
    osl_edge_detector_params params;
    if (sigma)
      params._sigma = sigma;
    osl_edge_detector filter(params);
    filter.detect_edges(image, edges);
  } break;

  default:
    vcl_cerr << __FILE__ ": unrecognised which_canny=" << which_canny << vcl_endl;
    break;
  }

}
