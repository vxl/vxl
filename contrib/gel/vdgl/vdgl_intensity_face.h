#ifndef _IntensityFace_h_
#define _IntensityFace_h_

//:
// \file
// \brief A face with intensity attributes
//
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
//
// \author Author J.L. Mundy - November 21, 1999
//
//-------------------------------------------------------------------------

#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_region.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_sptr.h>
#include <vdgl/vdgl_intensity_face_sptr.h>

class vdgl_intensity_face : public vtol_face_2d, public vdgl_digital_region 
{
public:
  //Constructors
  vdgl_intensity_face(vcl_vector<vtol_edge_sptr>* edges);
  vdgl_intensity_face(one_chain_list& one_chains);
  //  vdgl_intensity_face(vcl_vector<vtol_edge_sptr>* edges, vdgl_digital_region& dr);
  vdgl_intensity_face(vcl_vector<vtol_one_chain_sptr>* chains, vdgl_digital_region& dr);
  vdgl_intensity_face(vdgl_intensity_face& iface);
  vdgl_intensity_face(vtol_face_2d& face, int npts, float* xp, float* yp,
                unsigned short *pix);
  vdgl_intensity_face(vtol_face_2d& face, int npts, float* xp, float* yp, float* zp,
                unsigned short *pix);
  ~vdgl_intensity_face();

  bool IsHoleP();
  inline vtol_topology_object::vtol_topology_object_type
    GetTopologyType() const { return vtol_topology_object::INTENSITYFACE; }
  virtual vdgl_intensity_face* CastToIntensityFace() {return this;}
  virtual vdgl_digital_region* cast_to_digital_region() {return this;}
  //Accessors
  //The Face moment matrix
  virtual vnl_matrix<double> MomentMatrix();
  //UtilityMethods
#if 0
  //The projection of the face onto a given orientation
  virtual void extrema(vcl_vector<float>& orientation, float& min, float& max);
#endif
  //this method is tailored to the DigitalCurve boundary of the vdgl_intensity_face
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
