#include <vcl/vcl_string.h> // C++ specific includes first
#include <vcl/vcl_cassert.h>
#include <vbl/vbl_arg.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vsl/vsl_harris_params.h>
#include <vsl/vsl_harris.h>

// ** please don't make this program interactive. it must be
// usable in a script, as a filter. **
int main(int argc,char **argv) {
  vbl_arg<vcl_string> infile ("-in"   ,"input image file"    ,"-"); // default is stdin.
  vbl_arg<vcl_string> outfile("-out"  ,"output corner file (default is stdout)"  ,"");
  vbl_arg<double>     sigma  ("-sigma","gauss sigma"         ,0.7);
  vbl_arg<int>        corner_count_max("-c","Max number of corners", 900);
  vbl_arg<int>        adaptive_window_size("-w","Adaptive window size (0 disables)", 64);
  vbl_arg<vcl_string> cormap ("-map"  ,"cornerness map (pnm)","");
  vbl_arg<bool>       pab    ("-pab"  ,"emulate pab harris"  ,false);
  vbl_arg_parse(argc,argv);
  
  // load image
  vil_image I;
  if (infile() == "-") {
    //cerr << "reading image from stdin" << endl;
    //I = vil_load(cin);
    cerr << "cannot read from stdin yet" << endl;
    return 1;
  }
  else
    I = vil_load(infile().c_str());
  
  // parameters
  vsl_harris_params params;
  params.corner_count_max = corner_count_max();
  params.gauss_sigma = sigma();
  params.verbose = true;
  params.adaptive_window_size = adaptive_window_size();
  params.adaptive = (adaptive_window_size() != 0);
  params.pab_emulate = pab();
  
  // compute object
  vsl_harris H(params);
  H.compute(I);

  // save
  if (outfile() == "") {
    cerr << "writing image to stdout" << endl;
    H.save_corners(cout);
  }
  else
    H.save_corners(outfile().c_str());

  // cornerness map
  if (cormap.set())
    vil_save(H.image_cornerness_buf, cormap().c_str(), "pnm");
  
  return 0;
}
