// This is brl/bseg/sdet/sdet_region_proc.cxx
#include "sdet_region_proc.h"
//:
// \file
#include <vnl/vnl_numeric_traits.h>
#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_roi.h>
#include <brip/brip_vil1_float_ops.h>
#include <sdet/sdet_edgel_regions.h>
#include <vsol/vsol_box_2d.h>
#include <vtol/vtol_list_functions.h>
#include <vtol/vtol_intensity_face.h>
#include <sdet/sdet_detector.h>

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
sdet_region_proc::sdet_region_proc(sdet_region_proc_params& rpp)
  : sdet_region_proc_params(rpp)
{
}

//:Default Destructor
sdet_region_proc::~sdet_region_proc()
{
}

//-------------------------------------------------------------------------
//: Set the image to be processed
//
void sdet_region_proc::set_image(vil1_image& image,
                                 vsol_box_2d_sptr const& box)
{
  if (!image)
  {
    vcl_cout <<"In sdet_region_proc::set_image(.) - null input\n";
    return;
  }

  if (box)
  {
    clip_ = vil1_image();
    roi_ = new brip_roi(image.width(), image.height());
    roi_->add_region(box);
    if (!brip_vil1_float_ops::chip(image, roi_, clip_))
      vcl_cout << "In sdet_region_proc::set_image() - chip failed\n";
  }
  regions_valid_ = false;
  image_ = image;
}

//--------------------------------------------------------------------------
//: extract a set of vtol_intensity_face(s)
void sdet_region_proc::extract_regions()
{
  if (regions_valid_)
    return;
  // Check the image
  if (!image_||!clip_)
  {
    vcl_cout << "In sdet_region_proc::extract_regions() - no image\n";
    return;
  }
  vil1_image temp = image_;
  if (clip_)
    temp = clip_;
  vcl_cout << "sdet_region_proc::extract_regions(): width = "
           << temp.width() << " height = " << temp.height() << vcl_endl;

  //Process the image to extract regions
  regions_.clear();

  // -tpk- need to pass along the scaled image rather than the orignal
  sdet_detector detector(dp_);
  detector.SetImage(temp);
  detector.DoContour();
  vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  if (!edges||!edges->size())
  {
    vcl_cout << "In sdet_region_proc::extract_regions() -"
             << " No Edgels were computed\n";
    return;
  }

  sdet_edgel_regions er(array_scale_, verbose_, debug_);
  er.compute_edgel_regions(temp, *edges, regions_);
  if (debug_)
    edge_image_ = er.GetEdgeImage(*edges);
  regions_valid_ = true;
}

//----------------------------------------------------------
//: Clear internal storage
//
void sdet_region_proc::clear()
{
  regions_.clear();
}

//--------------------------------------------------------------------------
//: Use a linear approximation to intensity to predict region data.
//  Return the residual error
vil1_image sdet_region_proc::get_residual_image()
{
  if (!image_||!regions_valid_)
  {
    vcl_cout << "In sdet_region_proc::get_residual_image() - no regions\n";
    return 0;
  }
  int xsize = image_.width(), ysize = image_.height();
  vil1_memory_image_of<unsigned char> res_image(xsize, ysize);
  res_image.fill(0);
  float min_res = (float)vnl_numeric_traits<unsigned short>::maxval;
  for (vcl_vector<vtol_intensity_face_sptr>::iterator fit = regions_.begin();
       fit != regions_.end(); fit++)
    for ((*fit)->reset(); (*fit)->next();)
    {
      float res = (*fit)->Ir();
      if (res<min_res)
        min_res = res;
    }

  for (vcl_vector<vtol_intensity_face_sptr>::iterator fit = regions_.begin();
       fit != regions_.end(); fit++)
    for ((*fit)->reset(); (*fit)->next();)
    {
      int x = int((*fit)->X()), y = int((*fit)->Y());
      float res = (*fit)->Ir();
      float is = res-min_res;//to ensure non-negative
      if (is>255)
        is = 255;//to ensure within char
      unsigned char pix = (unsigned char)is;
      res_image(x, y)=pix;
    }
  return res_image;
}
#if 0
//: If a clip has been used we have to transform the regions back into the image coordinates.
bool sdet_region_proc::transform_regions()
{
  if (!roi_)
    return false;
  //the origin of the roi
  int cmin = roi->cmin(0), rmin = roi->rmin(0);
  //we need to get all the edges and remove duplicates
  //then get the vertices and remove duplicates
  //finally transform the digital regions
  vcl_vector<vtol_edge_sptr> edges;
  for (vcl_vector<vtol_intensity_face_sptr>::iterator fit = regions_.begin();
       fit != regions_.end(); fit++)
  {
    vcl_vector<vtol_edge_sptr> face_edges;
    (*fit)->edges(face_edges);
    for (vcl_vector<vtol_edge_sptr>::iterator eit = face_edges.begin();
         eit != face_edges.end(); eit++)
      edges.push_back(*eit);
  }
  //remove duplicates
  vcl_vector<vtol_edge_sptr>* edg_adr = &edges;
  edg_adr = tagged_union(edg_adr);

  for (vcl_vector<vtol_edge_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
    ...
}
#endif
