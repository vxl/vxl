#include <vsl/vsl_harris_params.h>
#include <vsl/vsl_harris.h>

#include <vcl/vcl_string.h>
#include <vbl/vbl_arg.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

int main(int argc,char **argv) {
  vbl_arg<vcl_string> infile ("-in"   ,"input image file"    ,"");
  vbl_arg<vcl_string> outfile("-out"  ,"output corner file"  ,"");
  vbl_arg<double>     sigma  ("-sigma","gauss sigma"         ,0.7);
  vbl_arg<vcl_string> cormap ("-map"  ,"cornerness map (pnm)","");
  vbl_arg<bool>       pab    ("-pab"  ,"emulate pab harris"  ,false);
  vbl_arg_parse(argc,argv);
  
  assert(infile()  != "");
  assert(outfile() != "");

  // load image
  vil_image I = vil_load(infile().c_str());

  // parameters
  vsl_harris_params params;
  params.gauss_sigma = sigma();
  params.verbose = true;
  params.pab_emulate=pab();
  
  // compute object
  vsl_harris H(params);
  H.compute(I);

  // save
  H.save_corners(outfile().c_str());

  if (cormap() != "") // cornerness map
    vil_save(*H.image_cornerness_ptr, cormap().c_str(), "pnm");
  
  return 0;
}
