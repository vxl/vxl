// This is gel/vtol/vtol_intensity_face.h
#ifndef vtol_intensity_face_h_
#define vtol_intensity_face_h_
//:
// \file
// \brief A face with intensity attributes
//
//  A sub-class of Face which contains intensity attributes that are the
//  result of sampling the interior of the face in an image. The boundaries
//  of the face are vtol_edge(s) with DigitalCurve geometry. The
//  IntensityFace is nominally 2d and embedded in an image coordinate
//  frame. The IntensityFace can be constructed from a 3-d linear
//  Face (Face with ImplicitPlane and ImplicitLine geometry) under
//  a WorldToImage camera projection, or directly from an image region
//  analysis.  In the latter case the IntensityFace is constructed
//  from a set of vtol_edge(s) and then the interior is derived from the
//  associated image region.  If the IntensityFace is derived from
//  a projected linear Face, the projected boundaries of a set of Faces
//  are used to tessellate the image for region analysis.
//
// \author J.L. Mundy
// \date   November 21, 1999
//
// \verbatim
//  Modifications
//   8-May-2002 - Peter Vanroose - no longer inherits from vdgl_digital_region:
//                    dependency changed to "has_a", but with cast_to semantics
//   8-Jan-2003 - Peter Vanroose - moved compute_bounding_box() to vtol_face
//   5-Feb-2003 - Peter Vanroose - moved IsHoleP() to vtol_face_2d
//   9-May-2003 - Mike Petersen - intensity face attributes support:
//                    - added get_min() & get_max() pass-throughs
//                    - restored perimeter() API
//                    - added topology_type() override (INTENSITYFACE)
//   14-Nov-2003 - Joe Mundy - removed leak since region_ was not being deleted
//   22-Sep-2004 - Peter Vanroose - deprecated all 3D interface stuff
//   22-Sep-2004 - Peter Vanroose - added cast_to_intensity_face()
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vnl/vnl_matrix.h>
#include <vdgl/vdgl_digital_region.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_intensity_face_sptr.h>

class vtol_intensity_face : public vtol_face_2d
{
 protected:
  // Data Members--------------------------------------------------------------
  vdgl_digital_region* region_;
 public:
  //Constructors---------------------------------------------------------------
  vtol_intensity_face(vtol_face_2d_sptr const& face);
  vtol_intensity_face(vcl_vector<vtol_edge_sptr>* edges);
  vtol_intensity_face(one_chain_list & one_chains);
  //  vtol_intensity_face(vcl_vector<vtol_edge_sptr>* edges, vdgl_digital_region& dr);
  vtol_intensity_face(vcl_vector<vtol_one_chain_sptr>* chains, vdgl_digital_region const& dr);
  vtol_intensity_face(vtol_intensity_face_sptr const& iface);
  vtol_intensity_face(vtol_face_2d_sptr const& face, int npts, float const* xp, float const* yp,
                      unsigned short const* pix);
  ~vtol_intensity_face();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* clone(void) const;

  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

  inline vtol_topology_object::vtol_topology_object_type
    GetTopologyType() const { return vtol_topology_object::INTENSITYFACE; }

  // MPP 5/9/2003
  // Added API consistent w/ overloaded vtol method
  virtual vtol_topology_object::vtol_topology_object_type
  topology_type(void) const { return GetTopologyType(); }

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an intensity face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual const vtol_intensity_face* cast_to_intensity_face(void) const { return this; }

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is an intensity face, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vtol_intensity_face* cast_to_intensity_face(void) { return this; }

  virtual vdgl_digital_region* cast_to_digital_region() const {return region_;}
  virtual vdgl_digital_region* cast_to_digital_region() {return region_;}
  double area() const { return region_->area(); }
  void ResetPixelData() { region_->ResetPixelData(); }
  void IncrementMeans(float x, float y, unsigned short pix){region_->IncrementMeans(x,y,pix);}
  void ComputeIntensityStdev(){region_->ComputeIntensityStdev();}
  void InitPixelArrays() { region_->InitPixelArrays(); }
  void InsertInPixelArrays(float x, float y, unsigned short pix){region_->InsertInPixelArrays(x,y,pix);}

  float const* Xj() const { return region_->Xj(); }
  float const* Yj() const { return region_->Yj(); }
  // \deprecated since intensity_face is strictly 2d in VXL
  float const* Zj() const { return 0; }
  unsigned short const* Ij() const { return region_->Ij(); }

  int Npix()const {return region_->Npix(); }
  float X() const { return region_->X(); }
  float Y() const { return region_->Y(); }
  // \deprecated since intensity_face is strictly 2d in VXL
  float Z() const { return 0.f; }
  unsigned short I() const {return region_->I();}

  void set_X(float x){region_->set_X(x); }
  void set_Y(float y){region_->set_Y(y); }
  void set_I(unsigned short I){region_->set_I(I);}

  void reset() const {region_->reset();}
  bool next() const {return region_->next();}
  float Xo()const { return region_->Xo(); }
  float Yo()const { return region_->Yo(); }
  // \deprecated since intensity_face is strictly 2d in VXL
  float Zo()const { return 0.f; }
  float Io()const { return region_->Io(); }
  float Io_sd()const { return region_->Io_sd(); }
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

  // MPP 5/9/2003
  // Additional digital region pass-throughs
  float get_min() const { return region_->get_min(); }
  float get_max() const { return region_->get_max(); }

  //Accessors
  // The Face moment matrix
  virtual vnl_matrix<double> MomentMatrix();

  //Utility Methods

  // MPP 5/9/2003
  // Resurrected from #ifdef'd block below for intensity face attributes
  double perimeter();
};

#endif // vtol_intensity_face_h_
