#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vul/vul_arg.h>
#include <vidl/vidl_movie.h>
#include <vidl/vidl_io.h>
#include <vidl/vidl_frame.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vidl/vidl_image_list_codec.h>

#ifdef HAS_MPEG2
# include <vidl/vidl_mpegcodec.h>
#endif

#ifdef VCL_WIN32
#include <vidl/vidl_avicodec.h>
#endif


#include <vil/algo/vil_threshold.txx>
VIL_THRESHOLD_INSTANTIATE(float);

bool print_xml_params( vcl_string output_file,
                       vul_arg_info_list& arg_list,
                       vcl_string param_block_name );
                       

int main(int argc, char** argv)
{
  // Register video codecs
  vidl_io::register_codec(new vidl_image_list_codec);

#ifdef VCL_WIN32
  vidl_io::register_codec(new vidl_avicodec);
#endif

#ifdef HAS_MPEG2
  vidl_io::register_codec(new vidl_mpegcodec);
#endif

  // Arguments
  vul_arg_info_list arg_list;
  vul_arg<vcl_string> parameter_output_file(arg_list,"-X","parameter output file","");
  vul_arg<vcl_string> input_video_file(arg_list,"-V","video input file","");
  vul_arg<vcl_string> status_block_file(arg_list,"-S","status block file","");
  vul_arg<vcl_string> performance_output_file(arg_list,"-P","performance output file","");
  vul_arg<vcl_string> output_directory(arg_list,"-O","output directory","");
  vul_arg<bool> print_params_only(arg_list,"-print_params_only",
                                           "print xml paramter file and exit",false);
  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  // if print_xml_params returns 0, exit with no error
  if (parameter_output_file() != "")
    print_xml_params(parameter_output_file(), arg_list, "motion_test_params");
  if ( print_params_only() )
    return 0;
  

  
  // Open the movie
  vidl_movie_sptr video = vidl_io::load_movie(input_video_file().c_str());
  if (!video) {
    vcl_cerr << "Failed to open the video" << vcl_endl;
    return -1;
  }

  


  // Load each frame of the movie, convert to greyscale, cast to float
  //   and push onto a vector
  vcl_vector< vil_image_view<float> > images;
  for ( vidl_movie::frame_iterator f_itr = video->first();
        f_itr != video->end();  ++f_itr ){
    vil_image_view<float> input_image, grey_image;
    input_image = vil_convert_stretch_range((float)0.0, f_itr->get_view());
    if( input_image.nplanes() == 3 ) {
      vil_convert_planes_to_grey( input_image , grey_image );
    } 
    else if ( input_image.nplanes() == 1 ) {
      grey_image = input_image;
    }
    else{
      vcl_cerr << "Error: can't deal with an image with " 
               << input_image.nplanes() << " planes."<< vcl_endl;
      return -1;
    }
    images.push_back(grey_image);
  }

  for ( vcl_vector< vil_image_view<float> >::iterator itr = images.begin()+1;
        itr != images.end();  ++itr ){
    vil_image_view<float> diff_img;
    vil_math_image_abs_difference(*itr, *(itr-1), diff_img);
    vil_image_view<bool> bool_img;
    vil_threshold_above<float>( diff_img, bool_img, 0.5);
    int sum = 0;
    vil_math_sum(sum, bool_img, 0);
    int total = bool_img.ni()*bool_img.nj();
    vcl_cout << "Sum: "<<sum<<" total: "<< total << " ratio: "<< double(sum)/total << vcl_endl;
  }
  vcl_cout << "DONE!!!" << vcl_endl;

  return 0;
}



// print the parameters to an XML file
bool print_xml_params( vcl_string output_file,
                       vul_arg_info_list& arg_list,
                       vcl_string param_block_name )
{
  vcl_ofstream outstream(output_file.c_str());
  if (!outstream)
  {
    vcl_cerr << "error: could not create param output file ["<<output_file<<"]\n";
    return false;
  }
  int n_args = arg_list.args_.size();

  outstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<vxl>\n"
            << "  <params app=\""<<param_block_name<<"\" nparams=\""<<n_args<<"\">\n";

  for (vcl_vector<vul_arg_base*>::iterator arg_it = arg_list.args_.begin();
       arg_it != arg_list.args_.end(); ++arg_it)
  {
    vcl_string command = (*arg_it)->option();
    vcl_string description = (*arg_it)->help();
    vcl_string type = (*arg_it)->type_;
    if (type == "bool")
      type = "flag";

    outstream << "    <param command=\"" << command << "\" description=\""
              << description << "\" type=\"" << type << '\"';
    if (command == "-V")
      outstream << " System_info=\"INPUT_VIDEO\"";
    if (command == "-O")
      outstream << " System_info=\"OUTPUT_DIRECTORY\"";
    if (command == "-S")
      outstream << " System_info=\"STATUS_BLOCK\"";
    if (command == "-P")
      outstream << " System_info=\"PERFORMANCE_OUTPUT\"";
//    // see if param is an output file
//    for (vcl_vector<vul_arg<vcl_string> >::iterator pit = output_files_.begin();
//         pit != output_files_.end(); pit++)
//    {
//      vcl_string of_cmd = ( (*pit).option() );
//        if (command == of_cmd)
//        {
//          outstream << " System_info=\"OUTPUT\"";
//          break;
//        }
//    }
    outstream << " value=\"";
    // if arg is a string, we have to get rid of ' ' around value
    vcl_ostringstream value_stream;
    (*arg_it)->print_value(value_stream);
    if (type == "string")
    {
      vcl_string value_string = value_stream.str();
      value_string.replace(value_string.find("'"),1,"");
      value_string.replace(value_string.find("'"),1,"");
      outstream << value_string;
    }
    else if (type == "flag")
    {
      vcl_string value_string = value_stream.str();
      if ((value_string == "not set") || (command == "-print_params_only"))
        value_string = "off";
      else if (value_string == "set")
        value_string = "on";
      else
        value_string = "unknown";
      outstream << value_string;
    }
    else
      outstream << value_stream.str();
    outstream << "\" />\n";
  }
  outstream << "  </params>\n"
            << "</vxl>\n";

  outstream.close();
  return true;
}

