// This is libname/classname.h
#ifndef classname_h_
#define classname_h_
//:
// \file
// \brief <short description> FIXME
// \author <your name here> FIXME
//
//  <long description> FIXME
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

//: A brief description of the class.
// More information about the class
class classname
{
  // PUBLIC INTERFACE----------------------------------------------------------
 public:

  // Constructors/Destructors--------------------------------------------------

  //: Default constructor
  classname() : data_member_(0) {}
  //: Copy Constructor
  classname(classname const& c) : data_member_(c.data_member()) {}
  //: Constructor from an int
  classname(int v) : data_member_(v) {}
  // Destructor
 ~classname() {}

  // Operators-----------------------------------------------------------------
  classname& operator=(classname const&);
  bool operator==(classname const&) const;

  // Data Access---------------------------------------------------------------

  //: Return data_member
  int data_member() const { return data_member_; }

  // Data Control--------------------------------------------------------------

  //: Set data_member
  void set_data_member(int v) { data_member_ = v; }

  // Utility Methods-----------------------------------------------------------

  // INTERNALS-----------------------------------------------------------------
 protected:
  // Data Members--------------------------------------------------------------
  int data_member_;

 private:
  // Helpers-------------------------------------------------------------------
};

#endif // classname_h_
