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
  
  /////////////////////////////////////////////////////////////////////////////
  // Create v sphere
  /////////////////////////////////////////////////////////////////////////////
  //boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  //vsph_view_sphere<vsph_view_point<vcl_string> > sphere(scene->bounding_box(), radius());
  //sphere.add_uniform_views(vnl_math::pi/3, vnl_math::pi/18.0, ni(), nj());
  //vcl_cout<<"Number of views to store: "<<sphere.size()<<vcl_endl;
  
  ////count images to make sure there is one per view
  //int num_imgs = 0;
  //for (vul_file_iterator im=imgdir()+"/*.*g"; im; ++im) ++num_imgs;
  //if(num_imgs != sphere.size()) {
    //vcl_cout<<"Number of images doesn't match number of views!!!!"<<vcl_endl;
    //return -1; 
  //}
  
  ////iterate through views and assign them
  //vsph_view_sphere<vsph_view_point<vcl_string> >::iterator iter; int img_index=0;
  //for(iter = sphere.begin(); iter != sphere.end(); ++iter, ++img_index)
  //{
    //vcl_stringstream stream;
    //stream<<imgdir()<<"/expimg_"<<img_index<<".png"; 
    //vcl_string* filepath = new vcl_string();
    //(*filepath) = stream.str(); 

    //vsph_view_point<vcl_string>& view = iter->second; 
    //view.set_metadata(filepath); 
  //}

  ////get all of the row directories
  //vcl_vector<vcl_string> row_dirs;
  //for (vul_file_iterator fn=imgdir()+"/row*"; fn; ++fn) 
    //row_dirs.push_back(fn()); 
  //vcl_sort(row_dirs.begin(), row_dirs.end()); 
  
  ////figure out how many images there are in a row dir
  //int imgs_per_row = 0;
  //for (vul_file_iterator im=row_dirs[0]+"/*.*g"; im; ++im) ++imgs_per_row;
  
  ////throw together 2d string array
  //vbl_array_2d<vcl_string> images(row_dirs.size(), imgs_per_row); 
  //for(int row=0; row<row_dirs.size(); ++row) 
  //{
    //vul_file_iterator im=row_dirs[row]+"/*.*g";
    //for(int col=0; col<imgs_per_row; ++col, ++im)
       //images(row, col) = im(); 
  //}
  //vcl_cout<<"Created Image array of size: "<<images.rows()<<","<<images.cols()<<vcl_endl;
  
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
  return vgui::run(shell, image->width(), image->height());
}
