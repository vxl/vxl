#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_fit_lines_params.h>
#include <sdet/sdet_grid_finder_params.h>
#include <vpro/vpro_video_process_sptr.h>
#include <vpro/vpro_grid_finder_process.h>
#include <vvid/vvid_command_line_player.h>
#include <vul/vul_arg.h>

int main(int argc, char** argv)
{
  // create arg list
  vul_arg_info_list arg_list;

  // create player
  vvid_command_line_player player;

  vul_arg<vcl_string> output_file(arg_list,"-F","output file","grid_points.txt");

  // sdet_detector_params
  vul_arg<float> smooth_sigma(arg_list,"-ss","smooth sigma",(float)1.0);
  vul_arg<float> noise_threshold(arg_list,"-nt","noise threshold",(float)3.5);
  vul_arg<bool> automatic_threshold(arg_list,"-auto_threshold","automatic threshold",false);
  vul_arg<bool> aggressive_closure(arg_list,"-aggressive_closure","aggressive closure",true);
  vul_arg<bool> compute_junctions(arg_list,"-compute_junctions","compute junctions",true);
  //sdet_fit_lines params
  vul_arg<int> min_fit_length(arg_list,"-mfl","min line fit length",10);
  vul_arg<float> rms_distance(arg_list,"-rmsd","max rms distance",(float)0.1);
  //sdet_grid_finder params
  vul_arg<float> angle_tolerance(arg_list,"-at","angle tolerance",(float)5.0);
  vul_arg<int> line_cnt_threshold(arg_list,"-lct","line count threshold",1);

  // system_info args
  player.add_system_info_args(arg_list);
  player.add_output_file(output_file);

  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);


  // if print_xml_params returns 0, exit with no error
  vcl_string param_block_name("grid_finder_params");
  player.print_xml_params(arg_list,param_block_name);
  if (player.print_params_only())
    return 0;

#if 0
  if (parameter_output_file() != "")
  {
    vcl_string param_block_name("grid_finder_params");
    player.print_xml_params(parameter_output_file(), arg_list, param_block_name);
    return 0;
  }
#endif

  sdet_detector_params dp;
  sdet_fit_lines_params flp;
  sdet_grid_finder_params gfp;

  dp.smooth = smooth_sigma();
  dp.noise_multiplier = noise_threshold();
  dp.automatic_threshold = automatic_threshold();
  if (aggressive_closure())
    dp.aggressive_junction_closure=1;
  else
    dp.aggressive_junction_closure=0;
  dp.junctionp = compute_junctions();
  flp.min_fit_length_ = min_fit_length();
  flp.rms_distance_ = rms_distance();
  gfp.angle_tol_ = angle_tolerance();
  gfp.thresh_ = line_cnt_threshold();

  // create video process
  vpro_grid_finder_process* gfpro = new vpro_grid_finder_process(dp,flp,gfp);
  vpro_video_process_sptr gf_process = gfpro;
  // set output file
  vcl_stringstream output_file_stream;
  //output_file_stream << output_directory() << '/';
  output_file_stream << output_file();
  gfpro->set_output_file(output_file_stream.str());
  // set video process
  player.set_video_process(gf_process);
  // set input file
  player.load_video_file();//input_video_file());
  // set status block output file
  //player.set_status_output_file(status_block_file());

  vcl_cout << "playing video..\n";
  if (!player.play_video())
  {
    // error
    return 1;
  }
  vcl_cout << "...done.\n";
  player.print_performance_output("calibration video",gfpro->frame_scores_);
  vcl_cout << "wrote performance file\n";

  return 0;
}

