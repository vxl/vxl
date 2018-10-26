#ifndef brip_line_generator_h
#define brip_line_generator_h
//:
// \file
// \brief A process for generating a digital line
// \author J.L. Mundy
// \date May 4, 2008
//
// \verbatim
//  Modifications ported from TargetJr
// \endverbatim
//
//  Incremental generation of a digital line, a series of pixels that
//  lie on a line segment specified by (xs, ys)->(xe, ye).  The function
//  returns true when each new pixel is generated.  When the line segment
//  is completed the function returns false. The first call to the function
//  should have init = true to signal the start of the generation process.
//
//  Usage is as follows:
//   ...
//   vil_image_view<unsigned short> img = resc.get_view();//some resource
//   bool init = true;
//   while (brip_line_generator::generate(init, xs, ys, xe, ye, x, y))
//   {
//     int xi = (int)x, yi = (int)y; //convert the pixel location to integer
//     unsigned short val = img(xi, yi) //get a pixel value
//        ....
//   }
//
// -------------------------------------------------------------------------

class brip_line_generator
{
 public:
  static bool generate(bool& init, float xs, float ys,
                       float xe, float ye,
                       float& x, float& y);
 private:
  brip_line_generator() = delete;
};

#endif // brip_line_generator_h
