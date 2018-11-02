// This is gel/vsol/vsol_group_2d.h
#ifndef vsol_group_2d_h_
#define vsol_group_2d_h_
//*****************************************************************************
//:
// \file
// \brief Group of spatial objects in a 2D space
//
// \author Francois BERTEL
// \date   2000-04-14
//
// \verbatim
//  Modifications
//   2000-04-14 Francois BERTEL   Creation
//   2000-06-17 Peter Vanroose    Implemented all operator==()s and type info
//   2004-05-14 Peter Vanroose    Added describe()
//   2004-05-16 Joseph Mundy      Added binary I/0
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vector>
#include <iostream>
#include <vsl/vsl_binary_io.h>
#include <vsol/vsol_spatial_object_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsol_group_2d : public vsol_spatial_object_2d
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set of objects that `this' contains
  //---------------------------------------------------------------------------
  std::vector<vsol_spatial_object_2d_sptr> *storage_;

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
  ~vsol_group_2d() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone(void) const override;

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
  vsol_spatial_object_2d_type spatial_type(void) const override;

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //  REQUIRE: size()>0
  //---------------------------------------------------------------------------
  void compute_bounding_box(void) const override; // virtual of vsol_spatial_object_2d

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

  //***************************************************************************
  // Element change
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Add an object `new_object' to `this'
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
  const vsol_group_2d *cast_to_group(void) const override { return this; }
  vsol_group_2d *cast_to_group(void) override { return this; }

  //---------------------------------------------------------------------------
  //: Has `this' the same number of elements and as other and equal elements?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_group_2d &other) const;
  bool operator==(const vsol_spatial_object_2d& obj) const override; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' not the same number of elements and as other and not equal elements?
  //---------------------------------------------------------------------------
  inline bool operator!=(const vsol_group_2d &o)const{return !operator==(o);}

  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const override;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is) override;

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return "vsol_group_2d"; }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const std::string& cls) const { return cls==is_a(); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  inline void describe(std::ostream &strm, int blanking=0) const override
  {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << "vsol_group_2d of size " << this->size() << ":\n";
    for (std::vector<vsol_spatial_object_2d_sptr>::const_iterator it = storage_->begin();
         it != storage_->end(); ++it)
      (*it)->describe(strm,blanking+2);
  }
};


//: Binary save vsol_group_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_group_2d* p);

//: Binary load vsol_group_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_group_2d* &p);

#endif // vsol_group_2d_h_
