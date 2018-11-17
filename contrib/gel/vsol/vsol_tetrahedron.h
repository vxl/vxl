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

#include <iostream>
#include <iosfwd>
#include <vsol/vsol_polyhedron.h>
#include <vsol/vsol_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  ~vsol_tetrahedron() override = default;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d* clone(void) const override { return new vsol_tetrahedron(*this); }

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
  double volume(void) const override;

  //***************************************************************************
  // Element change
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first vertex
  //---------------------------------------------------------------------------
  void set_p0(const vsol_point_3d_sptr& new_p0);

  //---------------------------------------------------------------------------
  //: Set the second vertex
  //---------------------------------------------------------------------------
  void set_p1(const vsol_point_3d_sptr& new_p1);

  //---------------------------------------------------------------------------
  //: Set the third vertex
  //---------------------------------------------------------------------------
  void set_p2(const vsol_point_3d_sptr& new_p2);

  //---------------------------------------------------------------------------
  //: Set the last vertex
  //---------------------------------------------------------------------------
  void set_p3(const vsol_point_3d_sptr& new_p3);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  bool in(vsol_point_3d_sptr const &p) const override;

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vsol_tetrahedron"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const std::string& cls) const override
  { return cls==is_a() || vsol_polyhedron::is_class(cls); }
};

#endif // vsol_tetrahedron_h_
