#include <osl/osl_fit_lines.h>
#include <osl/osl_easy_canny.h>
#include <osl/osl_save_topology.h>
#include <vil/vil_load.h>
#include <vcl_iostream.h>

// runs Canny on the given input image, followed by line fitting
int main(int argc, char *argv[])
{
  // input image
  if (argc < 2) return 1;
  vil_image image = vil_load(argv[1]);
  if (!image) return 2;

  // Canny edgel extraction
  vcl_list<osl_edge*> edgels;
  osl_easy_canny(0, image, &edgels);

  // line fitting
  osl_fit_lines fit = osl_fit_lines_params();
  vcl_list<osl_edge*> fit_results;
  fit.incremental_fit_to_list(&edgels, &fit_results);

  // output results
  osl_save_topology(vcl_cout, fit_results, vcl_list<osl_Vertex*>());

  return 0;
}
