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
  image_ = 0;
  vimage_ = 0;
  clip_ = 0;
  vclip_ = 0;
  use_vil_image_ = true; //who knows, but need to chose one
}

//:Default Destructor
sdet_region_proc::~sdet_region_proc()
{
}

//-------------------------------------------------------------------------
//: Set the image to be processed (vil1 type image)
//
void sdet_region_proc::set_image(vil1_image const& image,
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
  use_vil_image_ = false;
}

//-------------------------------------------------------------------------
//: Set the image to be processed (vil type image)
//
void sdet_region_proc::set_image_resource(vil_image_resource_sptr const& image,
                                          vsol_box_2d_sptr const& box)
{
  if (!image)
  {
    vcl_cout <<"In sdet_region_proc::set_image(.) - null input\n";
    return;
  }
vil_image_resource_sptr vil_crop(const vil_image_resource_sptr &src, unsigned i0,
                                 unsigned n_i, unsigned j0, unsigned n_j);
  if (box)
  {
    vclip_ = 0;
    
    unsigned i0 = (unsigned)box->get_min_x(), j0 = (unsigned)box->get_min_y();
    unsigned n_i = ((unsigned)box->get_max_x())-i0;
    unsigned n_j = ((unsigned)box->get_max_y())-j0;
    vclip_ = vil_crop(image, i0, n_i, j0, n_j);
    if (!vclip_)
      vcl_cout << "In sdet_region_proc::set_image_resource() - chip failed\n";
  }
  regions_valid_ = false;
  vimage_ = image;
  use_vil_image_ = true;
}
bool sdet_region_proc::
set_detector_image_vil1(sdet_detector& d)
{
  // Check the image 
  if (!image_ && !clip_)
    {
      vcl_cout << "In sdet_region_proc::extract_regions() -"
               << " no vil1_image nor clip\n";
      return false;
    }
  vil1_image temp = image_;
  if (clip_)
    temp = clip_;
  vcl_cout << "sdet_region_proc::extract_regions(): width = "
           << temp.width() << " height = " << temp.height() << vcl_endl;

  d.SetImage(temp);
  return true;
}

bool sdet_region_proc::
set_detector_image_vil(sdet_detector& d)
{
  // Check the image resources 
  if (!vimage_ && !vclip_)
    {
      vcl_cout << "In sdet_region_proc::extract_regions() -"
               << " no vil_image nor clip\n";
      return false;
    }
  vil_image_resource_sptr temp = vimage_;
  if (clip_)
    temp = vclip_;
  vcl_cout << "sdet_region_proc::extract_regions(): width = "
           << temp->ni() << " height = " << temp->nj() << vcl_endl;

  d.SetImage(temp);
  return true;
}

//--------------------------------------------------------------------------
//: extract a set of vtol_intensity_face(s)
void sdet_region_proc::extract_regions()
{
  if (regions_valid_)
    return;
  vcl_vector<vtol_edge_2d_sptr> * edges = 0;
  sdet_detector detector(dp_);
  if(!use_vil_image_&&!this->set_detector_image_vil1(detector))
    {
      vcl_cout << "In sdet_region_proc::extract_regions() [vil1] -"
               << " image set error\n";
      return;
    }
  else if(use_vil_image_&&!this->set_detector_image_vil(detector))
    {
      vcl_cout << "In sdet_region_proc::extract_regions() [vil]-"
               << " image set error\n";
      return;
    }

  detector.DoContour();
  edges = detector.GetEdges();
  if (!edges||!edges->size())
  {
    vcl_cout << "In sdet_region_proc::extract_regions() -"
             << " No Edgels were computed\n";
    return;
  }
  //Process the image to extract regions
  regions_.clear();
  sdet_edgel_regions er(array_scale_, verbose_, debug_);
  if(use_vil_image_)
    if(clip_)
      er.compute_edgel_regions(vclip_, *edges, regions_);
    else
      er.compute_edgel_regions(vimage_, *edges, regions_);
  else
    if(clip_)
      er.compute_edgel_regions(clip_, *edges, regions_);
    else 
      er.compute_edgel_regions(image_, *edges, regions_);
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
//--------------------------------------------------------------------------
//: Use a linear approximation to intensity to predict region data.
//  Return the residual error
vil_image_view<float> sdet_region_proc::get_residual_image_view()
{
  vil_image_view<float> null;
  if (!vimage_||!regions_valid_)
  {
    vcl_cout << "In sdet_region_proc::get_residual_image() - no regions\n";
    return null;
  }

  int xsize = vimage_->ni(), ysize = vimage_->nj();
  null.set_size(xsize, ysize);
  null.fill(0.0);
  for (vcl_vector<vtol_intensity_face_sptr>::iterator fit = regions_.begin();
       fit != regions_.end(); fit++)
    for ((*fit)->reset(); (*fit)->next();)
    {
      int x = int((*fit)->X()), y = int((*fit)->Y());
      null(x, y)= (*fit)->Ir();
    }
  return null;
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
