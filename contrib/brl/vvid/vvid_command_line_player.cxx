// This is brl/vvid/vvid_command_line_player.cxx
#include "vvid_command_line_player.h"
//:
// \file
// \author D.E.Crispell

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vpro/vpro_video_process.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_io.h>
#include <vidl_vil1/vidl_vil1_frame.h>

//: Default Constructor
vvid_command_line_player::vvid_command_line_player()
{
   movie_=(vidl_vil1_movie*)0;
   video_process_ = 0;
   status_output_filename_ = "";

   parameter_output_file_ = new vul_arg<vcl_string>(arg_list_,"-X","parameter output file","");
   input_video_file_ = new vul_arg<vcl_string>(arg_list_,"-V","video input file","");
   status_block_file_ = new vul_arg<vcl_string>(arg_list_,"-S","status block file","");
   performance_output_file_ = new vul_arg<vcl_string>(arg_list_,"-P","performance output file","");
   output_directory_ = new vul_arg<vcl_string>(arg_list_,"-O","output directory","");
   print_params_flag_ = new vul_arg<bool>(arg_list_,"-print_params_only",
                                          "print xml parameter file and exit",false);
}

//: Destructor
vvid_command_line_player::~vvid_command_line_player()
{
  delete parameter_output_file_;
  delete input_video_file_;
  delete status_block_file_;
  delete output_directory_;
  delete print_params_flag_;
}

//: load a video
bool vvid_command_line_player::load_video_file()//vcl_string filename)
{
  vcl_string filename = (*input_video_file_)();
  movie_ = vidl_vil1_io::load_movie(filename.c_str());
  if (!movie_)
  {
    vcl_cerr << "Failed to load movie file\n";
    return false;
  }
  vidl_vil1_movie::frame_iterator pframe = movie_->last();
  nframes_ = pframe->get_real_frame_index();
  return true;
}

//: set video process
bool vvid_command_line_player::set_video_process(vpro_video_process_sptr video_proc)
{
  video_process_ = video_proc;
  return true;
}


//: add the required SYSTEM_INFO args to the list of args.
// Algorithm-specific args can then be added to the list and parsed.
bool vvid_command_line_player::add_system_info_args(vul_arg_info_list& arg_list)
{
  arg_list.add(parameter_output_file_);
  arg_list.add(input_video_file_);
  arg_list.add(status_block_file_);
  arg_list.add(output_directory_);
  arg_list.add(performance_output_file_);
  arg_list.add(print_params_flag_);

  return true;
}

//: play the video
bool vvid_command_line_player::play_video()
{
  if (!(video_process_ && movie_))
  {
    vcl_cerr << "Video Process or Movie not loaded\n";
    return false;
  }
  for (vidl_vil1_movie::frame_iterator pframe=movie_->begin();
       pframe!=movie_->end();
       ++pframe)
  {
    vcl_cout << "frame["<< pframe->get_real_frame_index()<<"]\n";
    vil1_image img = pframe->get_image();

    vil1_memory_image_of<unsigned char> image(img);

    // note - it is up to the video process to call clear_input() after each frame
    video_process_->add_input_image(image);

    if (video_process_->execute())
    {
      //if (video_process_->get_output_type()==vpro_video_process::SPATIAL_OBJECT)

      //else if (video_process_->get_output_type()==vpro_video_process::IMAGE)

      //else if (video_process_->get_output_type()==vpro_video_process::TOPOLOGY)
    }
    // write status block
    if ((*status_block_file_)() != "")
        write_status((*status_block_file_)(), pframe->get_real_frame_index());
  }
  return true;
}

//: write status block to file
// TODO: each video process should be able to have its own status_block type
void vvid_command_line_player::write_status(vcl_string output_file, int iframe)
{
  float percent_done = 0;
  if (nframes_)
    percent_done = float(iframe)/float(nframes_);

  vcl_ofstream outstream(output_file.c_str());
  outstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<status>\n"
            << "  <basic>\n"
            << "    <info percent_completed=\""<<percent_done<<"\"/>\n"
            << "  </basic>\n"
            << "  <optional>\n"
            << "    <info number_of_frames_processed=\""<<iframe<<"\"/>\n"
            << "    <info total_number_of_frames=\""<<nframes_<<"\"/>\n"
            << "  </optional>\n"
            << "</status>\n";

  outstream.close();
}

//: so the wrapper program can exit if the user wants param file only
bool vvid_command_line_player::print_params_only()
{
  return (*print_params_flag_)();
}

bool vvid_command_line_player::add_output_file(vul_arg<vcl_string> output_file)
{
  output_files_.push_back(output_file);
  return true;
}

bool vvid_command_line_player::print_performance_output(vcl_string video_name,
                                                        vcl_vector<float> frame_scores)
{
  vcl_cout << "printing performance\n";
  vcl_string filename = (*performance_output_file_)();
  if (filename == "")
    return false;
  vcl_ofstream outstream(filename.c_str());
  if (!outstream)
  {
    vcl_cerr << "error: could not create performance output file ["<<filename<<"]\n";
    return false;
  }
  outstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<performance>\n"
            << "  <description>Video Frame Scores</description>\n"
            << "  <frames>\n"
            << "    <video name=\""<<video_name<<"\" totalframes=\""<<frame_scores.size()<<"\">\n";
  int frame_idx = 0;
  for (vcl_vector<float>::iterator fit=frame_scores.begin(); fit != frame_scores.end(); fit++)
  {
    outstream << "      <frame index=\""<<frame_idx++<<"\" score=\""<<*fit<<"\"/>\n";
  }
  outstream << "    </video>\n"
            << "  </frames>\n"
            << "</performance>\n";

  outstream.close();
  return true;
}
bool vvid_command_line_player::print_xml_params(vul_arg_info_list& arg_list,
                                                vcl_string param_block_name)
{
  vcl_string filename = (*parameter_output_file_)();
  if (filename == "")
    return false;

  vcl_ofstream outstream(filename.c_str());
  if (!outstream)
  {
    vcl_cerr << "error: could not create param output file ["<<filename<<"]\n";
    return false;
  }
  int n_args = arg_list.args_.size();

  outstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<vxl>\n"
            << "  <params app=\""<<param_block_name<<"\" nparams=\""<<n_args<<"\">\n";

  for (vcl_vector<vul_arg_base*>::iterator arg_it = arg_list.args_.begin();
       arg_it != arg_list.args_.end(); arg_it++)
  {
    vul_arg_base *arg = *arg_it;
    vcl_string command = arg->option();
    vcl_string description = arg->help();
    vcl_string type = arg->type_;
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
    // see if param is an output file
    for (vcl_vector<vul_arg<vcl_string> >::iterator pit = output_files_.begin();
         pit != output_files_.end(); pit++)
    {
      vcl_string of_cmd = ( (*pit).option() );
        if (command == of_cmd)
        {
          outstream << " System_info=\"OUTPUT\"";
          break;
        }
    }
    outstream << " value=\"";
    // if arg is a string, we have to get rid of ' ' around value
    vcl_ostringstream value_stream;
    arg->print_value(value_stream);
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
