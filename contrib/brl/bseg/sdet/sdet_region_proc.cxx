// This is brl/bseg/sdet_region_proc.cxx

#include <gevd/gevd_float_operators.h>
#include <sdet/sdet_edgel_regions.h>
#include <vdgl/vdgl_intensity_face.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_region_proc.h>

#define ushortPixel(buf,x,y)  (*((unsigned short*)buf->GetElementAddr(x,y)))
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
void sdet_region_proc::set_image(vil_image& image)
{
  if (!image)
    {
      vcl_cout <<"In sdet_region_proc::set_image(.) - null input\n";
      return;
    }
  regions_valid_ = false;
  image_ = image;
}

//--------------------------------------------------------------------------
//: extract a set of vdgl_intensity_face(s)
void sdet_region_proc::extract_regions()
{
  if (regions_valid_)
    return;

  // Check the image
  if (!image_)
    {
      vcl_cout << "In sdet_region_proc::extract_regions() - no image\n";
      return;
    }

  vcl_cout << "sdet_region_proc::extract_regions(): width = "
           << image_.width() << " height = " << image_.height() << vcl_endl;

  //Process the image to extract regions
  regions_.clear();

  // -tpk- need to pass along the scaled image rather than the orignal
  sdet_detector detector(dp_);
  detector.SetImage(image_);
  detector.DoContour();
  vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

  if (!edges||!edges->size())
    {
      vcl_cout << "In sdet_region_proc::extract_regions()- "
               << " No Edgels were computed\n";
      return;
    }

  sdet_edgel_regions er(debug_, verbose_);
  er.compute_edgel_regions(image_, *edges, regions_);
  if(debug_)
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


