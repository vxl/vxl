// This is brl/bseg/bvxm/pro/processes/bvxm_roc_process.cxx
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "bvxm_roc_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>


bool bvxm_roc_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_roc_process_globals;
  //process takes 3 inputs
  //input[0]: True changes directory
  //input[1]: Observed changes directory
  //input[2]: The ROC output filename
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  return pro.set_input_types(input_types_);
}

bool bvxm_roc_process(bprb_func_process& pro)
{
  using namespace bvxm_roc_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  std::string true_changes_dir = pro.get_input<std::string>(i++);
  std::string detected_changes_dir = pro.get_input<std::string>(i++);
  std::string roc_filename = pro.get_input<std::string>(i++);

  std::ofstream roc_stream(roc_filename.c_str());


  //iterate through changes dir, get all image names that we need to process
  std::vector<std::string> names;
  for ( vul_file_iterator fit = (true_changes_dir + "/*.*"); fit; ++fit )
  {
    if ( vul_file::is_directory(fit()) )
      continue;
    std::string image_name = fit();

    std::string file_ext = vul_file::extension(fit());

    //get extension
    if (( file_ext !=  ".png") & ( file_ext !=  ".jpg") & ( file_ext !=  ".tiff"))
    {
      std::cerr << "error: file extension not supported\n";
      continue;
    }

    names.push_back(vul_file::strip_extension(vul_file::basename(fit())));
  }

  //define threshold interval
  std::vector<float> thresh;
  for (unsigned p = 0; p <= 1000; p= p+10)
    thresh.push_back(float(float(p)/1000.0));


  //iterate through all threshold for each image
  for (float n : thresh)
  {
    unsigned change_marked_change = 0;
    unsigned change_marked_nonchange = 0;
    unsigned nonchange_marked_nonchange = 0;
    unsigned nonchange_marked_change = 0;

    for (auto & name : names)
    {
      //Read image containing probabilistic changes
      std::string prob_image = detected_changes_dir + "\\" + name + ".tiff";
      vil_image_view<float> prob_change = vil_load( prob_image.c_str() );

      unsigned image_height = prob_change.nj();
      unsigned image_width = prob_change.ni();

      //Read image containing true changes
      std::string true_change_file =  true_changes_dir + "\\" + name + ".png";
      vil_image_view<vxl_byte> true_change =
        vil_convert_to_grey_using_average( vil_load(true_change_file.c_str()));


      //iterate through the image
      vil_image_view<vxl_byte> detected_change(image_width, image_height);
      for ( unsigned x = 0; x < image_width; x++ ){
        for ( unsigned y = 0; y < image_height; y++ ){
          if ( prob_change(x,y) < (n * 20.0 ))
            detected_change(x,y) = 255;
          else
            detected_change(x,y) = 0;
        }
      }

      //Compare thesholded images to the model
      for ( unsigned i = 0; i < true_change.ni(); i++ ){
        for ( unsigned j = 0; j < true_change.nj()-1; j++ ){
         //hiafa
          /* unsigned thresh_i = i - 164;
          unsigned thresh_j =j - 424;*/
         /* unsigned thresh_i = i/2;
          unsigned thresh_j = j/2;*/
          //same dimensions
          unsigned thresh_i = i;
          unsigned thresh_j =j;

          //count changes
          if ( thresh_i < detected_change.ni() && thresh_j < detected_change.nj())
          {
            if (( true_change(i,j) != 255 )&& ( true_change(i,j) != 0 ))continue;

#ifdef DEBUG //for debugging
            if (true_change(i,j) == 255)
              std::cout<<i<<' '<< j<<' ' << thresh_i<< ' ' <<thresh_j<< '\n';
#endif // DEBUG
            if ( true_change(i,j) > 200  && detected_change(thresh_i, thresh_j) == 255 )
              change_marked_change++;
            if ( true_change(i,j) > 200 && detected_change(thresh_i, thresh_j) == 0 )
              change_marked_nonchange++;
            if ( true_change(i,j) < 50 && detected_change(thresh_i, thresh_j) == 255 )
              nonchange_marked_change++;
            if ( true_change(i,j) < 50 && detected_change(thresh_i, thresh_j) == 0 )
              nonchange_marked_nonchange++;
          }
        }
      }

      //save image: for debugging purposes
      std::stringstream img_out_ss;
      img_out_ss <<"./"<< n *20.0 <<".png";
      std::string img_out = img_out_ss.str();
      vil_save( detected_change, img_out.c_str()  );
    }

    //write the text file
    double percent_change_marked_change = (double)change_marked_change/
      (double)(change_marked_change+change_marked_nonchange);
    double percent_nonchange_marked_change = (double)nonchange_marked_change/
      (double)(nonchange_marked_change+nonchange_marked_nonchange);
    roc_stream << percent_change_marked_change << '\t' <<
      percent_nonchange_marked_change << '\n';
    std::cerr << '.';
  }
  std::cerr << '\n';
  return true;
}
