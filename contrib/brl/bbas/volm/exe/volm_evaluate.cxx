//:
// \file
// \brief executable to evaluate output tiles using the ground truth locations
//        given a test image id, for all the available output prob maps,
//                                  find the number of pixels within/intersect the candidate list
//                                                            below the given threshold, and report the value at the gt loc pixel
//        in addition:
//               read the geo index, count the number of total hyps
//                                   count the number of hyps in the leaves that are within or intersect the candidate list
//               read the intermediate output binary files, count the number of total hyps which are below the given score threshold
//                                                          count the number of total cams which are below the given score threshold
//               report % of search space that is reduced
//               report % of cam space that is reduced
//
// \author Ozge C. Ozcanli
// \date Jan 28, 2013

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_camera_space.h>
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <bkml/bkml_parser.h>

// generate gt hypos
int main(int argc,  char** argv)
{
  vul_arg<vcl_string> gt_file("-gt_locs", "file with the gt locs of all test cases", "");
  vul_arg<unsigned> id("-id", "id of the test image", 40);
  vul_arg<vcl_string> out("-out", "folder which has the output tiles", "");
  vul_arg<unsigned> thres("-t", "output prob map threshold to count # of pixels less than", 127);
  vul_arg<vcl_string> local_folder("-loc_out", "local output folder where the intermediate score files are stored", "");
  vul_arg<unsigned> pass_id("-pass", "e.g. 0 to evaluate binary score output of pass 0 in the local output folder", 0);
  vul_arg<float> cam_threshold("-ct", "score threshold to count # of cameras below at each loc", 0.8f);
  vul_arg_parse(argc, argv);

  vcl_cout << "argc: " << argc << vcl_endl;
  if (out().compare("") == 0 || local_folder().compare("") == 0 || gt_file().compare("") == 0) {
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  unsigned threshold = thres();

  vsl_b_ifstream ifs(local_folder() + "camera_space.bin");
  volm_camera_space_sptr camera_space = new volm_camera_space;
  camera_space->b_read(ifs);
  ifs.close();
  vcl_cout << " READ camera space, number of cams: " << camera_space->valid_indices().size() << vcl_endl;

  vcl_vector<vcl_pair<vgl_point_3d<double>, vcl_pair<vcl_pair<vcl_string, int>, vcl_string> > > samples;
  unsigned int cnt = volm_io::read_gt_file(gt_file(), samples);
  if (id() >= cnt) {
    vcl_cerr << "the file: " << gt_file() << " does not contain test id: " << id() << "!\n";
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  else
    vcl_cout << "evaluating using gt id: " << id() << " with name: " << samples[id()].second.first.first << " type: " << samples[id()].second.second << " lat: " << samples[id()].first.y() << " lon: " << samples[id()].first.x() << " elev: " << samples[id()].first.z() << '\n';

  vcl_cout << "\n==================================================================================================\n"
           << "\t\t  Evaluate output prob maps using pixel value threshold: " << threshold << '\n'
           << "==================================================================================================\n" << vcl_endl;


  // read the output tiles
  unsigned cnt_below = 0;
  unsigned tot_pix_count = 0;
  unsigned tot_pix_unevaluated = 0;
  vcl_vector<volm_tile> tiles;
  if (samples[id()].second.second == "desert")
    tiles = volm_tile::generate_p1_wr1_tiles();
  else
    tiles = volm_tile::generate_p1_wr2_tiles();
  vcl_cout << " generated " << tiles.size() << " tiles for type: " << samples[id()].second.second << '\n';

  for (unsigned i = 0; i < tiles.size(); i++)
  {
    vcl_string name = out() + "/" + "ProbMap_" + tiles[i].get_string() + ".tif";
    if (!vul_file::exists(name))
    {
      vcl_cerr << " missing:\t " << name << "!\n";
      continue;
    }
    else
      vcl_cout << " adding:\t " << name << " to evaluation!\n";

    vil_image_view<vxl_byte> tile = vil_load(name.c_str());
    for (unsigned ii = 0; ii < tile.ni(); ii++)
      for (unsigned jj = 0; jj < tile.nj(); jj++)
      {
        double tlat, tlon;
        tiles[i].img_to_global(ii, jj, tlon, tlat);
        if (tile(ii, jj) == 0)
          tot_pix_unevaluated++;
        else
        {
          tot_pix_count++;
          if (tile(ii, jj) < threshold)
            cnt_below++;
        }
      }

    unsigned u, v;
    tiles[i].global_to_img(samples[id()].first.x(),  samples[id()].first.y(), u, v);
    if (u < tile.ni() && v < tile.nj())
      vcl_cout << "\t GT location: " << samples[id()].first.x() << ", " << samples[id()].first.y() << " is at pixel: " << u << ", " << v << " and has value: " << (int)tile(u, v) << vcl_endl;
  }
  vcl_cout << "tot pixels unevaluated: " << tot_pix_unevaluated << '\n'
           << "tot pixels evaluated: " << tot_pix_count << '\n'
           << "tot pixels below threshold " << threshold << ": " << cnt_below << '\n'
           << "so knocked out " << (float)cnt_below/tot_pix_count*100 << " percent of the evaluated ROI!\n\n"

           << "\n==================================================================================================\n"
           << "\t\t  Evaluate output score binaries using camera score threshold: " << cam_threshold() << '\n'
           << "==================================================================================================\n" << vcl_endl;

  unsigned cnt_hyp_below = 0;
  unsigned tot_hyp_count = 0;
  int cnt_cam_below = 0;
  unsigned total_cam_in_file = 0;
  int total_cams = camera_space->valid_indices().size();
  // read the scores, assumes that score binaries for a given tile fits into memory
  for (unsigned i = 0; i < tiles.size(); i++) {
    vcl_stringstream score_file_name; score_file_name << local_folder() << "pass" << pass_id() << "_scores_tile_" << i << ".bin";
    if (!vul_file::exists(score_file_name.str()))
    {
      vcl_cerr << " missing:\t " << score_file_name.str() << "!\n";
      continue;
    }
    else
      vcl_cout << " adding:\t " << score_file_name.str() << " to evaluation!\n";

    vcl_cout << " \t\t size of the file is: " << (float)(vul_file::size(score_file_name.str()))/(1024.0*1024.0*1024.0) << " Gb, assuming it fits memory!!\n";
    vcl_vector<volm_score_sptr> scores;
    volm_score::read_scores(scores, score_file_name.str());  // this file may be too large, make sure it fits to memory!!
    tot_hyp_count += scores.size();
    for (unsigned j = 0; j < scores.size(); j++) {
      //vcl_cout << "leaf id: " << scores[j]->leaf_id_ << " hyp_id: " << scores[j]->hypo_id_ << " max score: " << scores[j]->max_score_ << " # of cams: " << scores[j]->cam_id_.size() << '\n';
      total_cam_in_file += scores[j]->cam_id_.size();
      if (scores[j]->max_score_ < cam_threshold()) {
        cnt_hyp_below++;
        cnt_cam_below += total_cams;  // all the cams are eliminated for this position then
      }
      else
        cnt_cam_below += (total_cams - scores[j]->cam_id_.size());
    }
  }

  vcl_cout << "total # of hypotheses in the output score file: " << tot_hyp_count << vcl_endl
           << "total # of hypotheses below threshold (" << cam_threshold() << "): " << cnt_hyp_below << vcl_endl
           << "  so previous pass knocked out: " << ((float)cnt_hyp_below/tot_hyp_count)*100.0 << " percent of locations!\n"
           << "total # of cameras (total hyp*cam space size): " << tot_hyp_count*total_cams << vcl_endl
           << "total # of cameras saved in score files: " << total_cam_in_file << vcl_endl
           << "so total # of cameras knocked out: " << cnt_cam_below << vcl_endl
           << "  so previous pass knocked out: " << ((float)cnt_cam_below/(tot_hyp_count*total_cams))*100.0 << " percent of cameras!\n"
           << "\n==================================================================================================\n"
           << "==================================================================================================\n" << vcl_endl;

  return volm_io::SUCCESS;
}

