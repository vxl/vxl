// This is brl/bseg/sdet/sdet_detector.cxx
#include <iostream>
#include "sdet_detector.h"
//:
// \file
// see sdet_detector.h
//
//-----------------------------------------------------------------------------

#include <vil1/vil1_image.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <gevd/gevd_float_operators.h>
#include <gevd/gevd_step.h>
#include <gevd/gevd_fold.h>
#include <gevd/gevd_bufferxy.h>
#include <sdet/sdet_contour.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <brip/brip_vil_float_ops.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator.h>
#include <vsol/vsol_point_2d.h>
//--------------------------------------------------------------------------------
//
//: Constructors.
//
sdet_detector::sdet_detector(sdet_detector_params& params)
  : sdet_detector_params(params),
    edgel(nullptr), direction(nullptr),
    locationx(nullptr), locationy(nullptr), grad_mag(nullptr),
    angle(nullptr), junctionx(nullptr), junctiony(nullptr), njunction(0),
    vertices(nullptr), edges(nullptr),
    filterFactor(2), hysteresisFactor(2.0), noiseThreshold(0.0)
{
  if (params.automatic_threshold)
    noise = -params.noise_weight;
  else
    noise = params.noise_multiplier;
//don't really know but have to pick one
  use_vil_image = true;
  image=nullptr;
  vimage=nullptr;
  use_roi_ = false;
}

sdet_detector::sdet_detector(const vil1_image& img, float smoothSigma, float noiseSigma,
                             float contour_factor, float junction_factor, int min_length,
                             float maxgap, float min_jump)
  : image(img), vimage(nullptr), noise(noiseSigma), edgel(nullptr), direction(nullptr),
    locationx(nullptr), locationy(nullptr), grad_mag(nullptr),
    angle(nullptr), junctionx(nullptr), junctiony(nullptr), njunction(0),
    vertices(nullptr), edges(nullptr),
    filterFactor(2), hysteresisFactor(2.0f), noiseThreshold(0.0f)
{
  use_vil_image = false;
  sdet_detector_params::smooth = smoothSigma;
  sdet_detector_params::contourFactor = contour_factor;
  sdet_detector_params::junctionFactor = junction_factor;
  sdet_detector_params::minLength = min_length;
  sdet_detector_params::maxGap = maxgap;
  sdet_detector_params::minJump = min_jump;
  use_roi_ = false;
}

sdet_detector::sdet_detector(vil_image_resource_sptr & img, float smoothSigma, float noiseSigma,
                             float contour_factor, float junction_factor, int min_length,
                             float maxgap, float min_jump)
  : image(nullptr),vimage(img), noise(noiseSigma), edgel(nullptr), direction(nullptr),
    locationx(nullptr), locationy(nullptr), grad_mag(nullptr),
    angle(nullptr), junctionx(nullptr), junctiony(nullptr), njunction(0),
    vertices(nullptr), edges(nullptr),
    filterFactor(2), hysteresisFactor(2.0f), noiseThreshold(0.0f)
{
  use_vil_image = true;
  sdet_detector_params::smooth = smoothSigma;
  sdet_detector_params::contourFactor = contour_factor;
  sdet_detector_params::junctionFactor = junction_factor;
  sdet_detector_params::minLength = min_length;
  sdet_detector_params::maxGap = maxgap;
  sdet_detector_params::minJump = min_jump;
  use_roi_ = false;
}


//: Destructor.
//  Caller has an obligation to clear all the created edges and vertices.
sdet_detector::~sdet_detector()
{
  ClearData();
}


//: Clear data buffer.  Protected.
//
void sdet_detector::ClearData()
{
  delete edgel; delete direction; delete locationx; delete locationy;
  delete grad_mag; delete angle;
  delete [] junctionx; delete [] junctiony;
  if (vertices)
    vertices->clear();
  if (edges)
    edges->clear();
  delete vertices;
  vertices = nullptr;
  delete edges;
  edges = nullptr;
}


//: Detect the contour, a list of edges and vertices are generated.
//
bool  sdet_detector::DoContour()
{
  if (edges && vertices) return true;
  if(!sdet_detector_params::peaks_only && !sdet_detector_params::valleys_only){
    if (!DoStep()) {
      std::cout << "***Fail on DoContour (Step).\n";
      return false;
    }
  }else if (!DoFold()){
        std::cout << "***Fail on DoContour (Fold).\n";
        return false;
      }

  sdet_contour::ClearNetwork(edges, vertices);       // delete vertices/edges
  sdet_contour contour(this->hysteresisFactor*this->noiseThreshold,
                       this->minLength, this->minJump*this->noiseThreshold,
                       this->maxGap);

  // first, find isolated chains/cycles
  bool find_net  = contour.FindNetwork(*edgel, junctionp,
                                       njunction,
                                       junctionx, junctiony,
                                       edges, vertices);
  if (!find_net) {
    std::cout << "***Fail on FindNetwork.\n";
    return false;
  }

  //Insert a virtual border to enforce closure to support region topology.
  if (this->borderp)
    contour.InsertBorder(*edges, *vertices);

  //Move the edgel locations to interpolated positions using zero crossings
  contour.SubPixelAccuracy(*edges, *vertices,
                           *locationx, *locationy);

  // Reduce zig-zags and space out pixels in chains
  if (this->spacingp)
    sdet_contour::EqualizeSpacing(*edges);

  //Set gradient magnitude and angle values on each edgel
  if (grad_mag&&angle)
    sdet_contour::SetEdgelData(*grad_mag, *angle, *edges);

  //Keep this code which will be needed when we have ROI's
#if 0 // TargetJr
   const RectROI* roi = image->GetROI();
   sdet_contour::Translate(*edges, *vertices, // display location at center
                           roi->GetOrigX()+0.5, // of pixel instead of
                           roi->GetOrigY()+0.5); // upper-left corner
#endif
   if (!use_roi_)
     return true;
   sdet_contour::Translate(*edges, *vertices, // display location at center
                           static_cast<float>(roi_.cmin(0)), // of pixel instead of
                           static_cast<float>(roi_.rmin(0))); // upper-left corner
   return true;
}

//--------------------------------------------------------------------------------
//
//: Detect the fold contour, a list of edges and vertices are generated.
//
bool  sdet_detector::DoFoldContour()
{
  if (edges && vertices) return true;

  if (!DoFold()) {
    std::cout << "***Fail on DoFoldContour.\n";
    return false;
  }
  sdet_contour::ClearNetwork(edges, vertices);       // delete vertices/edges
  sdet_contour contour(this->hysteresisFactor*this->noiseThreshold,
                       this->minLength, this->minJump*this->noiseThreshold,
                       this->maxGap);

  // first, find isolated  chains/cycles
  bool t  = contour.FindNetwork(*edgel, junctionp,
                                njunction,
                                junctionx, junctiony,
                                edges, vertices);
  if (!t) {
    std::cout << "***Fail on FindNetwork.\n";
    return false;
  }
  contour.SubPixelAccuracy(*edges, *vertices, // insert subpixel
                           *locationx, *locationy); // accuracy
  if (this->spacingp)           // reduce zig-zags and space out pixels
    sdet_contour::EqualizeSpacing(*edges); // in chains
  if (this->borderp)            // insert a virtual contour to enforce
    contour.InsertBorder(*edges, *vertices); // closure at border
  if (grad_mag&&angle)
    sdet_contour::SetEdgelData(*grad_mag, *angle, *edges); //Continuous edgel orientation.
#if 0 // TargetJr
  sdet_contour::add_vertex_edgels(*edges);
  const RectROI* roi = image->GetROI();
  sdet_contour::Translate(*edges, *vertices, // display location at center
                          roi->GetOrigX()+0.5, // of pixel instead of
                          roi->GetOrigY()+0.5); // upper-left corner
#endif
  return true;
}

//---------------------------------------------------------------------------
//
//: Detect step profiles in the image, using dG+NMS+extension.
//
bool sdet_detector::DoStep()
{
  if (edgel) return true;

  const gevd_bufferxy* source;
  if (use_vil_image)
    source = GetBufferFromVilImage();
  else
    source = GetBufferFromImage();
  if (!source) {
    std::cout << " cannot get image buffer\n";
    return false;
  }
  gevd_step step(this->smooth, this->noise, this->contourFactor, this->junctionFactor);

  step.DetectEdgels(*source, edgel, direction, locationx, locationy, grad_mag, angle);

  if (this->junctionp) {                // extension to real/virtual contours
    njunction = step.RecoverJunctions(*source,
                                      *edgel, *direction,
                                      *locationx, *locationy,
                                      junctionx, junctiony);
  }
  else {
    njunction = 0;
    delete [] junctionx; junctionx = nullptr;
    delete [] junctiony; junctiony = nullptr;
  }

  this->noiseThreshold = step.NoiseThreshold();
  delete source;//this fixes a leak
  return edgel!=nullptr;
}

//---------------------------------------------------------------------------
//
//: Detect fold profiles in the image, using dG+NMS+extension.
//
bool sdet_detector::DoFold()
{
  if (edgel) return true;
  const gevd_bufferxy* source;
  if (use_vil_image)
    source = GetBufferFromVilImage();
  else
    source = GetBufferFromImage();
  if (!source) {
    std::cout << " cannot get image buffer for fold processing\n";
    return false;
  }

  gevd_fold fold(this->smooth, this->noise,
            this->contourFactor,
            this->junctionFactor);

  fold.DetectEdgels(*source, edgel, direction,
                    locationx, locationy,
                    sdet_detector_params::peaks_only,sdet_detector_params::valleys_only,
                    true, //Flag to compute mag, angle
                    grad_mag, angle); //Reusing grad_mag, actually |d2G|

  if (this->junctionp) {                // extension to real/virtual contours
    njunction = fold.RecoverJunctions(*source,
                                      *edgel, *direction,
                                      *locationx, *locationy,
                                      junctionx, junctiony);
  }
  else {
    njunction = 0;
    delete [] junctionx; junctionx = nullptr;
    delete [] junctiony; junctiony = nullptr;
  }

  this->noiseThreshold = fold.NoiseThreshold();
  delete source;//this fixes a leak
  return edgel!=nullptr;
}


//--------------------------------------------------------------------------------
//
//: Transform data in the image as float buffer.
//
// two versions so as not to break anything from the ancient code JLM
// vil1_image version
gevd_bufferxy* sdet_detector::GetBufferFromImage()
{
  gevd_bufferxy* image_float_buf = nullptr;

  if (image_float_buf) return image_float_buf;
  //Tests for validity
  if (!image)
  {
    std::cout << "In sdet_detector::GetBufferFromImage() - no image\n";
    return nullptr;
  }
  if (image.components()!=1)
  {
    std::cout << "In sdet_detector::GetBufferFromImage() -"
             << " not exactly one component\n";
    return nullptr;
  }

#if 0 // TargetJr
  RectROI* roi = image->GetROI(); // find user-selected region of interest
  int sizex = roi->GetSizeX();
  int sizey = roi->GetSizeY();
#endif
  int sizey= image.rows();
  int sizex= image.cols();

  image_float_buf = new gevd_bufferxy(sizex, sizey,8*sizeof(float));

#if 0 // commented out
  if (image->GetPixelType() == Image::FLOAT)
  {
    image->GetSection(image_float_buf->GetBuffer(),
                      roi->GetOrigX(), roi->GetOrigY(), sizex, sizey);
    return image_float_buf;
  }
#endif

  gevd_bufferxy image_buf(sizex, sizey, image.bits_per_component());

#if 0 // commented out
  image->GetSection(image_buf.GetBuffer(),      // copy bytes image into buf
                    roi->GetOrigX(), roi->GetOrigY(), sizex, sizey);
#endif

  image.get_section(image_buf.GetBuffer(),     // copy bytes image into buf
                    0, 0, sizex, sizey);

  if (! gevd_float_operators::BufferToFloat(image_buf, *image_float_buf))
  {
    delete image_float_buf;
    image_float_buf = nullptr;
  }

  return image_float_buf;
}
// vil_image version
gevd_bufferxy* sdet_detector::GetBufferFromVilImage()
{
  gevd_bufferxy* image_float_buf = nullptr;

  if (image_float_buf) return image_float_buf;
  //Tests for validity

  if (!use_vil_image||!vimage->ni()||!vimage->nj())
  {
    std::cout << "In sdet_detector::GetBufferFromVilImage() - no image\n";
    return nullptr;
  }

  vil_image_resource_sptr process_region = vimage;

  //if an roi is specified then extract view and wrap a resource around it
  if (use_roi_)
  {
    if (roi_.n_regions()!=1)//no roi to process
      return nullptr;
    vil_image_view_base_sptr vb =
      vimage->get_view(roi_.cmin(0), roi_.csize(0), roi_.rmin(0), roi_.rsize(0));
    if (!vb)
      return nullptr;
    process_region = vil_new_image_resource_of_view(*vb);
  }

  if (vimage->nplanes()!=1)
  {
    vil_image_view<unsigned short> sview
      = brip_vil_float_ops::convert_to_short(process_region);
    process_region = vil_new_image_resource_of_view(sview);
  }

  int sizey= process_region->nj();
  int sizex= process_region->ni();

  image_float_buf = new gevd_bufferxy(sizex, sizey,8*sizeof(float));

  gevd_bufferxy image_buf(process_region);

  if (! gevd_float_operators::BufferToFloat(image_buf, *image_float_buf))
  {
    delete image_float_buf;
    image_float_buf = nullptr;
  }

  return image_float_buf;
}

void sdet_detector::print(std::ostream &strm) const
{
  strm << "sdet_detector:\n"
       << "    noise " << noise << std::endl
       << "    njunction " << njunction << std::endl
       << "    num vertices " << vertices->size() << std::endl
       << "    num edges " << edges->size() << std::endl
       << "    filterfactor " << filterFactor << std::endl
       << "    hysteresisfactor " << hysteresisFactor << std::endl
       << "    noiseThreshold " << noiseThreshold << std::endl
       << "    smooth " <<   smooth << std::endl // Smoothing kernel sigma
       << "    noise_weight " <<   noise_weight << std::endl //The weight between sensor noise and texture noise
       << "    noise_multiplier " <<   noise_multiplier << std::endl // The overal noise threshold scale factor
       << "    automatic_threshold " <<   automatic_threshold << std::endl // Determine the threshold values from image
       << "    aggressive_junction_closure " <<   aggressive_junction_closure << std::endl //Close junctions aggressively
       << "    minLength " <<   minLength << std::endl          // minimum chain length
       << "    contourFactor " <<   contourFactor << std::endl  // Threshold along contours
       << "    junctionFactor " <<   junctionFactor << std::endl //Threshold at junctions
       << "    filterFactor " <<   filterFactor << std::endl    // ratio of sensor to texture noise
       << "    junctionp " <<   junctionp << std::endl // recover missing junctions
       << "    minJump " <<   minJump << std::endl  // change in strength at junction
       << "    maxGap " <<   maxGap << std::endl   // Bridge small gaps up to max_gap across.
       << "    spacingp " <<   spacingp << std::endl  // equalize spacing?
       << "    borderp " <<   borderp << std::endl   // insert virtual border for closure?
       << "    corner_angle " <<   corner_angle << std::endl // smallest angle at corner
       << "    separation " <<   separation << std::endl // |mean1-mean2|/sigma
       << "    min_corner_length " <<   min_corner_length << std::endl // min length to find corners
       << "    cycle " <<   cycle << std::endl // number of corners in a cycle
       << "    ndimension " <<   ndimension // spatial dimension of edgel chains.
       << std::endl;
}

void sdet_detector::DoBreakCorners(std::vector<vtol_edge_2d_sptr >& /* in_edgels */,
                                   std::vector<vtol_edge_2d_sptr >& /* out_edgels */)
{
  std::cerr << "sdet_detector::DoBreakCorners() NYI\n";
}

void sdet_detector::SetImage(const vil1_image& img)
{
  use_vil_image = false;
  image = img;
}

void sdet_detector::SetImage(vil_image_resource_sptr const& img, brip_roi const& roi)
{
  use_vil_image = true;
  vimage = img;
  use_roi_ = true;
  roi_ = roi;
}

void sdet_detector::SetImage(vil_image_resource_sptr const& img)
{
  use_vil_image = true;
  vimage = img;
}

bool sdet_detector::
get_vdgl_edges(std::vector<vdgl_digital_curve_sptr>& vd_edges )
{
  vd_edges.clear();
  if (!edges)
    return false;

  for (auto & e : *edges)
  {
    if (!e)
      continue;
    vsol_curve_2d_sptr c = e->curve();
    vdgl_digital_curve* dc = c->cast_to_vdgl_digital_curve();
    if (!dc)
      continue;

    vd_edges.emplace_back(dc);
  }
  if (!vd_edges.size())
    return false;
  return true;
}
std::vector<vsol_digital_curve_2d_sptr> sdet_detector::convert_vdgl_to_vsol(std::vector<vdgl_digital_curve_sptr> const& vd_edges){
  std::vector<vsol_digital_curve_2d_sptr> edges;
  for (const auto & vd_edge : vd_edges)
  {
      //get the edgel chain
    vdgl_interpolator_sptr itrp = vd_edge->get_interpolator();
    vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();
    unsigned int n = ech->size();
    // convert to vsol_digital curve
    vsol_digital_curve_2d_sptr vsdc = new vsol_digital_curve_2d();
    for (unsigned int i=0; i<n;i++)
    {
      vdgl_edgel ed = (*ech)[i];
      double x = ed.get_x(), y = ed.get_y();
      vsdc->add_vertex(new vsol_point_2d(x, y));
    }

   edges.push_back(vsdc);
  }
  return edges;
}
bool
sdet_detector::get_vsol_edges(std::vector<vsol_digital_curve_2d_sptr>& edges )
{
  std::vector<vdgl_digital_curve_sptr> vd_edges;
  if (!this->get_vdgl_edges(vd_edges))
    return false;
  edges = sdet_detector::convert_vdgl_to_vsol(vd_edges);
  return true;
}
