#include <testlib/testlib_test.h>
#include <bapl/bapl_bbf_tree.h>
#include <bapl/bapl_lowe_keypoint_sptr.h>
#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_keypoint_sptr.h>
#include <bapl/bapl_keypoint_set.h>
#include <bapl/bapl_keypoint_set_sptr.h>

#include <vil/vil_plane.h>
#include <vil/vil_image_view.h>
#include <ipts/ipts_draw.h>

#include <vul/vul_timer.h>
#include <bpgl/algo/bpgl_fm_compute_ransac.h>

void draw_keypoint(vil_image_view<vxl_byte>& img, const bapl_keypoint_sptr& k)
{
  bapl_lowe_keypoint_sptr kp;
  kp.vertical_cast(k);
  vil_image_view<vxl_byte> output_img_r = vil_plane(img, 0);
  vil_image_view<vxl_byte> output_img_g = vil_plane(img, 1);
  vil_image_view<vxl_byte> output_img_b = vil_plane(img, 2);
  int ii = int(kp->location_i()+0.5); int jj = int(kp->location_j()+0.5);
  if (ii >= 0 && jj >= 0 && ii < (int)img.ni() && jj < (int)img.nj()) {
    ipts_draw_cross(output_img_r, ii, jj, int(kp->scale()+0.5), vxl_byte(255));
    ipts_draw_cross(output_img_g, ii, jj, int(kp->scale()+0.5), vxl_byte(255));
    ipts_draw_cross(output_img_b, ii, jj, int(kp->scale()+0.5), vxl_byte(0));
  }
}

static void test_match_keypoints(int argc, char* argv[])
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

  std::string name1 = dir_base + "frame000.key";
  std::string name2 = dir_base + "frame001.key";

  std::vector< bapl_keypoint_sptr > keypoints1, keypoints2;
  std::ifstream ifs1(name1.c_str());
  if (!ifs1.is_open()) {
    return;
  }
  std::ifstream ifs2(name2.c_str());
  if (!ifs2.is_open()) {
    return;
  }
  int n; ifs1 >> n; int len; ifs1 >> len;
  for (int i = 0; i < n; i++) {
    bapl_lowe_keypoint_sptr kp = read_from_file(ifs1, len);
    keypoints1.push_back(kp);
  }
  ifs1.close();
  ifs2>>n; ifs2>>len;
  for (int i = 0; i < n; i++) {
    bapl_lowe_keypoint_sptr kp = read_from_file(ifs2, len);
    keypoints2.push_back(kp);
  }
  // set ids of the keypoints as the order they're read from the file
  for (unsigned i = 0; i < keypoints1.size(); i++) {
    keypoints1[i]->set_id(i);
  }
  for (unsigned i = 0; i < keypoints2.size(); i++) {
    keypoints2[i]->set_id(i);
  }
  std::cout << "loaded: " << keypoints1.size() << " keypoints in the first set!\n";
  std::cout << "loaded: " << keypoints2.size() << " keypoints in the second set!\n";

  vul_timer t;

  bapl_bbf_tree bbf(keypoints2, 16);  // create a kd-tree for features of J (second image)

  std::vector<bapl_key_match> matches;

  //for (unsigned i=0; i<keypoints1.size(); ++i) {  // for each feature in I (first image)
  for (unsigned i=0; i<1; ++i) {  // for each feature in I (first image)
    bapl_keypoint_sptr query = keypoints1[i];
    std::vector<bapl_keypoint_sptr> match;
    //bbf.n_nearest(query, match, 2, 200);       // find its two nearest neighbors, 200 is parameter value used in bundler package
    bbf.n_nearest(query, match, 2, 1);       // limit the depth of the search
    if ( vnl_vector_ssd(query->descriptor(),match[0]->descriptor()) <
        vnl_vector_ssd(query->descriptor(),match[1]->descriptor())*.6*.6) {   // 0.6 is parameter value used in bundler package
      bapl_key_match k_p(query, match[0]);
      matches.push_back(k_p);                      // accepted match
    }
  }

  std::cout << "Found: " << matches.size() << " matches!\n";

  std::vector<bapl_key_match> matches_pruned(matches);
  bapl_keypoint_match_set::prune_spurious_matches(matches_pruned);
  std::cout << "After pruning found: " << matches_pruned.size() << '\n';
  bapl_keypoint_match_set_sptr ms = new bapl_keypoint_match_set(0, 1, matches_pruned);

  // refine matches
  std::vector<bapl_key_match> matches_refined;
  ms->refine_matches(9.0f, matches_refined);

  std::cout << "After refinement found: " << matches_refined.size() << '\n';
}

TESTMAIN_ARGS(test_match_keypoints)
