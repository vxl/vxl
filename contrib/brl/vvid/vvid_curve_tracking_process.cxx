#include <vcl_iostream.h>
#include <vil/vil_memory_image_of.h>
#include <vcl_vector.h>
#include <bdgl/bdgl_curve_tracker.h>
#include <brip/brip_float_ops.h>
#include <vvid/vvid_curve_tracking_process.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vsol/vsol_curve_2d.h>

vvid_curve_tracking_process::vvid_curve_tracking_process(const bdgl_curve_tracker_params & tp, const sdet_detector_params & dp)
{
	detect_params_ = dp;
	params_ = tp;
}

vvid_curve_tracking_process::~vvid_curve_tracking_process()
{
}

bool vvid_curve_tracking_process::execute()
{
	// init
  if (this->get_N_inputs()!=1)
    {
      vcl_cout << "In vvid_curve_tracking_process::execute() - not exactly one"
               << " input image \n";
      return false;
    }
  spat_objs_.clear();

	// input image
  vil_image img = vvid_video_process::get_input_image(0);
  vil_memory_image_of<unsigned char> cimg;
  if (img.components()==3)
    {
      vil_memory_image_of<float> fimg = brip_float_ops::convert_to_float(img);
      vvid_video_process::clear_input();//remove image from input
      //convert a color image to grey
      cimg = brip_float_ops::convert_to_byte(fimg);
    }
  else
    cimg = vil_memory_image_of<unsigned char>(img);

  //initialize the detector
  sdet_detector detector(detect_params_);
  detector.SetImage(cimg);

  //process edges
  detector.DoContour();
  vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  if (!edges)
    return false;

	// pass the edges
  vsol_curve_2d_sptr c;
  vdgl_digital_curve_sptr dc;
  vdgl_interpolator_sptr interp;
  vdgl_edgel_chain_sptr  ec;
  vcl_vector< vdgl_edgel_chain_sptr > ecl;
  for (unsigned int i=0;i<edges->size();i++){
    c = (*edges)[i]->curve();
    dc = c->cast_to_digital_curve();
    interp = dc->get_interpolator();
    ec = interp->get_edgel_chain();
    ecl.push_back(ec);
  }
  input_curve_.push_back(ecl);

	// tracking
	int t = input_curve_.size()-1;
	track_frame(t);

	// pass the results

	// display
  for (unsigned int i=0;i<get_output_size_at(t);i++){
    //vcl_cout<<".";
		vdgl_interpolator_sptr  intp = new vdgl_interpolator_linear( get_output_curve_at(t,i) );
		vdgl_digital_curve_sptr dc = new vdgl_digital_curve(intp);
		spat_objs_.push_back( dc->cast_to_spatial_object_2d() );
		spat_objs_[spat_objs_.size()-1]->set_tag_id( get_output_id_at(t,i) );
  }

  output_image_ = 0;//no output image is produced
  return true;
}

//-----------------------------------------------------------------
