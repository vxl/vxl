// This is brl/vvid/vvid_edge_process.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vil/vil_memory_image_of.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <brip/brip_float_ops.h>
#include <sdet/sdet_detector.h>
#include <vvid/vvid_edge_process.h>

vvid_edge_process::vvid_edge_process(sdet_detector_params & dp)
  : sdet_detector_params(dp)
{
}

vvid_edge_process::~vvid_edge_process()
{
}

bool vvid_edge_process::execute()
{
  if (this->get_N_inputs()!=1)
    {
      vcl_cout << "In vvid_edge_process::execute() - not exactly one"
               << " input image \n";
      return false;
    }
  topo_objs_.clear();

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
  sdet_detector detector(*((sdet_detector_params*)this));
  detector.SetImage(cimg);
  //process edges
  detector.DoContour();
  vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  if (!edges)
    return false;

  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges->begin();
       eit != edges->end(); eit++)
    topo_objs_.push_back((*eit)->cast_to_topology_object());

  output_image_ = 0;//no output image is produced
  return true;
}
