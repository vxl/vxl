//This is brl/bseg/bvxm/pro/processes/bvxm_generate_edge_map_process.cxx
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
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

// === define parameters here ===

//:parameter specifying whether to use lines instead of edges
#define PARAM_USE_LINES "use_lines"
//:gaussian sigma for the edge detection process
#define PARAM_GAUSSIAN_SIGMA "edge_detection_gaussian_sigma"
//:minimum line length for the line detection process
#define PARAM_LINE_MIN_LENGTH "line_fitting_min_length"
//:error tolerance for the line detection process
#define PARAM_LINE_ERROR "line_fitting_error_tolerance"

bool bvxm_generate_edge_map_process(bprb_func_process& pro)
{
  //inputs
  unsigned n_inputs_ = 1;
  if (pro.n_inputs()<n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get inputs
  // image
  unsigned i = 0;
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  //check input validity
  if (!input_image_sptr) {
    vcl_cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }
  // get parameters
  double edge_detection_gaussian_sigma=0.0,line_fitting_min_length=0.0,line_fitting_error_tolerance=0.0;
  bool use_lines = false;
  pro.parameters()->get_value(PARAM_USE_LINES, use_lines);
  pro.parameters()->get_value(PARAM_GAUSSIAN_SIGMA, edge_detection_gaussian_sigma);
  pro.parameters()->get_value(PARAM_LINE_MIN_LENGTH, line_fitting_min_length);
  pro.parameters()->get_value(PARAM_LINE_ERROR, line_fitting_error_tolerance);

  //locals
  vil_image_view<vxl_byte> input_image(input_image_sptr);
  int ni = input_image.ni();
  int nj = input_image.nj();

  // initialize the output edge image
  vil_image_view<vxl_byte> edge_image;
  edge_image.set_size(ni,nj);
  edge_image.fill(0);

  // set parameters for the edge detector
  sdet_detector_params dp;
  dp.smooth = (float)edge_detection_gaussian_sigma;
  dp.automatic_threshold = false;
  dp.aggressive_junction_closure = 0;
  dp.junctionp = false;

  // detect edgels from the input image
  sdet_detector detector(dp);
  vil_image_resource_sptr input_image_res_sptr = vil_new_image_resource_of_view(*input_image_sptr);
  detector.SetImage(input_image_res_sptr);
  detector.DoContour();
  vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  // checks if the user selected to use lines instead of edges
  if (use_lines)
  {
    // set parameters for line detection
    sdet_fit_lines_params line_detector_params((int)line_fitting_min_length,line_fitting_error_tolerance);
    sdet_fit_lines line_detector(line_detector_params);
    line_detector.set_edges(*edges);
    line_detector.fit_lines();

    // estimate 2d line segments using the detected edgels
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
    for (unsigned i=0; i<lines.size(); i++) {
      vsol_point_2d_sptr tpt1 = lines[i]->p0();
      vsol_point_2d_sptr tpt2 = lines[i]->p1();

      double x1 = tpt1->x();
      double y1 = tpt1->y();
      double x2 = tpt2->x();
      double y2 = tpt2->y();

      double x_diff = x2 - x1;
      double y_diff = y2 - y1;

      double line_length = lines[i]->length();

      for (double j=0.0; j<line_length; j=j+0.5){
        int coord_x = vnl_math_rnd(x1 + ((j/line_length)*x_diff));
        int coord_y = vnl_math_rnd(y1 + ((j/line_length)*y_diff));
        if (coord_x>=0 && coord_y>=0 && coord_x<ni && coord_y<nj){
          edge_image(coord_x,coord_y) = 255;
        }
      }
    }
  }
  else{
    // the user selected use edges instead of lines (default option)
    vcl_vector<vcl_pair<int,int> > curr_edge_points;

    // iterate over each connected edge component
    for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges->begin(); eit != edges->end(); eit++)
    {
      vsol_curve_2d_sptr c = (*eit)->curve();
      vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
      if (!dc)
        continue;
      vdgl_interpolator_sptr intp = dc->get_interpolator();
      vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();

      // iterate over each point in the connected edge component
      for (unsigned j=0; j<ec->size(); j++) {
        vdgl_edgel curr_edgel = ec->edgel(j);
        int cr_x = (int)curr_edgel.x();
        int cr_y = (int)curr_edgel.y();

        // add the current edge point to the list of edge points
        vcl_pair<int,int> curr_pair(cr_x,cr_y);
        curr_edge_points.push_back(curr_pair);
      }
    }

    // print edge to the empty edge image
    for (unsigned i=0; i<curr_edge_points.size(); i++) {
      edge_image(curr_edge_points[i].first,curr_edge_points[i].second) = 255;
    }

    // Following loop removes the edges in the image boundary
    int temp_index = nj-1;
    for (int i=0; i<ni; i++) {
      edge_image(i,0) = 0;
      edge_image(i,temp_index) = 0;
    }
    temp_index = ni-1;
    for (int j=0; j<nj; j++) {
      edge_image(0,j) = 0;
      edge_image(temp_index,j) = 0;
    }
  }

  // return the output edge image
  unsigned j = 0;
  vcl_vector<vcl_string> output_types_(1);
  output_types_[j++] = "vil_image_view_base_sptr";
  pro.set_output_types(output_types_);

  j=0;
  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(edge_image));
  pro.set_output(j++,output0);

  return true;
}
