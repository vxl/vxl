#include "bvxm_generate_edge_map_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <sdet/sdet_detector.h>
#include <sdet/sdet_fit_lines.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vil/vil_new.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d.h>

bvxm_generate_edge_map_process::bvxm_generate_edge_map_process()
{
  // process takes 1 input: 
  //input[0]: input grayscale image
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0] = "vil_image_view_base_sptr";

  // process has 1 output:
  // output[0]: output edge image
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0] = "vil_image_view_base_sptr";

  // adding parameters
  parameters()->add("parameter specifying weather to use lines instead of edges", "use_lines", false);
  parameters()->add("gaussian sigma for the edge detection process", "edge_detection_gaussian_sigma", 2.0);
  parameters()->add("minimum line length for the line detection process", "line_fitting_min_length", 10.0);
  parameters()->add("error tolerance for the line detection process", "line_fitting_error_tolerance", 0.2);
}

bool bvxm_generate_edge_map_process::execute()
{
  // Sanity check
  if(!this->verify_inputs())
    return false;

  brdb_value_t<vil_image_view_base_sptr>* input0 = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr input_image_sptr = input0->value();

  double edge_detection_gaussian_sigma,line_fitting_min_length,line_fitting_error_tolerance;
  bool use_lines;
  if (!parameters()->get_value("use_lines", use_lines) ||
    !parameters()->get_value("edge_detection_gaussian_sigma", edge_detection_gaussian_sigma) ||
    !parameters()->get_value("line_fitting_min_length", line_fitting_min_length) ||
    !parameters()->get_value("line_fitting_error_tolerance", line_fitting_error_tolerance)
    ) {
      vcl_cout << "problems in retrieving parameters\n";
      return false;
  }

  vcl_vector<bprb_param*> all_parameters = parameters()->get_param_list();
  vcl_cout << vcl_endl;
  vcl_cout << "printing parameters:" << vcl_endl;
  vcl_cout << "--------------------" << vcl_endl;
  for(unsigned i=0; i<all_parameters.size(); i++){
    vcl_cout << all_parameters[i]->name() << ": " << all_parameters[i]->value_str() << vcl_endl;
  }
  vcl_cout << vcl_endl;

  vil_image_view<vxl_byte> input_image(input_image_sptr);

  int ni = input_image.ni();
  int nj = input_image.nj();
  vil_image_view<vxl_byte> edge_image;  
  edge_image.set_size(ni,nj);
  edge_image.fill(0);

  sdet_detector_params dp;
  dp.smooth = (float)edge_detection_gaussian_sigma;
  dp.automatic_threshold = false;
  dp.aggressive_junction_closure = 0;
  dp.junctionp = false;

  sdet_detector detector(dp);
  vil_image_resource_sptr input_image_res_sptr = vil_new_image_resource_of_view(*input_image_sptr);
  detector.SetImage(input_image_res_sptr);
  detector.DoContour();
  vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  if(use_lines){
    sdet_fit_lines_params line_detector_params((int)line_fitting_min_length,line_fitting_error_tolerance);
    sdet_fit_lines line_detector(line_detector_params);
    line_detector.set_edges(*edges);
    line_detector.fit_lines();
    vcl_vector<vsol_line_2d_sptr> & lines = line_detector.get_line_segs();

    double ni_d = (double)ni;
    double nj_d = (double)nj;

    // Following loop removes the lines caused by the edges in the image boundary
    for (unsigned i=0; i<lines.size(); i++){
      double curr_length = lines[i]->length();
      if (vnl_math_abs(curr_length-ni_d) < 2.0 || vnl_math_abs(curr_length-nj_d) < 2.0) {
        lines.erase(lines.begin()+i);
        i--;
      }
    }

    // Drawing lines to the edge image
    for(unsigned i=0; i<lines.size(); i++){
      vsol_point_2d_sptr tpt1 = lines[i]->p0();
      vsol_point_2d_sptr tpt2 = lines[i]->p1();

      double x1 = tpt1->x();
      double y1 = tpt1->y();
      double x2 = tpt2->x();
      double y2 = tpt2->y();

      double x_diff = x2 - x1;
      double y_diff = y2 - y1;

      double line_length = lines[i]->length();

      for(double j=0.0; j<line_length; j=j+0.5){
        int coord_x = vnl_math_rnd(x1 + ((j/line_length)*x_diff));
        int coord_y = vnl_math_rnd(y1 + ((j/line_length)*y_diff));
        if(coord_x>=0 && coord_y>=0 && coord_x<ni && coord_y<nj){
          edge_image(coord_x,coord_y) = 255;
        }
      }
    }
  }
  else{
    vcl_vector<vcl_pair<int,int> > curr_edge_points;

    for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges->begin(); eit != edges->end(); eit++)
    {
      vsol_curve_2d_sptr c = (*eit)->curve();
      vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
      if (!dc)
        continue;
      vdgl_interpolator_sptr intp = dc->get_interpolator();
      vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
      for(unsigned j=0; j<ec->size(); j++){
        vdgl_edgel curr_edgel = ec->edgel(j);
        int cr_x = (int)curr_edgel.x();
        int cr_y = (int)curr_edgel.y();

        vcl_pair<int,int> curr_pair(cr_x,cr_y);
        curr_edge_points.push_back(curr_pair);
      }
    }

    for(unsigned i=0; i<curr_edge_points.size(); i++) {
      edge_image(curr_edge_points[i].first,curr_edge_points[i].second) = 255;
    }

    // Following loop removes the edges in the image boundary
    int temp_index = nj-1;
    for(int i=0; i<ni; i++){
      edge_image(i,0) = 0;
      edge_image(i,temp_index) = 0;
    }
    temp_index = ni-1;
    for(int j=0; j<nj; j++){
      edge_image(0,j) = 0;
      edge_image(temp_index,j) = 0;
    }
  }

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(edge_image));
  output_data_[0] = output0;

  return true;
}
