// This is gel/vsol/vsol_group_3d.h
#ifndef vsol_group_3d_h_
#define vsol_group_3d_h_
//*****************************************************************************
//:
// \file
// \brief Group of spatial objects in a 3D space
//
// \author François BERTEL
// \date   2000/05/03
//
// \verbatim
//  Modifications
//   2000/05/03 François BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2004/05/14 Peter Vanroose  Added describe()
//   2004/09/06 Peter Vanroose  Added operator==()
//   2004/09/06 Peter Vanroose  Added Binary I/O
// \endverbatim
//*****************************************************************************

#include <vsl/vsl_binary_io.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

class vsol_group_3d : public vsol_spatial_object_3d
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  // Description: Set of objects that `this' contains
  //---------------------------------------------------------------------------
  vcl_vector<vsol_spatial_object_3d_sptr> *storage_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default Constructor: group with no child
  //---------------------------------------------------------------------------
  vsol_group_3d(void);

  //---------------------------------------------------------------------------
  //: Copy constructor.
  //  The objects of the group are not duplicated
  //---------------------------------------------------------------------------
  vsol_group_3d(vsol_group_3d const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //  The objects of the group are not deleted
  //---------------------------------------------------------------------------
  virtual ~vsol_group_3d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d* clone(void) const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the object `i'
  //  REQUIRE: i>=0 and i<size()
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d_sptr object(unsigned int i) const;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the real type of a group. It is a SPATIALGROUP
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d_type spatial_type(void) const;

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //  REQUIRE: size()>0
  //---------------------------------------------------------------------------
  virtual void compute_bounding_box(void) const; // virtual of vsol_spatial_object_3d

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
  bool is_child(vsol_spatial_object_3d_sptr const& new_object) const;

  //***************************************************************************
  // Element change
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Add an object `new_object'to `this'
  //---------------------------------------------------------------------------
  void add_object(vsol_spatial_object_3d_sptr const& new_object);

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
  //  Needed because VXL is not necessarily compiled with -frtti
  //---------------------------------------------------------------------------
  virtual vsol_group_3d const* cast_to_group(void) const { return this; }
  virtual vsol_group_3d *cast_to_group(void) { return this; }

  //---------------------------------------------------------------------------
  //: Has `this' the same number of elements and as other and equal elements?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_group_3d const& other) const;
  virtual bool operator==(vsol_spatial_object_3d const& obj) const; // virtual of vsol_spatial_object_3d

  //---------------------------------------------------------------------------
  //: Has `this' not the same number of elements and as other and not equal elements?
  //---------------------------------------------------------------------------
  inline bool operator!=(vsol_group_3d const& o)const{return !operator==(o);}

  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //---------------------------------------------------------------------------
  //: Return a platform independent string identifying the class
  //---------------------------------------------------------------------------
  virtual vcl_string is_a() const { return "vsol_group_3d"; }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const vcl_string& cls) const { return cls==is_a(); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  inline void describe(vcl_ostream &strm, int blanking=0) const
  {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << "vsol_group_3d of size " << this->size() << ":\n";
    for (vcl_vector<vsol_spatial_object_3d_sptr>::const_iterator it = storage_->begin();
         it != storage_->end(); ++it)
      (*it)->describe(strm,blanking+2);
  }
};

//: Binary save vsol_group_3d* to stream.
void vsl_b_write(vsl_b_ostream &os, vsol_group_3d const* p);

//: Binary load vsol_group_3d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_group_3d* &p);

#endif // vsol_group_3d_h_
