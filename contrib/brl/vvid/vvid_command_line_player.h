// This is brl/vvid/vvid_command_line_player.h
#ifndef vvid_command_line_player_h_
#define vvid_command_line_player_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author D.E.Crispell
// \brief Processes video files from the command line (no graphical output)
//
// \verbatim
//  Modifications:
//   D.E.Crispell Sept 9, 2003 created
// \endverbatim
//----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vpro/vpro_video_process_sptr.h>
#include <vul/vul_arg.h>
#include <vidl_vil1/vidl_vil1_movie_sptr.h>

//: A class for proccessing videos.
class vvid_command_line_player
{
 public:
  vvid_command_line_player();
  ~vvid_command_line_player();

  //: load a video
  bool load_video_file();//vcl_string filename);

  //: set video process
  bool set_video_process(vpro_video_process_sptr video_proc);

  //: register an output file so that it can be identified in the XML output
  bool add_output_file(vul_arg<vcl_string> output_file);

  //: set the status output filename
  //bool set_status_output_file(vcl_string filename);

  //: play the video
  bool play_video();

  //: add required system_info args
  bool add_system_info_args(vul_arg_info_list& arg_list);

  //: check if user only wants an xml paramter file output
  bool print_params_only();

  //: print and xml parameter file
  bool print_xml_params(vul_arg_info_list& arg_list,
                        vcl_string param_block_name);

  //: print performance output
  bool print_performance_output(vcl_string video_name, vcl_vector<float> frame_scores);

 protected:
  void write_status(vcl_string filename, int iframe);

 private:
  vidl_vil1_movie_sptr movie_;
  vpro_video_process_sptr video_process_;
  vcl_string status_output_filename_;
  int nframes_;

  vul_arg_info_list arg_list_;
  vul_arg<vcl_string> *parameter_output_file_;
  vul_arg<vcl_string> *input_video_file_;
  vul_arg<vcl_string> *status_block_file_;
  vul_arg<vcl_string> *output_directory_;
  vul_arg<vcl_string> *performance_output_file_;
  vul_arg<bool> *print_params_flag_;
  vcl_vector<vul_arg<vcl_string> > output_files_;
};

#endif
