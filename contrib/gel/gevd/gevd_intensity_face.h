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
#ifndef _IntensityFace_h_
#define _IntensityFace_h_

// .NAME IntensityFace - A face with intensity attributes

// .LIBRARY Detection
// .HEADER Segmentation package
// .INCLUDE Detection/IntensityFace.h
// .FILE Detection/IntensityFace.h
// .FILE Detection/IntensityFace.C
// 
// .SECTION Description
//  A sub-class of Face which contains intensity attributes that are the
//  result of samping the interior of the face in an image. The boundaries
//  of the face are vtol_edge(s) with DigitalCurve geometry. The 
//  IntensityFace is nominally 2d and embedded in an image coordinate
//  frame. The IntensityFace can be constructed from a 3-d linear
//  Face (Face with ImplicitPlane and ImplicitLine geometry) under 
//  a WorldToImage camera projection, or directly from an image region 
//  analysis.  In the latter case the IntensityFace is constructed
//  from a set of vtol_edge(s) and then the interior is derived from the 
//  associated image region.  If the IntensityFace is derived from
//  a projected linear Face, the projected boundaries of a set of Faces
//  are used to tesselate the image for region analysis.
// .SECTION Author J.L. Mundy - November 21, 1999
//  
// .SECTION Modifications <none>
//
//-------------------------------------------------------------------------
//#include <cool/defs.h>
//#include <cool/Transform.h>
//#include <Basics/Histogram_ref.h>
//#include <Geometry/ImplicitPlane_ref.h>
//#include <DigitalGeometry/DigitalRegion.h>
//#include <Topology/Face_ref.h>
//#include <Topology/Face.h>
//#include <Detection/IntensityFace_ref.h>
//#include <DigitalGeometry/IntensityCoef_ref.h>
//class Edge;
//class OneChain;

#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_region.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_sptr.h>


class gevd_intensity_face : public vtol_face_2d, public vdgl_digital_region 
{
public:
  //Constructors
  gevd_intensity_face(vcl_vector<vtol_edge_sptr>* edges);
  gevd_intensity_face(one_chain_list& one_chains);
  //  gevd_intensity_face(vcl_vector<vtol_edge_sptr>* edges, vdgl_digital_region& dr);
  gevd_intensity_face(vcl_vector<vtol_one_chain_sptr>* chains, vdgl_digital_region& dr);
  gevd_intensity_face(gevd_intensity_face& iface);
  gevd_intensity_face(vtol_face_2d& face, int npts, float* xp, float* yp,
                unsigned short *pix);
  gevd_intensity_face(vtol_face_2d& face, int npts, float* xp, float* yp, float* zp,
                unsigned short *pix);
  ~gevd_intensity_face();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_sptr clone(void);


  bool IsHoleP();
  inline vtol_topology_object::vtol_topology_object_type 
    GetTopologyType() const { return vtol_topology_object::INTENSITYFACE; }
  virtual gevd_intensity_face* CastToIntensityFace() {return this;}
  //Accessors
  //The Face moment matrix
  virtual vnl_matrix<double> MomentMatrix();
  //UtilityMethods
#if 0
  //The projection of the face onto a given orientation
  virtual void extrema(vcl_vector<float>& orientation, float& min, float& max);
#endif
  //this method is tailored to the DigitalCurve boundary of the gevd_intensity_face
  virtual void compute_bounding_box();

#if 0
  //Only TaggedTransform can handle the shared geometry.
  virtual bool TaggedTransform(CoolTransform const& t);
#endif
  //no callers for perimeter()
  //float perimeter();
#if 0
  //Computations on the adjacent Face(s)
  Histogram_ref GetAdjacentRegionHistogram();
#endif
  float GetAdjacentRegionMean();
 protected:
  //members
};

#endif
