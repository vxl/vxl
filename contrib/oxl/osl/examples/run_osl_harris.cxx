// This is oxl/osl/examples/run_osl_harris.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation "vul_arg.h"//otherwise "unresolved typeinfo vul_arg_base"
#endif

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <osl/osl_harris_params.h>
#include <osl/osl_harris.h>

// ** please don't make this program interactive. it must be
// usable in a script, as a filter. **
int main(int argc,char **argv)
{
  vul_arg<vcl_string> infile ("-in"   ,"input image file"    ,"-"); // default is stdin.
  vul_arg<vcl_string> outfile("-out"  ,"output corner file (default is stdout)"  ,"");
  vul_arg<double>     sigma  ("-sigma","gauss sigma"         ,0.7);
  vul_arg<int>        corner_count_max("-c","Max number of corners", 900);
  vul_arg<int>        adaptive_window_size("-w","Adaptive window size (0 disables)", 64);
  vul_arg<vcl_string> cormap ("-map"  ,"cornerness map (pnm)","");
  vul_arg<bool>       pab    ("-pab"  ,"emulate pab harris"  ,false);
  vul_arg_parse(argc,argv);

  // load image
  vil1_image I;
  if (infile() == "-") {
    //vcl_cerr << "reading image from stdin\n";
    //I = vil1_load(cin);
    vcl_cerr << "cannot read from stdin yet\n";
    return 1;
  }
  else
    I = vil1_load(infile().c_str());

  // parameters
  osl_harris_params params;
  params.corner_count_max = corner_count_max();
  params.gauss_sigma = (float)sigma();
  params.verbose = true;
  params.adaptive_window_size = adaptive_window_size();
  params.adaptive = (adaptive_window_size() != 0);
  params.pab_emulate = pab();

  // compute object
  osl_harris H(params);
  H.compute(I);

  // save
  if (outfile() == "") {
    vcl_cerr << "writing image to stdout\n";
    H.save_corners(vcl_cout);
  }
  else
    H.save_corners(outfile().c_str());

  // cornerness map
  if (cormap.set())
    vil1_save(H.image_cornerness_buf, cormap().c_str(), "pnm");

  return 0;
}
