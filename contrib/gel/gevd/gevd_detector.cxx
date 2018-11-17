// This is gel/gevd/gevd_detector.cxx
#include <iostream>
#include "gevd_detector.h"
//:
// \file
// see gevd_detector.h
//
//-----------------------------------------------------------------------------

#include <vil1/vil1_image.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "gevd_pixel.h"
#include "gevd_float_operators.h"
#include "gevd_step.h"
#include "gevd_bufferxy.h"
#include "gevd_contour.h"
#include <vtol/vtol_edge_2d.h>

//--------------------------------------------------------------------------------
//
//: Constructors.
//
gevd_detector::gevd_detector(gevd_detector_params& params)
  : gevd_detector_params(params),
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

  image_float_buf_ = nullptr;
}

gevd_detector::gevd_detector(const vil1_image& img, float smoothSigma, float noiseSigma,
                             float contour_factor, float junction_factor, int min_length,
                             float maxgap, float min_jump)
  : image(img), noise(noiseSigma), edgel(nullptr), direction(nullptr),
    locationx(nullptr), locationy(nullptr), grad_mag(nullptr),
    angle(nullptr), junctionx(nullptr), junctiony(nullptr), njunction(0),
    vertices(nullptr), edges(nullptr),
    filterFactor(2), hysteresisFactor(2.0f), noiseThreshold(0.0f)
{
  gevd_detector_params::smooth = smoothSigma;
  gevd_detector_params::contourFactor = contour_factor;
  gevd_detector_params::junctionFactor = junction_factor;
  gevd_detector_params::minLength = min_length;
  gevd_detector_params::maxGap = maxgap;
  gevd_detector_params::minJump = min_jump;
  image_float_buf_ = nullptr;
}

//--------------------------------------------------------------------------
//: UnProtect lists that are protected by Contour::
void gevd_detector::UnProtectLists()
{
#if 0 // commented out
  if (edges)//Need to mimic the protection of Contour
  for (CoolListP<Edge*>::iterator eit = edges->begin();
       eit != edges->end(); eit++)
    (*eit)->UnProtect();
  if (vertices)
    for (CoolListP<Vertex*>::iterator vit = vertices->begin();
         vit != vertices->end(); vit++)
      (*vit)->UnProtect();
#endif
}


//: Destructor.
//  Caller has an obligation to clear all the created edges and vertices.
gevd_detector::~gevd_detector()
{
  ClearData();
}


//: Clear data buffer.  Protected.
//
void gevd_detector::ClearData()
{
  delete edgel; delete direction; delete locationx; delete locationy;
  delete grad_mag; delete angle;
  delete [] junctionx; delete [] junctiony;
  delete vertices;
  delete edges;
  if (image_float_buf_) delete image_float_buf_;
}


//: Detect the contour, a list of edges and vertices are generated.
//
bool  gevd_detector::DoContour()
{
  if (edges && vertices) return true;

  if (!DoStep()) {
    std::cout << "***Fail on DoContour.\n";
    return false;
  }
  gevd_contour::ClearNetwork(edges, vertices);       // delete vertices/edges
  gevd_contour contour(this->hysteresisFactor*this->noiseThreshold, this->minLength,
                       this->minJump*this->noiseThreshold, this->maxGap);
  bool t  = contour.FindNetwork(*edgel, njunction, // first, find isolated
                                junctionx, junctiony,   // chains/cycles
                                edges, vertices);
  if (!t) {
    std::cout << "***Fail on FindNetwork.\n";
    return false;
  }

  std::vector<vtol_edge_2d_sptr>::iterator edge;
  std::cout << "IN DoContour before SubPixelAccuracy\n";
  this->print(std::cout);
  for ( edge = edges->begin() ; edge != edges->end(); ++edge)
    {
    std::cout << "Edgel output from DoContour:";
    (*edge)->describe(std::cout, 2);
    }

  contour.SubPixelAccuracy(*edges, *vertices, // insert subpixel
                           *locationx, *locationy); // accuracy
  if (this->spacingp)           // reduce zig-zags and space out pixels
    gevd_contour::EqualizeSpacing(*edges); // in chains
  if (this->borderp)            // insert a virtual contour to enforce
    contour.InsertBorder(*edges, *vertices); // closure at border
  if (grad_mag&&angle)
    gevd_contour::SetEdgelData(*grad_mag, *angle, *edges); //Continuous edgel orientation.

//   const RectROI* roi = image->GetROI();
//   gevd_contour::Translate(*edges, *vertices, // display location at center
//                      roi->GetOrigX()+0.5, // of pixel instead of
//                      roi->GetOrigY()+0.5); // upper-left corner

  return true;
}

//--------------------------------------------------------------------------------
//
//: Detect the fold contour, a list of edges and vertices are generated.
//
bool  gevd_detector::DoFoldContour()
{
  if (edges && vertices) return true;

//   if (!DoFold()) {
//     std::cout << "***Fail on DoFoldContour.\n";
//     return false;
//   }
  gevd_contour::ClearNetwork(edges, vertices);       // delete vertices/edges
  gevd_contour contour(this->hysteresisFactor*this->noiseThreshold,
                       this->minLength, this->minJump*this->noiseThreshold,
                       this->maxGap);

  bool t  = contour.FindNetwork(*edgel, njunction, // first, find isolated
                                junctionx, junctiony,   // chains/cycles
                                edges, vertices);
  if (!t) {
    std::cout << "***Fail on FindNetwork.\n";
    return false;
  }
  contour.SubPixelAccuracy(*edges, *vertices, // insert subpixel
                           *locationx, *locationy); // accuracy
  if (this->spacingp)           // reduce zig-zags and space out pixels
    gevd_contour::EqualizeSpacing(*edges); // in chains
  if (this->borderp)            // insert a virtual contour to enforce
    contour.InsertBorder(*edges, *vertices); // closure at border
  if (grad_mag&&angle)
    gevd_contour::SetEdgelData(*grad_mag, *angle, *edges); //Continuous edgel orientation.

//   const RectROI* roi = image->GetROI();
//   gevd_contour::Translate(*edges, *vertices, // display location at center
//                      roi->GetOrigX()+0.5, // of pixel instead of
//                      roi->GetOrigY()+0.5); // upper-left corner

  return true;
}

//---------------------------------------------------------------------------
//
//: Detect step profiles in the image, using dG+NMS+extension.
//
bool gevd_detector::DoStep()
{
  if (edgel) return true;

  const gevd_bufferxy* source = GetBufferFromImage();
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

  return edgel!=nullptr;
}

#if 0 // commented out
//---------------------------------------------------------------------------
//
//: Detect fold profiles in the image, using dG+NMS+extension.
//
bool gevd_detector::DoFold()
{
  if (edgel) return true;

  const BufferXY* source = GetBufferFromImage();
  if (!source) {
    std::cout << " cannot get image buffer\n";
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
  }
  else {
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
gevd_bufferxy* gevd_detector::GetBufferFromImage()
{
  if (image_float_buf_) return image_float_buf_;

  if (!image)
  {
    std::cout << "No image\n";
    return nullptr;
  }

  //  RectROI* roi = image->GetROI(); // find user-selected region of interest
  //  int sizex = roi->GetSizeX();
  //  int sizey = roi->GetSizeY();
  int sizey= image.rows();
  int sizex= image.cols();

  image_float_buf_ = new gevd_bufferxy(sizex, sizey,8*sizeof(float));

#if 0 // commented out
  if (image->GetPixelType() == Image::FLOAT)
  {
    image->GetSection(image_float_buf_->GetBuffer(),
                      roi->GetOrigX(), roi->GetOrigY(), sizex, sizey);
    return image_float_buf_;
  }
#endif

  //   gevd_bufferxy image_buf(sizex, sizey, image->GetBitsPixel());
  gevd_bufferxy image_buf(sizex, sizey, image.bits_per_component());

#if 0 // commented out
  image->GetSection(image_buf.GetBuffer(),      // copy bytes image into buf
                    roi->GetOrigX(), roi->GetOrigY(), sizex, sizey);
#endif

  image.get_section(image_buf.GetBuffer(),     // copy bytes image into buf
                    0, 0, sizex, sizey);

  if (! gevd_float_operators::BufferToFloat(image_buf, *image_float_buf_))
  {
    delete image_float_buf_;
    image_float_buf_ = nullptr;
  }

  return image_float_buf_;
}

// If we're setting a new image, we need to start from scratch
void gevd_detector::SetImage(const vil1_image& img)
{
  image = img;
  if (image_float_buf_) {
    delete image_float_buf_;
    image_float_buf_ = nullptr;
  }
  if (edgel) delete edgel;
  if (vertices) delete vertices;
  if (direction) delete direction;
  if (locationx) delete locationx;
  if (locationy) delete locationy;
  if (grad_mag) delete grad_mag;
  if (angle) delete angle;
  if (junctionx) delete [] junctionx;
  if (junctiony) delete [] junctiony;
}

void gevd_detector::print(std::ostream &strm) const
{
  strm << "gevd_Detector:\n"
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
       << "    ndimension " <<   ndimension << std::endl // spatial dimension of edgel chains.
       << std::endl;
}
