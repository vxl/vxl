#include <iostream>
#include <osl/osl_fit_lines.h>
#include <osl/osl_easy_canny.h>
#include <osl/osl_save_topology.h>
#include <vil1/vil1_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// runs Canny on the given input image, followed by line fitting
int main(int argc, char *argv[])
{
  // input image
  if (argc < 2) return 1;
  vil1_image image = vil1_load(argv[1]);
  if (!image) return 2;

  // Canny edgel extraction
  std::list<osl_edge*> edgels;
  osl_easy_canny(0, image, &edgels);

  // line fitting
  osl_fit_lines fit = osl_fit_lines_params();
  std::list<osl_edge*> fit_results;
  fit.incremental_fit_to_list(&edgels, &fit_results);

  // output results
  osl_save_topology(std::cout, fit_results, std::list<osl_Vertex*>());

  return 0;
}
