#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <bwm/bwm_observer_cam.h>
#include <bwm/bwm_observer_mgr.h>
#include <bwm/bwm_3d_corr.h>
#include <bwm/bwm_3d_corr_sptr.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vgl/vgl_point_3d.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>

#include <bwm/video/bwm_video_corr_processor.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_cam_istream.h>

#include <vpgl/algo/vpgl_triangulate_points.h>

#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_image_list_istream.h>

#include <vpgl/vpgl_fundamental_matrix.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

#include <ipts/ipts_draw.h>
#include <mvl/TriTensor.h>

// this executable takes a site file as input with a set of 2d image to image correspondences
// and computes a) pairwise Fundamental matrices b) triple-wise trifocal tensor for consecutive frames, then creates output images with colored correspondences such that
//   the ones that violate FM are red and the ones that satisfy FM are blue

unsigned fg_plane = 0; // draw onto red plane
unsigned bg_plane = 2; // draw onto blue plane

#include <vil/vil_view_as.h>

void compute_FM_constraint(std::vector<bwm_video_corr_sptr>& corrs,
                           std::vector<vpgl_perspective_camera<double> >& cams,
                           std::vector<vil_image_view<float> >& imgs, double thres, std::string& out_folder)
{
  // now for each frame pair, compute FM from camera matrices
  for (unsigned i = 1; i < imgs.size(); i++) {
    std::cout << " frame pair: [" << i-1 << ',' << i << "]\n";
    std::cout << "cam \n" << i-1 << cams[i-1] << "\n" << i << cams[i] << std::endl;

    // compute FM
    vpgl_fundamental_matrix<double> fm(cams[i-1], cams[i]);
    std::cout << "FM: \n" << fm << std::endl;

    // retrieve the corrs of this pair
    std::vector<std::pair<vgl_point_2d<double>, vgl_point_2d<double> > > points;
    std::vector<unsigned> points_planes;

    for (unsigned ii = 0; ii < corrs.size(); ii++)
    {
      bwm_video_corr_sptr corr = corrs[ii];
      std::map<unsigned, vgl_point_2d<double> > matches = corr->matches();
      std::pair<vgl_point_2d<double>, vgl_point_2d<double> > pt;
      bool found1 = false, found2 = false;
      for (std::map<unsigned, vgl_point_2d<double> >::iterator iter = matches.begin(); iter != matches.end(); iter++) {
        if (iter->first == i-1) {
          pt.first = iter->second; found1 = true; }
        else if (iter->first == i) {
          pt.second = iter->second; found2 = true; }
      }
      if (found1 && found2) {
        points.push_back(pt);

        // The residual for each correspondence is the sum of the squared distances from the points to their epipolar lines.
        vgl_homg_line_2d<double> lr = fm.r_epipolar_line( vgl_homg_point_2d<double>( pt.second ) );
        vgl_homg_line_2d<double> ll = fm.l_epipolar_line( vgl_homg_point_2d<double>( pt.first ) );
        double val = vgl_homg_operators_2d<double>::perp_dist_squared( lr, vgl_homg_point_2d<double>( pt.first ) )
                 + vgl_homg_operators_2d<double>::perp_dist_squared( ll, vgl_homg_point_2d<double>( pt.second ) );

        std::cout << "val: " << val << std::endl;
        if (val < thres)
          points_planes.push_back(fg_plane);
        else
          points_planes.push_back(bg_plane);
      }
    }

    // create the output images
    vil_image_view<vxl_byte> input_image_1, input_image_2;
    vil_convert_cast(imgs[i-1], input_image_1);
    vil_convert_cast(imgs[i], input_image_2);

    vil_image_view<vxl_byte> output_image_1_temp(input_image_1.ni(), input_image_1.nj(), 1);
    vil_convert_planes_to_grey(input_image_1,output_image_1_temp);
    vil_image_view<vxl_byte> output_image_2_temp(input_image_2.ni(), input_image_2.nj(), 1);
    vil_convert_planes_to_grey(input_image_2, output_image_2_temp);

    vil_image_view<vxl_byte> output_image_1(input_image_1.ni(), input_image_1.nj(), 3);
    vil_image_view<vxl_byte> output_image_2(input_image_2.ni(), input_image_2.nj(), 3);
    vil_image_view<vxl_byte> output_image_1_r = vil_plane(output_image_1, 0);
    vil_image_view<vxl_byte> output_image_2_r = vil_plane(output_image_2, 0);
    vil_image_view<vxl_byte> output_image_1_g = vil_plane(output_image_1, 1);
    vil_image_view<vxl_byte> output_image_2_g = vil_plane(output_image_2, 1);
    vil_image_view<vxl_byte> output_image_1_b = vil_plane(output_image_1, 2);
    vil_image_view<vxl_byte> output_image_2_b = vil_plane(output_image_2, 2);
    output_image_1_r.deep_copy(output_image_1_temp);
    output_image_2_r.deep_copy(output_image_2_temp);
    output_image_1_g.deep_copy(output_image_1_temp);
    output_image_2_g.deep_copy(output_image_2_temp);
    output_image_1_b.deep_copy(output_image_1_temp);
    output_image_2_b.deep_copy(output_image_2_temp);

    //output_image_1.deep_copy(input_image_1);
    //output_image_2.deep_copy(input_image_2);

    for (unsigned k = 0; k < points.size(); k++) {
      vil_image_view<vxl_byte> output_img1_r = vil_plane(output_image_1, points_planes[k]);
      vil_image_view<vxl_byte> output_img2_r = vil_plane(output_image_2, points_planes[k]);

      int ii = int((points[k].first).x() + 0.5);
      int jj = int((points[k].first).y() + 0.5);
      if (ii >= 0 && jj >= 0 && ii < (int)output_img1_r.ni() && jj < (int)output_img1_r.nj()) {
        ipts_draw_cross(output_image_1_r, ii, jj, 5, vxl_byte(0) );
        ipts_draw_cross(output_image_1_g, ii, jj, 5, vxl_byte(0) );
        ipts_draw_cross(output_image_1_b, ii, jj, 5, vxl_byte(0) );

        ipts_draw_cross(output_img1_r, ii,jj,5,vxl_byte(255) );
      }
      ii = int((points[k].second).x() + 0.5);
      jj = int((points[k].second).y() + 0.5);
      if (ii >= 0 && jj >= 0 && ii < (int)output_img2_r.ni() && jj < (int)output_img2_r.nj()) {
        ipts_draw_cross(output_image_2_r, ii, jj, 5, vxl_byte(0) );
        ipts_draw_cross(output_image_2_g, ii, jj, 5, vxl_byte(0) );
        ipts_draw_cross(output_image_2_b, ii, jj, 5, vxl_byte(0) );

        ipts_draw_cross(output_img2_r, ii,jj,5,vxl_byte(255) );
      }
    }

    std::stringstream str; str << out_folder << "out_t_" << thres << "_pair_" << i-1 << "_" << i << "img1.png";
    std::cout << "saving : " << str.str() << '\n';
    vil_save(output_image_1, str.str().c_str());
    std::stringstream str2; str2 << out_folder << "out_t_" << thres << "_pair_" << i-1 << "_" << i << "img2.png";
    vil_save(output_image_2, str2.str().c_str());
  }
}

void compute_TFT_constraint(std::vector<bwm_video_corr_sptr>& corrs,
                            std::vector<vpgl_perspective_camera<double> >& cams,
                            std::vector<vil_image_view<float> >& imgs, double thres, std::string& out_folder)
{
  // now for each frame triplet, compute TFT from camera matrices
  for (unsigned i = 2; i < imgs.size(); i++) {
    std::cout << " frame triplet: [" << i-2 << ',' << i-1 << ',' << i << "]\n";

    // compute TFT using oxl/mvl library
    PMatrix C1(cams[i-2].get_matrix());
    PMatrix C2(cams[i-1].get_matrix());
    PMatrix C3(cams[i].get_matrix());
    TriTensor TFT(C1, C2, C3);

    std::cout << "TFT: \n" << TFT << std::endl;

    // retrieve the corrs of this pair
    std::vector< std::vector< vgl_point_2d<double> > > points;
    std::vector<unsigned> points_planes;

    for (unsigned ii = 0; ii < corrs.size(); ii++)
    {
      bwm_video_corr_sptr corr = corrs[ii];
      std::map<unsigned, vgl_point_2d<double> > matches = corr->matches();
      vgl_point_2d<double> pt1;
      vgl_point_2d<double> pt2;
      vgl_point_2d<double> pt3;
      bool found1 = false, found2 = false, found3 = false;
      for (std::map<unsigned, vgl_point_2d<double> >::iterator iter = matches.begin(); iter != matches.end(); iter++) {
        if (iter->first == i-2) {
          pt1 = iter->second; found1 = true; }
        else if (iter->first == i-1) {
          pt2 = iter->second; found2 = true; }
        else if (iter->first == i) {
          pt3 = iter->second; found3 = true; }
      }
      if (found1 && found2 && found3) {
        std::vector< vgl_point_2d<double> > pts;
        pts.push_back(pt1);
        pts.push_back(pt2);
        pts.push_back(pt3);
        points.push_back(pts);

        //vgl_homg_point_2d<double> pt3_r = TFT.image3_transfer_qd(vgl_homg_point_2d<double>(pt1), vgl_homg_point_2d<double>(pt2));
        //double min_dist = vgl_homg_operators_2d<double>::distance_squared(pt3_r, vgl_homg_point_2d<double>(pt3));

        vgl_homg_point_2d<double> pt2_r = TFT.image2_transfer_qd(vgl_homg_point_2d<double>(pt1), vgl_homg_point_2d<double>(pt3));
        double min_dist = vgl_homg_operators_2d<double>::distance_squared(pt2_r, vgl_homg_point_2d<double>(pt2));
        std::cout << "min_dist: " << min_dist << std::endl;

        if (min_dist < thres)
          points_planes.push_back(fg_plane);
        else
          points_planes.push_back(bg_plane);
      }
    }

    // create the output images
    vil_image_view<vxl_byte> input_image_1, input_image_2, input_image_3;
    vil_convert_cast(imgs[i-2], input_image_1);
    vil_convert_cast(imgs[i-1], input_image_2);
    vil_convert_cast(imgs[i], input_image_3);

    vil_image_view<vxl_byte> output_image_1(input_image_1.ni(), input_image_1.nj(), 3);
    vil_image_view<vxl_byte> output_image_2(input_image_2.ni(), input_image_2.nj(), 3);
    vil_image_view<vxl_byte> output_image_3(input_image_3.ni(), input_image_3.nj(), 3);
    output_image_1.deep_copy(input_image_1);
    output_image_2.deep_copy(input_image_2);
    output_image_3.deep_copy(input_image_3);

    for (unsigned k = 0; k < points.size(); k++) {
      vil_image_view<vxl_byte> output_img1_r = vil_plane(output_image_1, points_planes[k]);
      vil_image_view<vxl_byte> output_img2_r = vil_plane(output_image_2, points_planes[k]);
      vil_image_view<vxl_byte> output_img3_r = vil_plane(output_image_3, points_planes[k]);

      int ii = int((points[k][0]).x() + 0.5);
      int jj = int((points[k][0]).y() + 0.5);
      if (ii >= 0 && jj >= 0 && ii < (int)output_img1_r.ni() && jj < (int)output_img1_r.nj())
        ipts_draw_cross(output_img1_r, ii,jj,5,vxl_byte(255) );
      ii = int((points[k][1]).x() + 0.5);
      jj = int((points[k][1]).y() + 0.5);
      if (ii >= 0 && jj >= 0 && ii < (int)output_img2_r.ni() && jj < (int)output_img2_r.nj())
        ipts_draw_cross(output_img2_r, ii,jj,5,vxl_byte(255) );
      ii = int((points[k][2]).x() + 0.5);
      jj = int((points[k][2]).y() + 0.5);
      if (ii >= 0 && jj >= 0 && ii < (int)output_img3_r.ni() && jj < (int)output_img3_r.nj())
        ipts_draw_cross(output_img3_r, ii,jj,5,vxl_byte(255) );
    }

    std::stringstream str; str << out_folder << "out_triplet_" << i-2 << "_" << i-1 << "_" << i << "img1.png";
    std::cout << "saving : " << str.str() << '\n';
    vil_save(output_image_1, str.str().c_str());
    std::stringstream str2; str2 << out_folder << "out_triplet_" << i-2 << "_" << i-1 << "_" << i << "img2.png";
    vil_save(output_image_2, str2.str().c_str());
    std::stringstream str3; str3 << out_folder << "out_triplet_" << i-2 << "_" << i-1 << "_" << i << "img3.png";
    vil_save(output_image_3, str3.str().c_str());

  }

}


int main(int argc, char** argv)
{
  //Get Inputs

  vul_arg<std::string> site_file   ("-site", "site file",  "");
  vul_arg<std::string> out_folder   ("-out", "out folder",  "");
  vul_arg<double> thres ("-t", "threshold to declare foreground/background in pixels", 1.0);

  if (argc < 5) {
    std::cout << "usage: bwm_triangulate_2d_corrs -site <site file> -out <folder to save output> -t <threshold to declare fg/bg in pixels>\n";
    return -1;
  }
  vul_arg_parse(argc, argv);

  bwm_video_corr_processor cp;
  cp.set_verbose(true);
  if (!cp.open_video_site(site_file().c_str(), true))
    return -1;
  std::string cam_path = cp.camera_path();
  bwm_video_cam_istream_sptr cstr = new bwm_video_cam_istream(cam_path);

  std::vector<vpgl_perspective_camera<double> > cams;

  do {
    vpgl_perspective_camera<double>* cam = cstr->read_camera();
    if (cam) {
      cams.push_back(*cam);
    }
    else
      break;
  } while (true);

  std::cout << "found: " << cams.size() << std::endl;
  std::vector<bwm_video_corr_sptr> corrs = cp.correspondences();
  std::cout << "there are: " << corrs.size() << " corrs in the file\n";

  std::cout << "reading images from stream: " << cp.video_path() << '\n';

  std::vector<vil_image_view<float> > imgs;
  unsigned max_frame = 5;
  unsigned cnt;
  for (cnt = 0; cnt < max_frame; cnt++) {
    vil_image_view<float> temp;
    if (!cp.frame_at_index(cnt, temp))
      break;
    imgs.push_back(temp);
  }
  unsigned frame_cnt = cnt;

  std::cout << " retrieved " << frame_cnt << " frames from the stream: " << cp.video_path() << std::endl;

  compute_FM_constraint(corrs, cams, imgs, thres(), out_folder());
  //compute_TFT_constraint(corrs, cams, imgs, thres(), out_folder());


  return 0;
}
