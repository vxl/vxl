#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
#include <bapl/bapl_connectivity.h>
#include <bapl/bapl_connectivity_sptr.h>
#include <bapl/bapl_lowe_keypoint_sptr.h>
#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_keypoint_sptr.h>
#include <bapl/bapl_keypoint_set.h>

#include <vul/vul_awk.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_compute_tracks(int argc, char* argv[])
{
  std::string dir_base;

  if ( argc >= 2 ) {
    dir_base = argv[1];
#ifdef _WIN32
    dir_base += "\\";
#else
    dir_base += "/";
#endif
  }

  // load keypoints
  std::vector<bapl_keypoint_set_sptr> img_keypoints;
  std::vector<int> number_of_keypoints;

  unsigned num_imgs = 11;
  number_of_keypoints.push_back(1251);  // kermit
  number_of_keypoints.push_back(1303);
  number_of_keypoints.push_back(1236);
  number_of_keypoints.push_back(1218);
  number_of_keypoints.push_back(1105);
  number_of_keypoints.push_back(1159);
  number_of_keypoints.push_back(950);
  number_of_keypoints.push_back(1106);
  number_of_keypoints.push_back(1271);
  number_of_keypoints.push_back(1159);
  number_of_keypoints.push_back(1122);
#if 0
  //unsigned num_imgs = 21;
  unsigned num_imgs = 5;
  int img_width = 480, img_height = 280;

  number_of_keypoints.push_back(1347);  // beijing
  number_of_keypoints.push_back(1321);
  number_of_keypoints.push_back(1294);
  number_of_keypoints.push_back(1270);
  number_of_keypoints.push_back(1292);
  number_of_keypoints.push_back(1321);
  number_of_keypoints.push_back(1305);
  number_of_keypoints.push_back(1320);
  number_of_keypoints.push_back(1335);
  number_of_keypoints.push_back(1383);
  number_of_keypoints.push_back(1415);
  number_of_keypoints.push_back(1391);
  number_of_keypoints.push_back(1422);
  number_of_keypoints.push_back(1433);
  number_of_keypoints.push_back(1431);
  number_of_keypoints.push_back(1472);
  number_of_keypoints.push_back(1463);
  number_of_keypoints.push_back(1464);
  number_of_keypoints.push_back(1477);
  number_of_keypoints.push_back(1517);
  number_of_keypoints.push_back(1535);
#endif // 0

  bapl_conn_table_sptr table = new bapl_conn_table(num_imgs);

  // create dummy keypoint vectors of the given sizes
  for (unsigned i = 0; i < num_imgs; i++) {
    std::vector< bapl_keypoint_sptr > keypoints;
    int n = number_of_keypoints[i];
    for (int j = 0; j < n; j++) {
      bapl_lowe_keypoint_sptr kp = new bapl_lowe_keypoint;
      kp->set_id(j);
      keypoints.push_back(kp);
    }
    bapl_keypoint_set_sptr key_set = new bapl_keypoint_set(keypoints);
    img_keypoints.push_back(key_set);
    table->add_image_data(i, key_set);
  }

#if 0
  // load keypoint vectors

  int j = 0;
  for (unsigned i = 0; i < num_imgs; i++) {
    std::vector< bapl_keypoint_sptr > keypoints;
    std::stringstream ii; ii.width(5); ii.fill('0'); ii << (360 + j);
    j += 5;
    std::string name = dir_base + "imgs/keys/videoframe" + ii.str() + ".key";
    std::cout << "name: " << name << std::endl;
    std::ifstream ifs(name.c_str());
    int n; ifs >> n; int len; ifs >> len;
    for (int j = 0; j < n; j++) {
      bapl_lowe_keypoint_sptr kp = read_from_file(ifs, len, img_width, img_height);
      keypoints.push_back(kp);
    }
    ifs.close();
    for (unsigned j = 0; j < keypoints.size(); j++)
      keypoints[j]->set_id(j);
    bapl_keypoint_set_sptr key_set = new bapl_keypoint_set(keypoints);
    img_keypoints.push_back(key_set);
    table->add_image_data(i, key_set);
  }


  // load init full match table
  std::string name_i = dir_base + "matches.init.txt";
  std::ifstream ifsi( name_i.c_str() );
  if (!ifsi)
    return;
  while (!ifsi.eof()) {
    std::vector<bapl_key_match> matches;
    // read the match file
    int id1, id2; ifsi >> id1; ifsi >> id2; int cnt; ifsi >> cnt;
    std::cout << "Reading " << cnt << " matches for images: " << id1 << " and " << id2 << std::endl;
    for (int i = 0; i < cnt; i++) {
      int kid1, kid2;
      ifsi >> kid1; ifsi >> kid2;
      if (kid1 >= (int)img_keypoints[id1]->keys_.size() || kid2 >= (int)img_keypoints[id2]->keys_.size()) {
        std::cout << "In bapl_load_match_process() -- Mismatch in keypoint vector size for: " << id1 << " or for: " << id2 << " and the match file!\n";
        return;
      }
      bapl_key_match m(img_keypoints[id1]->keys_[kid1], img_keypoints[id2]->keys_[kid2]);
      matches.push_back(m);
    }
    std::cout << id1 << ' ' << id2 << " n_matches: " << matches.size();
    bapl_keypoint_match_set::prune_spurious_matches(matches);
    std::cout << "after pruning n_matches: " << matches.size() << '\n';
    if (matches.size() >= 16) {
      bapl_keypoint_match_set_sptr ms = new bapl_keypoint_match_set(id1, id2, matches);
      std::vector<bapl_key_match> refined_matches;
      ms->refine_matches(25.0f, refined_matches);
      if (refined_matches.size() >= 16) {
        bapl_keypoint_match_set_sptr ms_refined = new bapl_keypoint_match_set(id1, id2, refined_matches);
        table->add(ms_refined);
      }
      //table->add(ms);
    }
  }
#endif // 0


  // load pruned match table
  std::string name = dir_base + "matches_with_ids.txt";
  //std::string name = dir_base + "matches.ransac_with_ids.txt";
  std::ifstream ifs( name.c_str() );
  std::vector<std::string> paths;
  vul_awk awk_p(ifs);
  for (; awk_p; ++awk_p)
  {
    std::string file_name = awk_p.line();
    //check for empty lines so that and empty line at the end of file won't cause error
    if (file_name.empty())
      continue;
    paths.push_back(file_name);
  }
  std::cout << "read: " << paths.size() << " lines!\n";
  ifs.close();
  for (const auto & path : paths) {
    std::stringstream line(path);
    int left_id, right_id; line >> left_id; line >> right_id;
    std::vector<bapl_key_match> matches;
    while (!line.eof()) {
      int kid1, kid2; line >> kid1; line >> kid2;
      bapl_key_match km; km.first = img_keypoints[left_id]->keys_[kid1]; km.second = img_keypoints[right_id]->keys_[kid2];
      matches.push_back(km);
    }
    std::cout << left_id << ' ' << right_id << " n_matches: " << matches.size() << std::endl;
    bapl_keypoint_match_set_sptr ms = new bapl_keypoint_match_set(left_id, right_id, matches);
    table->add(ms);
  }

  table->print_table();

  // now make the match table symmetric
  table->make_symmetric();
  table->print_table();

  //table->print_table_with_matches();

  // now find the tracks
  std::vector<bapl_track_data> tracks;
  if (table->compute_tracks(tracks))
    std::cout << "Success..!\n";
  else
    std::cout << "Track computation failed!\n";

  std::string out_file = "tracks_bapl.xml";
  std::ofstream ofs(out_file.c_str());
  print_tracks(ofs, tracks, 0, 0);

  TEST("Found Track Size",tracks.size(),677);


  //table->print_table_with_matches();
}

TESTMAIN_ARGS(test_compute_tracks);
