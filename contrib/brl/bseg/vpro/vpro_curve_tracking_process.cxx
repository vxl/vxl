#include "vpro_curve_tracking_process.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vil1/vil1_memory_image_of.h>
#include <vcl_vector.h>
//#include <bdgl/bdgl_curve_tracker.h>
#include <bdgl/bdgl_curve_tracking.h>

#include <brip/brip_float_ops.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <bdgl/bdgl_curve_algs.h>
#include <vsol/vsol_curve_2d.h>

vpro_curve_tracking_process::vpro_curve_tracking_process(bdgl_curve_tracking_params & tp, const sdet_detector_params & dp)
{
  detect_params_ = dp;
  tp_ = tp;
}

vpro_curve_tracking_process::~vpro_curve_tracking_process()
{
}

bool vpro_curve_tracking_process::execute()
{
  // init
  if (this->get_N_input_images()!=1)
  {
    vcl_cerr << "In vpro_curve_tracking_process::execute() -"
             << " not exactly one input image\n";
    return false;
  }
  output_spat_objs_.clear();

  // input image
  vil1_image img = vpro_video_process::get_input_image(0);
  vil1_memory_image_of<unsigned char> cimg;
  if (img.components()==3)
  {
    vil1_memory_image_of<float> fimg = brip_float_ops::convert_to_float(img);
    vpro_video_process::clear_input();//remove image from input
    //convert a color image to grey
    cimg = brip_float_ops::convert_to_byte(fimg);
  }
  else
    cimg = vil1_memory_image_of<unsigned char>(img);

  this->clear_input();
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
  vcl_vector<vtol_edge_2d_sptr> ecl;
  ecl.clear();

  for (unsigned int i=0; i<edges->size(); i++)
  {
    c  = (*edges)[i]->curve();
    dc = c->cast_to_digital_curve();
    if (dc->length()>tp_.min_length_of_curves)
      ecl.push_back((*edges)[i]);
  }

  input_curves_.push_back(ecl);
  // tracking
  int t = input_curves_.size()-1;
  track_frame(t);

  for (int i=0; i<get_output_size_at(t); ++i)
  {
    bdgl_tracker_curve_sptr test_curve1=get_output_curve(t,i);
    bdgl_tracker_curve_sptr new_curve,old_curve;
    vtol_edge_2d_sptr edc=test_curve1->get_curve();

    vdgl_digital_curve_sptr dc = bdgl_curve_algs::create_digital_curves(test_curve1->desc->points_);

    if (test_curve1->match_id_==-1 )
    {
      output_spat_objs_.push_back( dc->cast_to_spatial_object_2d() );
      output_spat_objs_[output_spat_objs_.size()-1]->set_tag_id(-1);
    }
    else if (test_curve1->match_id_>0 )
    {
      output_spat_objs_.push_back( dc->cast_to_spatial_object_2d() );
      output_spat_objs_[output_spat_objs_.size()-1]->set_tag_id( test_curve1->match_id_ );
    }
  }

  vcl_cout<<"\n frame no :"<<t;
  output_image_ = 0;//no output image is produced
  write_to_file();

  return true;
}


bool vpro_curve_tracking_process::write_to_file()
{
  //search for the biggest curve in output_curve_[0]

  // writing the code to write tracks and their point correspondences
  for (unsigned int k=0; k<output_curves_[0].size(); ++k)
  {
    match_data_sptr bmn = output_curves_[0][k]->get_best_match_next();
    int cnt=1;
    while (bmn)
    {
      cnt++;
#ifdef DEBUG
      vcl_cout<<"\nthe euc dist is "<<bmn->match_curve_set[0]->get_best_match_next()->euc_<<'\t';
#endif
      bmn=bmn->match_curve_set[0]->get_best_match_next();
    }

    if (cnt>5)
    {
      vcl_ostringstream o; o<<k;
      vcl_string curve_num=o.str();
      vcl_string temp_name="c:\\temp_input\\curve" + curve_num+".txt";
      vcl_ofstream f(temp_name.c_str());
      f<<"# CONTOUR_EDGE_MAP : canny+van-ducks\n"
       <<"# .cem files\n"
       <<"# Format :\n"
       <<"# Each contour block will consist of the following\n"
       <<"# [BEGIN CONTOUR]\n"
       <<"# EDGE_COUNT=num_of_edges\n"
       <<"# [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf\n"
       <<"CURVE\n";
      bool flag=true;
      bdgl_tracker_curve_sptr obj=output_curves_[0][k];
      while (flag)
      {
        f<<"[BEGIN CONTOUR]\n"
         <<"EDGE_COUNT="<<obj->desc->points_.size()<<'\n';
        for (unsigned int i=0; i<obj->desc->points_.size(); ++i)
        {
          f<<' '<<'['<<obj->desc->points_[i].x()<<", "<<obj->desc->points_[i].y()
           <<']'<<"   "<<obj->desc->angles_[i]<<' '<<obj->desc->grad_[i]<<'\n';
        }
        f<<"[END CONTOUR]\n";

        if (obj->get_best_match_next())
          obj=obj->get_best_match_next()->match_curve_set[0];
        else
          flag=false;
      }
      f<<"END CURVE\n";
      f.close();
    }
  }

  return true;
}

//-----------------------------------------------------------------
