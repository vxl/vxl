// This is gel/vsol/vsol_tetrahedron.h
#ifndef vsol_tetrahedron_h_
#define vsol_tetrahedron_h_
//*****************************************************************************
//:
// \file
// \brief tetrahedral volume in 3D space
//
// The vertices order gives the orientation of the tetrahedron
//
// \author Peter Vanroose
// \date   5 July 2000.
//
// \verbatim
//  Modifications
//   2004/05/14 Peter Vanroose  Added describe()
//   2004/09/24 Peter Vanroose  Removed operator==() since parent's suffices
// \endverbatim
//*****************************************************************************

#include <vsol/vsol_polyhedron.h>
#include <vsol/vsol_point_3d.h>
#include <vcl_iosfwd.h>

class vsol_tetrahedron : public vsol_polyhedron
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from 4 points
  //---------------------------------------------------------------------------
  vsol_tetrahedron(vsol_point_3d_sptr const& new_p0,
                   vsol_point_3d_sptr const& new_p1,
                   vsol_point_3d_sptr const& new_p2,
                   vsol_point_3d_sptr const& new_p3);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_tetrahedron(vsol_tetrahedron const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_tetrahedron() {}

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d* clone(void) const { return new vsol_tetrahedron(*this); }

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the first vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p0(void) const;

  //---------------------------------------------------------------------------
  //: Return the second vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p1(void) const;

  //---------------------------------------------------------------------------
  //: Return the third vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p2(void) const;

  //---------------------------------------------------------------------------
  //: Return the last vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p3(void) const;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the volume of `this'
  //---------------------------------------------------------------------------
  virtual double volume(void) const;

  //***************************************************************************
  // Element change
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first vertex
  //---------------------------------------------------------------------------
  void set_p0(vsol_point_3d_sptr new_p0);

  //---------------------------------------------------------------------------
  //: Set the second vertex
  //---------------------------------------------------------------------------
  void set_p1(vsol_point_3d_sptr new_p1);

  //---------------------------------------------------------------------------
  //: Set the third vertex
  //---------------------------------------------------------------------------
  void set_p2(vsol_point_3d_sptr new_p2);

  //---------------------------------------------------------------------------
  //: Set the last vertex
  //---------------------------------------------------------------------------
  void set_p3(vsol_point_3d_sptr new_p3);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  virtual bool in(vsol_point_3d_sptr const &p) const;

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(vcl_ostream &strm, int blanking=0) const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return vcl_string("vsol_tetrahedron"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const vcl_string& cls) const
  { return cls==is_a() || vsol_polyhedron::is_class(cls); }
};

#endif // vsol_tetrahedron_h_
