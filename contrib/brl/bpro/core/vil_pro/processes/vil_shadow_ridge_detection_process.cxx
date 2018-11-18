// This is brl/bpro/core/vil_pro/processes/vil_shadow_ridge_detection_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief This process detects shadow ridges from a detected shadow image.
// Shadow ridges are occluding contours from sun's viewpoint.

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_binary_opening.h>
#include <vil/algo/vil_blob.h>
#include <vnl/vnl_math.h>

namespace vil_shadow_ridge_detection_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 3;

  float dist_angles(float a,float b)
  {
    float dist = std::fabs((b-a));
    return dist < float(vnl_math::pi) ? dist : float(vnl_math::twopi) - dist;
  }
}

//: Init function
bool vil_shadow_ridge_detection_process_cons(bprb_func_process& pro)
{
  using namespace vil_shadow_ridge_detection_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "int"; // blob size ( 100 pixels or so)
  input_types_[2] = "float"; // -0.75*vnl_math::pi

  // process has 3 outputs
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr"; // Shadow  Regions
  output_types_[1] = "vil_image_view_base_sptr"; // Shadow Ridges
  output_types_[2] = "vil_image_view_base_sptr"; // Length of Shadows

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

//: Execute the process
bool vil_shadow_ridge_detection_process(bprb_func_process& pro)
{
  using namespace vil_shadow_ridge_detection_process_globals;

  // Sanity check
  if (pro.n_inputs()< 3) {
    std::cout << "vil_shadow_ridge_detection_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr out_img = pro.get_input<vil_image_view_base_sptr>(i++);
  auto blob_size = pro.get_input<unsigned int>(i++);
  auto sun_angle = pro.get_input<float>(i++);

  if (out_img->nplanes() != 1)
  {
    std::cout<<"Input needs to be a bool image" << std::endl;
    return false;
  }

  vil_image_view<bool> open_img(out_img->ni(),out_img->nj());
  vil_image_view<bool> close_img(out_img->ni(),out_img->nj());
  vil_structuring_element s;
  s.set_to_disk(2.0);
  vil_binary_opening(out_img,open_img,s);
  s.set_to_disk(3.0);
  vil_binary_dilate(open_img,close_img,s);

  auto * conn_region= new vil_image_view<unsigned>(out_img->ni(),out_img->nj());
  auto * conn_thresh_region= new vil_image_view<unsigned>(out_img->ni(),out_img->nj());
  conn_thresh_region->fill(0);
  auto * conn_edge = new vil_image_view<unsigned>(out_img->ni(),out_img->nj());
  vil_blob_labels(close_img,vil_blob_8_conn,*conn_region);

  std::vector<vil_blob_pixel_list> dest_pixel_lists;
  vil_blob_labels_to_pixel_lists(*conn_region,dest_pixel_lists);

  unsigned count = 1;
  for (auto & dest_pixel_list : dest_pixel_lists)
  {
    if (dest_pixel_list.size() > blob_size)
    {
      vil_blob_pixel_list::iterator iter;
      for (iter=dest_pixel_list.begin(); iter!=dest_pixel_list.end();iter++)
        (*conn_thresh_region)(iter->first,iter->second) = count;
      count++;
    }
  }

  std::cout<<"Selected "<<count <<" out of "<<dest_pixel_lists.size()<<std::endl;
  vil_blob_labels_to_edge_labels(*conn_thresh_region,vil_blob_8_conn,*conn_edge);

  std::vector<vil_blob_pixel_list> blob_edge_pixel_lists;
  vil_blob_labels_to_pixel_lists(*conn_edge,blob_edge_pixel_lists);

  auto * shadow_border_edge = new vil_image_view<float>(out_img->ni(),out_img->nj());
  shadow_border_edge->fill(10.0);
  auto * shadow_border_edge_thresholded = new vil_image_view<bool>(out_img->ni(),out_img->nj());
  auto * shadow_border_dist= new vil_image_view<float>(out_img->ni(),out_img->nj());
  shadow_border_dist->fill(-1.0f);

  for (auto & blob_edge_pixel_list : blob_edge_pixel_lists)
  {
    vil_blob_pixel_list::iterator iter;
    for (unsigned l = 0 ; l < blob_edge_pixel_list.size(); l++)
    {
      double min = vnl_math::twopi;
      float min_r = 0.0f;
      for (unsigned m = 0 ; m < blob_edge_pixel_list.size(); m++)
      {
        if (l!=m)
        {
          float dx = (float)blob_edge_pixel_list[m].first - (float)blob_edge_pixel_list[l].first;
          float dy = (float)blob_edge_pixel_list[m].second -  (float)blob_edge_pixel_list[l].second;
          float angle = std::atan2(dy,dx);

          float r = dx*dx+dy*dy;
          if (r > 16.0)
          {
            if (min > dist_angles(sun_angle , angle))
            {
              min = dist_angles(sun_angle, angle);
              min_r = r;
            }
          }
        }
      }
      (*shadow_border_edge)(blob_edge_pixel_list[l].first,blob_edge_pixel_list[l].second) = (float)min;
      if (min < 0.04)
      {
        (*shadow_border_edge_thresholded)(blob_edge_pixel_list[l].first,blob_edge_pixel_list[l].second) = true;
        (*shadow_border_dist)(blob_edge_pixel_list[l].first,blob_edge_pixel_list[l].second) = std::sqrt(min_r);
      }
    }
  }

  auto * shadow_border_conn_region= new vil_image_view<unsigned>(out_img->ni(),out_img->nj());
  auto * shadow_border_conn_thresh_region= new vil_image_view<unsigned char>(out_img->ni(),out_img->nj());
  auto * shadow_border_dist_thresholded= new vil_image_view<float>(out_img->ni(),out_img->nj());
  shadow_border_dist_thresholded->fill(-1.0f);
  shadow_border_conn_thresh_region->fill(0);

  vil_blob_labels(*shadow_border_edge_thresholded,vil_blob_8_conn,*shadow_border_conn_region);
  std::vector<vil_blob_pixel_list> shadow_edge_dest_pixel_lists;
  vil_blob_labels_to_pixel_lists(*shadow_border_conn_region,shadow_edge_dest_pixel_lists);
  count = 1;
  for (auto & shadow_edge_dest_pixel_list : shadow_edge_dest_pixel_lists)
  {
    if (shadow_edge_dest_pixel_list.size() > 25)
    {
      vil_blob_pixel_list::iterator iter;
      for (iter=shadow_edge_dest_pixel_list.begin(); iter!=shadow_edge_dest_pixel_list.end();iter++)
      {
        (*shadow_border_conn_thresh_region)(iter->first,iter->second) = count;
        (*shadow_border_dist_thresholded)(iter->first,iter->second) = (*shadow_border_dist)(iter->first,iter->second);
      }
      count++;
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, conn_thresh_region);
  pro.set_output_val<vil_image_view_base_sptr>(1, shadow_border_conn_thresh_region);
  pro.set_output_val<vil_image_view_base_sptr>(2, shadow_border_dist_thresholded);
  return true;
}
