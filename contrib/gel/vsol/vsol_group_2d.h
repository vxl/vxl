// This is gel/vsol/vsol_group_2d.h
#ifndef vsol_group_2d_h_
#define vsol_group_2d_h_
//*****************************************************************************
//:
// \file
// \brief Group of spatial objects in a 2D space
//
// \author François BERTEL
// \date   2000/04/14
//
// \verbatim
//  Modifications
//   2000/04/14 François BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2004/05/14 Peter Vanroose  Added describe()
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_spatial_object_2d.h>
#include <vcl_list.h>
#include <vcl_iostream.h>

class vsol_group_2d : public vsol_spatial_object_2d
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set of objects that `this' contains
  //---------------------------------------------------------------------------
  vcl_list<vsol_spatial_object_2d_sptr> *storage_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default Constructor: group with no child
  //---------------------------------------------------------------------------
  vsol_group_2d(void);

  //---------------------------------------------------------------------------
  //: Copy constructor.
  //  The objects of the group are not duplicated
  //---------------------------------------------------------------------------
  vsol_group_2d(const vsol_group_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //  The objects of the group are not deleted
  //---------------------------------------------------------------------------
  virtual ~vsol_group_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* clone(void) const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the object `i'
  //  REQUIRE: i>=0 and i<size()
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d_sptr object(unsigned int i) const;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the real type of a group. It is a SPATIALGROUP
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d_type spatial_type(void) const;

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //  REQUIRE: size()>0
  //---------------------------------------------------------------------------
  virtual void compute_bounding_box(void) const; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Return the number of direct children of the group
  //---------------------------------------------------------------------------
  unsigned int size(void) const { return storage_->size(); }

  //---------------------------------------------------------------------------
  //: Return the number of objects of the group
  //---------------------------------------------------------------------------
  unsigned int deep_size(void) const;

  //---------------------------------------------------------------------------
  //: Is `new_object' a child (direct or not) of `this' ?
  //---------------------------------------------------------------------------
  bool is_child(const vsol_spatial_object_2d_sptr &new_object) const;

  //---------------------------------------------------------------------------
  //: Return a platform independent string identifying the class
  //---------------------------------------------------------------------------
  virtual vcl_string is_a() const { return "vsol_group_2d"; }

  //***************************************************************************
  // Element change
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Add an object `new_object'to `this'
  //---------------------------------------------------------------------------
  void add_object(const vsol_spatial_object_2d_sptr &new_object);

  //***************************************************************************
  // Removal
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Remove object `i' of `this' (not delete it)
  //  REQUIRE: i>=0 and i<size()
  //---------------------------------------------------------------------------
  void remove_object(unsigned int i);

  //---------------------------------------------------------------------------
  //: The same behavior than dynamic_cast<>.
  //  Needed because VXL is not compiled with -frtti :-(
  //---------------------------------------------------------------------------
  virtual const vsol_group_2d *cast_to_group(void) const { return this; }
  virtual vsol_group_2d *cast_to_group(void) { return this; }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  inline void describe(vcl_ostream &strm, int blanking=0) const
  {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << "vsol_group_2d of size " << this->size() << ":\n";
    for (vcl_list<vsol_spatial_object_2d_sptr>::const_iterator it = storage_->begin();
         it != storage_->end(); ++it)
      (*it)->describe(strm,blanking+2);
  }
};

#endif // vsol_group_2d_h_
