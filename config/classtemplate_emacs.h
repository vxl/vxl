// This is @(eval (file-name-nondirectory (file-name-directory-noslash dirname)))/@(eval dirbase)/${FILESTEM}.h
#ifndef ${FILESTEM}_h_
#define ${FILESTEM}_h_
//:
// \file
// \brief <short description> FIXME
// \author @(eval (user-full-name)), ${DATE}
//
//  <long description!!> FIXME
//
// \verbatim
//  Example
//   @(eval (file-name-nondirectory (file-name-directory-noslash dirname)))/@(eval dirbase)/examples/${FILESTEM}_example.cxx FIXME
// \endverbatim
//
// \verbatim
//  See also
//   <list of related classes> FIXME
// \endverbatim
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

//: A brief description of the class.
// More information about the class
class ${FILESTEM}
{
  // PUBLIC INTERFACE----------------------------------------------------------
 public:

  // Constructors/Destructors--------------------------------------------------

  //: Default constructor
  ${FILESTEM}() : data_member_(0) {}
  //: Copy Constructor
  ${FILESTEM}(${FILESTEM} const& c) : data_member_(c.data_member()) {}
  //: Constructor from an int
  ${FILESTEM}(int v) : data_member_(v) {}
  // Destructor
 ~${FILESTEM}() {}

  // Operators-----------------------------------------------------------------
  ${FILESTEM}& operator=(${FILESTEM} const&);
  bool operator==(${FILESTEM} const&) const;

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

#endif // ${FILESTEM}_h_
