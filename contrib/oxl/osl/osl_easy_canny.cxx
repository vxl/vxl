// This is oxl/osl/osl_easy_canny.cxx
//:
// \file
// \author fsm

#include <iostream>
#include "osl_easy_canny.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <osl/osl_topology.h>

#include <osl/osl_canny_ox_params.h>
#include <osl/osl_canny_ox.h>

#include <osl/osl_canny_rothwell_params.h>
#include <osl/osl_canny_rothwell.h>

#include <osl/osl_edge_detector_params.h>
#include <osl/osl_edge_detector.h>

void osl_easy_canny(int which_canny,
                    vil1_image const &image,
                    std::list<osl_edge*> *edges,
                    double sigma)
{
  assert(edges!=nullptr);

  switch (which_canny) {
  case 0: {
    osl_canny_ox_params params;
    if (sigma)
      params.sigma = (float)sigma;
    osl_canny_ox filter(params);
    filter.detect_edges(image, edges);
  } break;

  case 1: {
    osl_canny_rothwell_params params;
    if (sigma)
      params.sigma = (float)sigma;
    osl_canny_rothwell filter(params);
    filter.detect_edges(image, edges);
  } break;

  case 2: {
    osl_edge_detector_params params;
    if (sigma)
      params.sigma_ = (float)sigma;
    osl_edge_detector filter(params);
    filter.detect_edges(image, edges);
  } break;

  default:
    std::cerr << __FILE__ ": unrecognised which_canny=" << which_canny << std::endl;
    break;
  }
}
