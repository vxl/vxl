#ifndef vxl_doc_rules_h_
#define vxl_doc_rules_h_
//-----------------------------------------------------------------------------
//
// .NAME	vxl_doc_rules - Example file of how documentation should look
// everything from the '-' in the previous line is the 'brief' documentation in
// Doxygen
// .LIBRARY	None (not used by Doxygen)
// .HEADER      None (not used by Doxygen)
// .INCLUDE	doc/vxl_doc_rules.h (not used by Doxygen)
// .FILE	vxl_doc_rules.h (not used by Doxygen)
// .FILE	vxl_doc_rules.h (not used by Doxygen)
//
// .SECTION Description
// Here you can write the long description of what's in this file
// Doxygen will add this to the extended documentation of the file
//
// .SECTION Author
//     Maarten Vergauwen
//     Dave Cooper
//     Maybe some other authors
//
// .SECTION Modifications:
//     No modifications yet
//------------------------------------------------------------------------


//: Brief description of the class
// The long description starts here
// \verbatim
// Everything between these tags is not formatted but kept "as is".
// This is similar to the \verbatim command in LaTeX.
// Only for Doxygen
// \endverbatim
// The long description ends here
class my_class {
public:

  //: \brief Brief description, the long one is in the .cxx file
  my_class();

  //: Brief description of the enum.
  // Here comes the detailed description of the enum
  enum vxl_formats {
    VXL_FLOAT,      /*!< This is documentation for this element of the enum */
    VXL_COMPLEX,    /*!< This works for Doxygen only */
    VXL_DOUBLE,     /*!< Remark the needed '<' for this ! */
  };


};

#endif
