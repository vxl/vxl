// This example displays two images in a deck.
// Images are loaded from the two filenames
// on the command line.

#include <vcl_iostream.h>
#include <vil/vil_load.h>

//cvg includes
#include <cvg/cvg_hemisphere_tableau.h>

//vgui includes
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//vbl array 2d of strings
#include <vbl/vbl_array_2d.h>

int main(int argc, char **argv)
{
  //init vgui (should choose/determine toolkit)
  vgui::init(argc, argv);
  vul_arg<vcl_string> imgdir("-img", "image directory", "");
  vul_arg_parse(argc, argv);


  //read sphere
  vsph_view_sphere<vsph_view_point<vcl_string> > isphere; 
  vcl_string sphere_path = imgdir() + "/sphere.bin";
  vsl_b_ifstream sphere_os(sphere_path);
  if (!sphere_os) {
    vcl_cout<<"cannot open "<<sphere_path<<" for writing\n";
    return -1;
  }
  vsl_b_read(sphere_os, isphere); 
  sphere_os.close();
  vcl_cout<<"sphere info : "<<isphere.size()<<vcl_endl;
  
  
  //get first one and put it in image
  vcl_string* first_img = isphere.begin()->second.metadata();
  vcl_cout<<"first_img "<<(*first_img)<<vcl_endl;
  vil_image_resource_sptr im = vil_load_image_resource(first_img->c_str());
  if ( !im ) {
    vcl_cerr << "Could not load " << first_img->c_str() << '\n';
    return 1;
  }
  
  // Load image (given in the first command line param) into an image tableau.
  cvg_hemisphere_tableau_new image(im, isphere);

  // Put the image tableau inside a 2D viewer tableau (for zoom, etc).
  vgui_viewer2D_tableau_new viewer(image);

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, image->width()/2, image->height()/2);
}
