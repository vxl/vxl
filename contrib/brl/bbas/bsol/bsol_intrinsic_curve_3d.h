#ifndef bsol_intrinsic_curve_3d_h_
#define bsol_intrinsic_curve_3d_h_
//*****************************************************************************
//:
// \file
// \brief Generic intrinsic curve in 3D that has intrinsic curvature, torsion, d_theta, d_s defined.
// Note that to make the definition intrinsic, I inherit it from vsol_curve_3d.
//
// \author MingChing Chang
// \date   2004/03/19
//
// \verbatim
//  Modifications
//   2004/03/17 MingChing Chang Creation
// \endverbatim
//*****************************************************************************

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vsol/vsol_curve_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vgl/vgl_vector_3d.h>

//: For Pi definition, use pi, and pi_over_2 defined in vnl/vnl_math.h

#define ZERO_TOLERANCE 1E-1

//: General 3d intrinsic curve class

class bsol_intrinsic_curve_3d : public vsol_curve_3d
{
 protected:

  //***************************************************************************
  // Data members
  // Description: List of vsol_point_3d
  vcl_vector<vsol_point_3d_sptr> *storage_;
  //: First point of the curve : just to conform to vsol_curve_2d standard
  vsol_point_3d_sptr p0_;
  //: Last point of the curve
  vsol_point_3d_sptr p1_;
  //: Arclength measured from the p0_
  vcl_vector<double> arcLength_;
  //: Arclength of the current segment i to i+1
  vcl_vector<double> s_;
  //: The z angle of the intrinsic curve
  vcl_vector<double> phi_;
  //: First derivative of the z angle of the intrinsic curve
  vcl_vector<double> phis_;
  //: Second derivative of the z angle of the intrinsic curve
  vcl_vector<double> phiss_;
  //: The xy angle of the intrinsic curve
  vcl_vector<double> theta_;
  //: First derivative of the xy angle of the intrinsic curve
  vcl_vector<double> thetas_;
  //: Second derivative of the xy angle of the intrinsic curve
  vcl_vector<double> thetass_;
  //: Curvature of the intrinsic curve
  vcl_vector<double> curvature_;
  //: Torsion of the intrinsic curve
  vcl_vector<double> torsion_;
  //: Total curvature of the intrinsic curve
  double totalCurvature_;
  //: Total angle change of the intrinsic curve
  double totalAngleChange_;

  //: Tangent vector for each point
  vcl_vector<vgl_vector_3d<double>*> Tangent_;
  //: Normal vector for each point
  vcl_vector<vgl_vector_3d<double>*> Normal_;
  //: Binormal vector for each point
  vcl_vector<vgl_vector_3d<double>*> Binormal_;

  //: A flag to indicate if this curve is open or close
  bool isOpen_;
  //: The '*.con3' filename of this intrinsic curve
  vcl_string CON3Filename_;

 public:
  //***************************************************************************
  // Initialization

  //: Default Constructor
  bsol_intrinsic_curve_3d();
  //: Constructor from a vcl_vector of points
  bsol_intrinsic_curve_3d(const vcl_vector<vsol_point_3d_sptr> &new_vertices);
  //: Copy constructor
  bsol_intrinsic_curve_3d(const bsol_intrinsic_curve_3d &other);
  //: Destructor
  virtual ~bsol_intrinsic_curve_3d();
  //: Clone `this': creation of a new object and initialization
  // See Prototype pattern
  virtual vsol_spatial_object_3d* clone(void) const;
  //: Return a platform independent string identifying the class
  vcl_string is_a() const { return vcl_string("bsol_intrinsic_curve_3d"); }

  //***************************************************************************
  // Access

  //: Return the first point of `this';  pure virtual of vsol_curve_3d
  virtual vsol_point_3d_sptr p0() const { return p0_; }
  //: Return the last point of `this';   pure virtual of vsol_curve_3d
  virtual vsol_point_3d_sptr p1() const { return p1_; }
  //: Is `i' a valid index for the list of vertices ?
  bool valid_index(unsigned int i) const { return i<size(); }
  //: Return vertex `i'
  vsol_point_3d_sptr vertex(const int i) const {
    assert(valid_index(i));
    return (*storage_)[i];
  }
  //: Return x coord of vertex `i'
  double x (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->x();
  }
  //: Return y coord of vertex `i'
  double y (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->y();
  }
  //: Return z coord of vertex `i'
  double z (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->z();
  }
  //: Return the number of vertices
  unsigned int size(void) const {
    return storage_->size();
  }
  //: Return the total length of the intrinsic curve
  virtual double length (void) const {
    return arcLength_[storage_->size()-1];
  }
  //: Return the total arclength from vertex `0' to vertex `i'
  double arcLength (const int i) const {
    assert (valid_index(i));
    if (i==0)
      return arcLength_[1];
    else
      return arcLength_[i];
  }
  //: Return the total arclength of the current vertex `i-1' to vertex `i'
  double s (const int i) const {
    assert (valid_index(i));
    if (i==0)
      return s_[1];
    else
      return s_[i];
  }
  //: Return the normalized arclength from vertex `0' to vertex `i'
  double normArcLength (const int i) const {
    return arcLength (i) / length();
  }
  //: Return the dx vertex `i-1' to vertex `i'
  double dx (const int i) const {
    assert(valid_index(i) && valid_index(i-1));
    return (*storage_)[i]->x() - (*storage_)[i-1]->x();
  }
  //: Return the dy vertex `i-1' to vertex `i'
  double dy (const int i) const {
    assert(valid_index(i) && valid_index(i-1));
    return (*storage_)[i]->y() - (*storage_)[i-1]->y();
  }
  //: Return the dz vertex `i-1' to vertex `i'
  double dz (const int i) const {
    assert(valid_index(i));
    return (*storage_)[i]->z() - (*storage_)[i-1]->z();
  }
  //: Return the angle of vertex `i'
  double phi (const int i) const {
    assert(valid_index(i));
    if (i==0)
      return phi_[1];
    else
      return phi_[i];
  }
  //: Return the angle of vertex `i'
  double phis (const int i) const {
    assert(valid_index(i));
    if (i==0 || i==1) {
      if (size()<3)
        return 0; //assert (size()>2);
      else
        return phis_[2];
    }
    else
      return phis_[i];
  }
  //: Return the angle of vertex `i'
  double phiss (const int i) const {
    assert(valid_index(i));
    if (i==0 || i==1 || i==2) {
      if (size()<4)
        return 0; //assert (size()>3);
      else
        return phiss_[3];
    }
    else
      return phiss_[i];
  }
  //: Return the angle of vertex `i'
  double theta (const int i) const {
    assert(valid_index(i));
    if (i==0)
      return theta_[1];
    else
      return theta_[i];
  }
  //: Return the angle of vertex `i'
  double thetas (const int i) const {
    assert(valid_index(i));
    if (i==0 || i==1) {
      if (size()<3)
        return 0; //assert (size()>2);
      else
        return thetas_[2];
    }
    else
      return thetas_[i];
  }
  //: Return the angle of vertex `i'
  double thetass (const int i) const {
    assert(valid_index(i));
    if (i==0 || i==1 || i==2) {
      if (size()<4)
        return 0; //assert (size()>3);
      else
        return thetass_[3];
    }
    else
      return thetass_[i];
  }
  //: Return the curvature of vertex `i'
  double curvature (const int i) const {
    assert(valid_index(i));
    if (i==0 || i==1) {
      if (size()<3)
        return 0; //assert (size()>2);
      else
        return curvature_[2];
    }
    else
      return curvature_[i];
  }
  //: Return the curvature of vertex `i'
  double torsion (const int i) const {
    assert(valid_index(i));
    if (i==0 || i==1|| i==2) {
      if (size()<4)
        return 0; //assert (size()>3);
      else
        return torsion_[3];
    }
    return torsion_[i];
  }

  //: Return the total curvature of the intrinsic curve
  double totalCurvature (void) {
    return totalCurvature_;
  }
  //: Return the total angle change of the intrinsic curve
  double totalAngleChange (void) {
    return totalAngleChange_;
  }
  //: Return Tangent vector for the `i-th' vertex
  vgl_vector_3d<double>* Tangent(int i) {
    assert(valid_index(i));
    if (i==0)
      return Tangent_[1];
    else
      return Tangent_[i];
  }
  //: Return Normal vector for the `i-th' vertex
  vgl_vector_3d<double>* Normal(int i) {
    assert(valid_index(i));
    if (i==0 || i==1) {
      if (size()<3)
        return 0; //assert (size()>2);
      else
        return Normal_[2];
    }
    else
      return Normal_[i];
  }
  //: Return Binormal vector for the `i-th' vertex
  vgl_vector_3d<double>* Binormal(int i) {
    assert(valid_index(i));
    if (i==0 || i==1) {
      if (size()<3)
        return 0; //assert (size()>2);
      else
        return Binormal_[2];
    }
    else
      return Binormal_[i];
  }

  //: Return if this curve is open or close
  bool isOpen (void) {
    return isOpen_;
  }
  //: The '*.con3' filename of this intrinsic curve
  vcl_string CON3Filename (void) {
    return CON3Filename_;
  }

  //***************************************************************************
  // Comparison

  //: Has `this' the same points than `other' in the same order ?
  virtual bool operator==(const bsol_intrinsic_curve_3d &other) const;
  virtual bool operator==(const vsol_spatial_object_3d& obj) const; // virtual of vsol_spatial_object_3d
  //: Has `this' the same points than `other' in the same order ?
  inline bool operator!=(const bsol_intrinsic_curve_3d &o) const {return !operator==(o);}

 public:

  //***************************************************************************
  // Status setting

  //: Set the first point of the curve
  virtual void set_p0 (const vsol_point_3d_sptr &new_p0) {
    p0_=new_p0;
    storage_->push_back(p0_);
  }
  //: Set the last point of the curve
  virtual void set_p1 (const vsol_point_3d_sptr &new_p1) {
    p1_=new_p1;
    storage_->push_back(p0_);
  }
  //: Compute intrinsic properties.
  // For add_vertex(), remove_vertex(), modify_vertex(), insert_vertex()
  // don't forget to call computeProperties()
  void computeProperties ();
  //void computeProperties_old ();

  //: Delete all points of the intrinsic curve
  void clear();

  //: Add another point to the end of the intrinsic curve
  void add_vertex (const vsol_point_3d_sptr &new_p) {
    storage_->push_back(new_p);
  }
  //: Add another point to the end of the intrinsic curve
  void add_vertex (double x, double y, double z) {
    vsol_point_3d_sptr newpoint = new vsol_point_3d (x, y, z);
    add_vertex (newpoint);
  }
  //: Remove one vertex from the intrinsic curve
  void remove_vertex (const int i) {
    assert (valid_index(i));
    storage_->erase (storage_->begin() + i);
  }
  //: Modify one vertex of the intrinsic curve
  void modify_vertex (const int i, double x, double y, double z) {
    assert (valid_index(i));
    (*storage_)[i]->set_x (x);
    (*storage_)[i]->set_y (y);
    (*storage_)[i]->set_z (z);
  }
  //: Insert one vertex to position `i' of the intrinsic curve
  //  Note that it inserts the vertex into `i-1'
  void insert_vertex (int i, double x, double y, double z) {
    assert (valid_index(i));
    vsol_point_3d_sptr pt = new vsol_point_3d (x,y,z);
    vcl_vector< vsol_point_3d_sptr >::iterator it = storage_->begin();
    it += i;
    storage_->insert (it, pt);
  }

  //: File I/O
  bool LoadCON3File (vcl_string fileName);
  bool SaveCON3File (vcl_string fileName);

  //***************************************************************************
  // Basic operations

  //: Compute the bounding box of `this'
  virtual void compute_bounding_box(void) const;

  //: output description to stream
  inline void describe(vcl_ostream &strm, int blanking=0) const {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << "<bsol_intrinsic_curve_3d " << ( isOpen_ ? "open" : "closed" )
         << ", total curvature=" << totalCurvature_
         << ", total angle change=" << totalAngleChange_ << ">\n";
  }
};

#endif // bsol_intrinsic_curve_3d_h_
