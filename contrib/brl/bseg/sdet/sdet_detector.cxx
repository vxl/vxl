// This is brl/bseg/sdet/sdet_detector.cxx
#include "sdet_detector.h"
//:
// \file
// see sdet_detector.h
//
//-----------------------------------------------------------------------------

#include <vil1/vil1_image.h>
#include <vcl_iostream.h>
#include <gevd/gevd_float_operators.h>
#include <gevd/gevd_step.h>
#include <gevd/gevd_bufferxy.h>
#include <sdet/sdet_contour.h>

//--------------------------------------------------------------------------------
//
//: Constructors.
//
sdet_detector::sdet_detector(sdet_detector_params& params)
  : sdet_detector_params(params),
    edgel(NULL), direction(NULL),
    locationx(NULL), locationy(NULL), grad_mag(NULL),
    angle(NULL), junctionx(NULL), junctiony(NULL), njunction(0),
    vertices(NULL), edges(NULL),
    filterFactor(2), hysteresisFactor(2.0), noiseThreshold(0.0)
{   
  if (params.automatic_threshold)
    noise = -params.noise_weight;
  else
    noise = params.noise_multiplier;
//don't really know but have to pick one
  use_vil_image = true;
  image=0;
  vimage=0;
}

sdet_detector::sdet_detector(vil1_image img, float smoothSigma, float noiseSigma,
                             float contour_factor, float junction_factor, int min_length,
                             float maxgap, float min_jump)
  : image(img), vimage(0), noise(noiseSigma), edgel(NULL), direction(NULL),
    locationx(NULL), locationy(NULL), grad_mag(NULL),
    angle(NULL), junctionx(NULL), junctiony(NULL), njunction(0),
    vertices(NULL), edges(NULL),
    filterFactor(2), hysteresisFactor(2.0), noiseThreshold(0.0)
{
  use_vil_image = false;
  sdet_detector_params::smooth = smoothSigma;
  sdet_detector_params::contourFactor = contour_factor;
  sdet_detector_params::junctionFactor = junction_factor;
  sdet_detector_params::minLength = min_length;
  sdet_detector_params::maxGap = maxgap;
  sdet_detector_params::minJump = min_jump;
}

sdet_detector::sdet_detector(vil_image_resource_sptr & img, float smoothSigma, float noiseSigma,
                             float contour_factor, float junction_factor, int min_length,
                             float maxgap, float min_jump)
  : image(0),vimage(img), noise(noiseSigma), edgel(NULL), direction(NULL),
    locationx(NULL), locationy(NULL), grad_mag(NULL),
    angle(NULL), junctionx(NULL), junctiony(NULL), njunction(0),
    vertices(NULL), edges(NULL),
    filterFactor(2), hysteresisFactor(2.0), noiseThreshold(0.0)
{
  use_vil_image = true;
  sdet_detector_params::smooth = smoothSigma;
  sdet_detector_params::contourFactor = contour_factor;
  sdet_detector_params::junctionFactor = junction_factor;
  sdet_detector_params::minLength = min_length;
  sdet_detector_params::maxGap = maxgap;
  sdet_detector_params::minJump = min_jump;
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
  vertices = 0;
  delete edges;
  edges = 0;
}


//: Detect the contour, a list of edges and vertices are generated.
//
bool  sdet_detector::DoContour()
{
  if (edges && vertices) return true;

  if (!DoStep()) {
    vcl_cout << "***Fail on DoContour.\n";
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
    vcl_cout << "***Fail on FindNetwork.\n";
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
  return true;
}

//--------------------------------------------------------------------------------
//
//: Detect the fold contour, a list of edges and vertices are generated.
//
bool  sdet_detector::DoFoldContour()
{
  if (edges && vertices) return true;

#if 0
  if (!DoFold()) {
    vcl_cout << "***Fail on DoFoldContour.\n";
    return false;
  }
#endif
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
    vcl_cout << "***Fail on FindNetwork.\n";
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
  if(use_vil_image)
    source = GetBufferFromVilImage();
  else
    source = GetBufferFromImage();
  if (!source) {
    vcl_cout << " cannot get image buffer\n";
    return false;
  }
  gevd_step step(this->smooth, this->noise, this->contourFactor, this->junctionFactor);

  step.DetectEdgels(*source, edgel, direction, locationx, locationy, grad_mag, angle);


  if (this->junctionp) {                // extension to real/virtual contours
    njunction = step.RecoverJunctions(*source,
                                      *edgel, *direction,
                                      *locationx, *locationy,
                                      junctionx, junctiony);
  } else {
    njunction = 0;
    delete [] junctionx; junctionx = NULL;
    delete [] junctiony; junctiony = NULL;
  }

  this->noiseThreshold = step.NoiseThreshold();
  delete source;//this fixes a leak
  return edgel!=NULL;
}

#if 0 // commented out
//---------------------------------------------------------------------------
//
//: Detect fold profiles in the image, using dG+NMS+extension.
//
bool sdet_detector::DoFold()
{
  if (edgel) return true;

  const BufferXY* source = GetBufferFromImage();
  if (!source) {
    vcl_cout << " cannot get image buffer\n";
    return false;
  }

  Fold fold(this->smooth, this->noise,
            this->contourFactor,
            this->junctionFactor);
  fold.DetectEdgels(*source, edgel, direction,
                    locationx, locationy, true, //Flag to compute mag, angle
                    grad_mag, angle); //Reusing grad_mag, actually |d2G|

  if (this->junctionp) {                // extension to real/virtual contours
    njunction = fold.RecoverJunctions(*source,
                                      *edgel, *direction,
                                      *locationx, *locationy,
                                      junctionx, junctiony);
  } else {
    njunction = 0;
    delete [] junctionx; junctionx = NULL;
    delete [] junctiony; junctiony = NULL;
  }

  this->noiseThreshold = fold.NoiseThreshold();
  return edgel!=NULL;
}
#endif // 0

//--------------------------------------------------------------------------------
//
//: Transform data in the image as float buffer.
//
// two versions so as not to break anything from the ancient code JLM
// vil1_image version
gevd_bufferxy* sdet_detector::GetBufferFromImage()
{
  gevd_bufferxy* image_float_buf = 0;

  if (image_float_buf) return image_float_buf;
  //Tests for validity
  if (!image)
  {
    vcl_cout << "In sdet_detector::GetBufferFromImage() - no image\n";
    return 0;
  }
  if (image.components()!=1)
  {
    vcl_cout << "In sdet_detector::GetBufferFromImage() -"
             << " not exactly one component\n";
    return 0;
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
    image_float_buf = 0;
  }

  return image_float_buf;
}
// vil_image version
gevd_bufferxy* sdet_detector::GetBufferFromVilImage()
{
  gevd_bufferxy* image_float_buf = 0;

  if (image_float_buf) return image_float_buf;
  //Tests for validity
  
  if(!use_vil_image||!vimage->ni()||!vimage->nj())
  {
    vcl_cout << "In sdet_detector::GetBufferFromVilImage() - no image\n";
    return 0;
  }
  if (vimage->nplanes()!=1)
  {
    vcl_cout << "In sdet_detector::GetBufferFromImage() -"
             << " not exactly one component\n";
    return 0;
  }

  int sizey= vimage->nj();
  int sizex= vimage->ni();

  image_float_buf = new gevd_bufferxy(sizex, sizey,8*sizeof(float));



  gevd_bufferxy image_buf(vimage);

  if (! gevd_float_operators::BufferToFloat(image_buf, *image_float_buf))
  {
    delete image_float_buf;
    image_float_buf = 0;
  }

  return image_float_buf;
}

void sdet_detector::print(vcl_ostream &strm) const
{
  strm << "sdet_detector:\n"
       << "    noise " << noise << vcl_endl
       << "    njunction " << njunction << vcl_endl
       << "    num vertices " << vertices->size() << vcl_endl
       << "    num edges " << edges->size() << vcl_endl
       << "    filterfactor " << filterFactor << vcl_endl
       << "    hysteresisfactor " << hysteresisFactor << vcl_endl
       << "    noiseThreshold " << noiseThreshold << vcl_endl
       << "    smooth " <<   smooth << vcl_endl // Smoothing kernel sigma
       << "    noise_weight " <<   noise_weight << vcl_endl //The weight between sensor noise and texture noise
       << "    noise_multiplier " <<   noise_multiplier << vcl_endl // The overal noise threshold scale factor
       << "    automatic_threshold " <<   automatic_threshold << vcl_endl // Determine the threshold values from image
       << "    aggressive_junction_closure " <<   aggressive_junction_closure << vcl_endl //Close junctions agressively
       << "    minLength " <<   minLength << vcl_endl          // minimum chain length
       << "    contourFactor " <<   contourFactor << vcl_endl  // Threshold along contours
       << "    junctionFactor " <<   junctionFactor << vcl_endl //Threshold at junctions
       << "    filterFactor " <<   filterFactor << vcl_endl    // ratio of sensor to texture noise
       << "    junctionp " <<   junctionp << vcl_endl // recover missing junctions
       << "    minJump " <<   minJump << vcl_endl  // change in strength at junction
       << "    maxGap " <<   maxGap << vcl_endl   // Bridge small gaps up to max_gap across.
       << "    spacingp " <<   spacingp << vcl_endl  // equalize spacing?
       << "    borderp " <<   borderp << vcl_endl   // insert virtual border for closure?
       << "    corner_angle " <<   corner_angle << vcl_endl // smallest angle at corner
       << "    separation " <<   separation << vcl_endl // |mean1-mean2|/sigma
       << "    min_corner_length " <<   min_corner_length << vcl_endl // min length to find corners
       << "    cycle " <<   cycle << vcl_endl // number of corners in a cycle
       << "    ndimension " <<   ndimension // spatial dimension of edgel chains.
       << vcl_endl;
}

void sdet_detector::DoBreakCorners(vcl_vector<vtol_edge_2d_sptr >& /* in_edgels */,
                                   vcl_vector<vtol_edge_2d_sptr >& /* out_edgels */)
{
  vcl_cerr << "sdet_detector::DoBreakCorners() NYI\n";
}

void sdet_detector::SetImage(vil1_image img)
{
  use_vil_image = false;
  image = img;
}


void sdet_detector::SetImage(vil_image_resource_sptr const& img)
{
  use_vil_image = true;
  vimage = img;
}
