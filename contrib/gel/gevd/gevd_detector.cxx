// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-----------------------------------------------------------------------------
//
// see gevd_detector.h
//
//-----------------------------------------------------------------------------

#include <vil/vil_image.h>

#include "gevd_pixel.h"
#include "gevd_float_operators.h"
#include "gevd_step.h"
#include "gevd_bufferxy.h"
#include "gevd_detector.h"

//--------------------------------------------------------------------------------
//
// -- Constructors.
//
gevd_detector::gevd_detector(gevd_detector_params& params)
  : gevd_detector_params(params),
    edgel(NULL), direction(NULL),
    locationx(NULL), locationy(NULL), grad_mag(NULL),
    angle(NULL), junctionx(NULL), junctiony(NULL), njunction(0),
    vertices(NULL), edges(NULL),
    filterFactor(2), hysteresisFactor(2.0), noiseThreshold(0.0)
{
  if(params.automatic_threshold)
    noise = -params.noise_weight;
  else
    noise = params.noise_multiplier;
}

gevd_detector::gevd_detector(vil_image img, float smoothSigma, float noiseSigma,
                             float contour_factor, float junction_factor, int min_length,
                             float maxgap, float min_jump)
  :  image(img), noise(noiseSigma), edgel(NULL), direction(NULL),
     locationx(NULL), locationy(NULL), grad_mag(NULL),
     angle(NULL), junctionx(NULL), junctiony(NULL), njunction(0),
     vertices(NULL), edges(NULL),
     filterFactor(2), hysteresisFactor(2.0), noiseThreshold(0.0)
{
  gevd_detector_params::smooth = smoothSigma;
  gevd_detector_params::contourFactor = contour_factor;
  gevd_detector_params::junctionFactor = junction_factor;
  gevd_detector_params::minLength = min_length;
  gevd_detector_params::maxGap = maxgap;
  gevd_detector_params::minJump = min_jump;
}

//--------------------------------------------------------------------------
// -- UnProtect lists that are protected by Contour::
void gevd_detector::UnProtectLists()
{
#if 0 // commented out
    if(edges)//Need to mimic the protection of Contour
    for(CoolListP<Edge*>::iterator eit = edges->begin();
        eit != edges->end(); eit++)
      (*eit)->UnProtect();
    if(vertices)
      for(CoolListP<Vertex*>::iterator vit = vertices->begin();
          vit != vertices->end(); vit++)
        (*vit)->UnProtect();
#endif
}


//--------------------------------------------------------------------------------
// -- Destructor. Caller has an obligation to clear all the created edges and
// vertices.
gevd_detector::~gevd_detector()
{
  ClearData();
}


//--------------------------------------------------------------------------------
//
// -- Clear data buffer.  Protected.
//
void gevd_detector::ClearData()
{
  delete edgel; delete direction; delete locationx; delete locationy;
  delete grad_mag; delete angle;
  delete [] junctionx; delete [] junctiony;
  delete vertices;
  delete edges;
}

#if 0 // commented out
//--------------------------------------------------------------------------------
//
// -- Detect step edges and put them to an edge list
//
void gevd_detector::DoContourDetector(CoolListP<Edge *> *the_edges)
{
  ClearData();
  DoContour();

  // copy to destination
  the_edges->clear();
  for(edges->reset(); edges->next();)
    the_edges->push_end(edges->value());
  this->UnProtectLists();
}

//--------------------------------------------------------------------------------
//
// -- Detect step edges and put them to an EdgelGroup.
//
void gevd_detector::DoContourDetector(EdgelGroup* edgel_group)
{
  ClearData();
  DoContour();

  // copy to destination
  for(edges->reset(); edges->next();)
    edgel_group->Add(edges->value());
  this->UnProtectLists();
}
//----------------------------------------------------------------------------
//
// -- Detect step edges and return an EdgelGroup
//
EdgelGroup gevd_detector::DoContourDetector(Image* img)
{
  image = img;
  ClearData();
  DoContour();
  EdgelGroup edgls(*edges);
  this->UnProtectLists();
  return edgls;
}

//----------------------------------------------------------------------------
//
// -- Detect step edges and fill an existing EdgelGroup
//
void gevd_detector::DoContourDetector(Image* img, EdgelGroup& edgels)
{
  image = img;
  ClearData();
  DoContour();
  edgels.Add(edges);
  this->UnProtectLists();
}

//----------------------------------------------------------------------------
//
// -- Detect step edges and fill an existing EdgelGroup
//
void gevd_detector::DoFoldContourDetector(Image* img, EdgelGroup& edgels)
{
  image = img;
  ClearData();
  DoFoldContour();
  edgels.Add(edges);
  this->UnProtectLists();
  return;
}

//----------------------------------------------------------------------------
//
// -- Detect edgels and then find all curvature corners and endpoints
//
void gevd_detector::DoCornerDetector(Image* img, IUPointGroup& corners)
{
  image = img;
  ClearData();
  DoContour();
  //Get parameters from the parameter block
  DoCorner(this->corner_angle, this->separation,
           this->min_corner_length,
           this->cycle, this->ndimension);
  for(vertices->reset(); vertices->next();)
    {
      IUPoint* p = vertices->value()->GetPoint();
      corners.Add(p);
    }
  this->UnProtectLists();
}
//--------------------------------------------------------------------------------
//
// -- Delete corners, add ncorner new coners to the end of the "vertices" list.
//
bool gevd_detector::DoCorner( float corner_angle,	// smallest angle at corner
                         float separation,	// |mean1-mean2|/sigma
                         int length,	// min length to find corners
                         int cycle,		// number of corners in a cycle
                         int ndimension	// number of dimension
)
{
  if (!DoStep() || !DoContour()) {
    cout << "***Fail on DoCorner." << endl;
    return false;
  }


  Corner corner(smooth, corner_angle,
                separation, length,
                cycle, ndimension);
  int ncorner = corner.DetectCorners(*edges, *vertices);
  cout << "Found " << ncorner << " corners." << endl;
  return true;
}

//--------------------------------------------------------------
// -- Take a input edgel group and detect corners on each edgel chain
//    Note that here we don't have to unprotect the edges and vertices
//    lists because we don't protect in the first place.
void gevd_detector::DoBreakCorners(EdgelGroup& in_edgels, EdgelGroup& out_edgels)
{
  edges = Edges(&in_edgels);
  CoolListP<Vertex*> vertices; //dummy argument
  Corner corner(this->smooth,	        // spatial smoothing [0.5 2.0]
                this->corner_angle,     // minimum angle in degrees
                this->separation,       // |mean1-mean2|/sigma threshold
                this->min_corner_length,//minimum length before finding corners
                this->cycle,            // minimum corners in a cycle
                this->ndimension);	// 2-d or 3-d edgels
  corner.DetectCorners(*edges, vertices);
  out_edgels.Add(edges);
}
//-----------------------------------------------------------------------------
//
// -- Detect the contour, a list of edges and vertices are genrated.
//
bool  gevd_detector::DoContour()
{
  if (edges && vertices) return true;

  if (!DoStep()) {
    cout << "***Fail on DoContour." << endl;
    return false;
  }
  Contour::ClearNetwork(edges, vertices);	// delete vertices/edges
  Contour contour(this->hysteresisFactor*this->noiseThreshold, this->minLength,
                  this->minJump*this->noiseThreshold, this->maxGap);
  bool t  = contour.FindNetwork(*edgel, njunction, // first, find isolated
                                junctionx, junctiony,	// chains/cycles
                                edges, vertices);
  if (!t) {
    cout << "***Fail on FindNetwork." << endl;
    return false;
  }
  contour.SubPixelAccuracy(*edges, *vertices, // insert subpixel
                           *locationx, *locationy); // accuracy
  if (this->spacingp)		// reduce zig-zags and space out pixels
    Contour::EqualizeSpacing(*edges); // in chains
  if (this->borderp)		// insert a virtual contour to enforce
    contour.InsertBorder(*edges, *vertices); // closure at border
  Contour::SetDepth(*edges, *vertices,
                    this->depth);
  if(grad_mag&&angle)
    Contour::SetEdgelData(*grad_mag, *angle, *edges); //Continous edgel orientation.

  const RectROI* roi = image->GetROI();
  Contour::Translate(*edges, *vertices, // display location at center
                     roi->GetOrigX()+0.5, // of pixel instead of
                     roi->GetOrigY()+0.5); // upper-left corner

  return true;
}

//--------------------------------------------------------------------------------
//
// -- Detect the fold contour, a list of edges and vertices are generated.
//
bool  gevd_detector::DoFoldContour()
{

  if (edges && vertices) return true;

  if (!DoFold()) {
    cout << "***Fail on DoFoldContour." << endl;
    return false;
  }
  Contour::ClearNetwork(edges, vertices);	// delete vertices/edges
  Contour contour(this->hysteresisFactor*this->noiseThreshold,
                  this->minLength, this->minJump*this->noiseThreshold,
                  this->maxGap);

  bool t  = contour.FindNetwork(*edgel, njunction, // first, find isolated
                                junctionx, junctiony,	// chains/cycles
                                edges, vertices);
  if (!t) {
    cout << "***Fail on FindNetwork." << endl;
    return false;
  }
  contour.SubPixelAccuracy(*edges, *vertices, // insert subpixel
                           *locationx, *locationy); // accuracy
  if (this->spacingp)		// reduce zig-zags and space out pixels
    Contour::EqualizeSpacing(*edges); // in chains
  if (this->borderp)		// insert a virtual contour to enforce
    contour.InsertBorder(*edges, *vertices); // closure at border
  Contour::SetDepth(*edges, *vertices,
                    this->depth);
  if(grad_mag&&angle)
    Contour::SetEdgelData(*grad_mag, *angle, *edges); //Continous edgel orientation.

  const RectROI* roi = image->GetROI();
  Contour::Translate(*edges, *vertices, // display location at center
                     roi->GetOrigX()+0.5, // of pixel instead of
                     roi->GetOrigY()+0.5); // upper-left corner

  return true;
}
#endif

//---------------------------------------------------------------------------
//
// -- Detect step profiles in the image, using dG+NMS+extension.
//
bool gevd_detector::DoStep()
{
  if (edgel) return true;

  const gevd_bufferxy* source = GetBufferFromImage();
  if (!source) {
    vcl_cout << " cannot get image buffer" << vcl_endl;
    return false;
  }

  gevd_step step(this->smooth, this->noise, this->contourFactor, this->junctionFactor);
  step.DetectEdgels(*source, edgel, direction, locationx, locationy, grad_mag, angle);

  if (this->junctionp) {		// extension to real/virtual contours
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

  return (edgel!=NULL);
}

#if 0 // commented out
//---------------------------------------------------------------------------
//
// -- Detect fold profiles in the image, using dG+NMS+extension.
//
bool gevd_detector::DoFold()
{
  if (edgel) return true;

  const BufferXY* source = GetBufferFromImage();
  if (!source) {
    cout << " cannot get image buffer" << endl;
    return false;
  }

  Fold fold(this->smooth, this->noise,
            this->contourFactor,
            this->junctionFactor);
  fold.DetectEdgels(*source, edgel, direction,
                    locationx, locationy, true, //Flag to compute mag, angle
                    grad_mag, angle); //Reusing grad_mag, actually |d2G|

  if (this->junctionp) {		// extension to real/virtual contours
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
  return (edgel!=NULL);
}
#endif

//--------------------------------------------------------------------------------
//
// -- Transform data in the image as float buffer.
//
gevd_bufferxy* gevd_detector::GetBufferFromImage()
{
  static gevd_bufferxy* image_float_buf = 0;

  if (image_float_buf) return image_float_buf;

  if (!image)
    {
      vcl_cout << "No image" << vcl_endl;
      return 0;
    }

  //  RectROI* roi = image->GetROI(); // find user-selected region of interest
  //  int sizex = roi->GetSizeX();
  //  int sizey = roi->GetSizeY();
  int sizex= image.rows();
  int sizey= image.cols();

  image_float_buf = new gevd_bufferxy(sizex, sizey,8*sizeof(float));

#if 0 // commented out
  if(image->GetPixelType() == Image::FLOAT)
    {
      image->GetSection(image_float_buf->GetBuffer(),
                        roi->GetOrigX(), roi->GetOrigY(), sizex, sizey);
      return image_float_buf;
    }
#endif

  //   gevd_bufferxy image_buf(sizex, sizey, image->GetBitsPixel());
   gevd_bufferxy image_buf(sizex, sizey, image.bits_per_component());

#if 0 // commented out
  image->GetSection(image_buf.GetBuffer(),	// copy bytes image into buf
                    roi->GetOrigX(), roi->GetOrigY(), sizex, sizey);
#endif

   image.get_section(image_buf.GetBuffer(),	// copy bytes image into buf
                     0, 0, sizex, sizey);

   if (! gevd_float_operators::BufferToFloat(image_buf, *image_float_buf))
     {
       delete image_float_buf;
       image_float_buf = 0;
     }

   return image_float_buf;
}
