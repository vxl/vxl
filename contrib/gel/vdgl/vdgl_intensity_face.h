// This is gel/vdgl/vdgl_intensity_face.h
#ifndef vdgl_intensity_face_h_
#define vdgl_intensity_face_h_
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
// \author J.L. Mundy
// \date   November 21, 1999
//
// \verbatim
//  Modifications
//   8-May-2002 - Peter Vanroose - no longer inherits from vdgl_digital_region:
//                dependency changed to "has_a", but with cast_to semantics
//   8-Jan-2003 - Peter Vanroose - moved compute_bounding_box() to vtol_face
//   5-Feb-2003 - Peter Vanroose - moved IsHoleP() to vtol_face_2d
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_region.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_face_2d.h>
#include <vdgl/vdgl_intensity_face_sptr.h>

class vdgl_intensity_face : public vtol_face_2d
{
 protected:
  // Data Members--------------------------------------------------------------
  vdgl_digital_region* region_;
 public:
  //Constructors---------------------------------------------------------------
  vdgl_intensity_face(vcl_vector<vtol_edge_sptr>* edges);
  vdgl_intensity_face(one_chain_list& one_chains);
  //  vdgl_intensity_face(vcl_vector<vtol_edge_sptr>* edges, vdgl_digital_region& dr);
  vdgl_intensity_face(vcl_vector<vtol_one_chain_sptr>* chains, vdgl_digital_region const& dr);
  vdgl_intensity_face(vdgl_intensity_face const& iface);
  vdgl_intensity_face(vtol_face_2d& face, int npts, float const* xp, float const* yp,
                      unsigned short const* pix);
  vdgl_intensity_face(vtol_face_2d& face, int npts, float const* xp, float const* yp,
                      float const* zp, unsigned short const* pix);
  ~vdgl_intensity_face();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d_sptr clone(void) const;

  inline vtol_topology_object::vtol_topology_object_type
    GetTopologyType() const { return vtol_topology_object::INTENSITYFACE; }
  virtual vdgl_intensity_face* CastToIntensityFace() {return this;}
  virtual vdgl_digital_region* cast_to_digital_region() const {return region_;}
  virtual vdgl_digital_region* cast_to_digital_region() {return region_;}
  double area() const { return region_->area(); }
  void ResetPixelData() { region_->ResetPixelData(); }
  void IncrementMeans(float x, float y, unsigned short pix){region_->IncrementMeans(x,y,pix);}
  void InitPixelArrays() { region_->InitPixelArrays(); }
  void InsertInPixelArrays(float x, float y, unsigned short pix){region_->InsertInPixelArrays(x,y,pix);}
  float const* Xj() const { return region_->Xj(); }
  float const* Yj() const { return region_->Yj(); }
  float const* Zj() const { return region_->Zj(); }
  unsigned short const* Ij() const { return region_->Ij(); }

  int Npix()const {return region_->Npix(); }
  float X() const { return region_->X(); }
  float Y() const { return region_->Y(); }
  float Z() const { return region_->Z(); }
  unsigned short I() const {return region_->I();}
  void reset() const {region_->reset();}
  bool next() const {return region_->next();}
  float Xo()const { return region_->Xo(); }
  float Yo()const { return region_->Yo(); }
  float Zo()const { return region_->Zo(); }
  float Io()const { return region_->Io(); }
  double X2()const { return region_->X2(); }
  double Y2()const { return region_->Y2(); }
  double XY()const { return region_->XY(); }
  double I2()const { return region_->I2(); }
  double XI()const { return region_->XI(); }
  double YI()const { return region_->YI(); }
  double Ix() const { return region_->Ix(); }
  double Iy() const { return region_->Iy(); }
  float Ir() const { return region_->Ir(); }
  float Diameter() const { return region_->Diameter(); }
  float AspectRatio() const { return region_->AspectRatio(); }
#if 0
  void PrincipalOrientation(vcl_vector<float>& axis) { region_->PrincipalOrientation(axis); }
#endif
  double Var() const { return region_->Var(); }

  //Accessors
  // The Face moment matrix
  virtual vnl_matrix<double> MomentMatrix();
  //Utility Methods
#if 0
  // The projection of the face onto a given orientation
  virtual void extrema(vcl_vector<float>& orientation, float& min, float& max);

  // Only TaggedTransform can handle the shared geometry.
  virtual bool TaggedTransform(CoolTransform const& t);

  float perimeter();

  // Computations on the adjacent Face(s)
  Histogram_ref GetAdjacentRegionHistogram();
#endif
  float GetAdjacentRegionMean();
};

#endif // vdgl_intensity_face_h_
