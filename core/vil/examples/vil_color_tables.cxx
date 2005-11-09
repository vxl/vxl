// This executable will create a sample image from
// each of the 37 color tables found in vil_color_table.h
// The created image is simply a ramp through 255 values.

#include <vcl_sstream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_color_table.h>

int main()
{
  vil_image_view<vil_rgb<vxl_byte> > cimg(2550,100,1);

  // Write an image for each of the 38 color maps
  for (unsigned int c=0; c<38; c++) {
    for (unsigned int j=0; j<cimg.nj(); j++) {
      for (unsigned int i=0; i<cimg.ni(); i++) {
        int color_index = int(i*0.1+0.5);
        cimg(i,j) = color_value(c, color_index);
      }
    }
    vcl_string fname;
    vcl_stringstream s;
    s << "color_table_" << c << ".tif";
    fname = s.str();
    vcl_cout << "Saving " << fname << vcl_endl;
    vil_save(cimg, fname.c_str());
  }

  return 0;
}
