#include <vsl/vsl_harris_params.h>
#include <vsl/vsl_harris.h>

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_string.h>
#include <vbl/vbl_arg.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

int main(int argc,char **argv) {
  vbl_arg<vcl_string> infile ("-in"   ,"input image file"    ,""); // default should be stdin.
  vbl_arg<vcl_string> outfile("-out"  ,"output corner file (default is stdout)"  ,"");
  vbl_arg<double>     sigma  ("-sigma","gauss sigma"         ,0.7);
  vbl_arg<int>        corner_count_max("-c","Max number of corners", 900);
  vbl_arg<int>        adaptive_window_size("-w","Adaptive window size (0 disables)", 64);
  vbl_arg<vcl_string> cormap ("-map"  ,"cornerness map (pnm)","");
  vbl_arg<bool>       pab    ("-pab"  ,"emulate pab harris"  ,false);
  vbl_arg_parse(argc,argv);
  
  vcl_string* in_file = new vcl_string(infile());
  if (*in_file == "") {
    cout << "input image file: ";
    char tmp[1024];
    cin >> tmp;
    delete in_file;
    in_file = new vcl_string(tmp);
  }
  assert(*in_file != "");

  vcl_string* out_file = new vcl_string(outfile());
  if (*out_file == "") {
    cout << "output image file: ";
    char tmp[1024];
    cin >> tmp;
    delete out_file;
    out_file = new vcl_string(tmp);
  }
  assert(*out_file != "");

  // load image
  vil_image I = vil_load(in_file->c_str());
  delete in_file;

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
  H.save_corners(out_file->c_str());
  delete out_file;

  if (cormap() != "") // cornerness map
    vil_save(*H.image_cornerness_ptr, cormap().c_str(), "pnm");
  
  return 0;
}
