//            (This is an example of how to code document your    )
//            ( file. Comments with brackets, like this one,      )
//            ( should be read but not copied into your file.     )
//            (                                                   )
//            (The style used here is specific to VXL             )
//            ( It is based on doxygen, with modifications.       )
//            ( So rather than use                                )
//            (        /** Brief comment                          )
//            (        */                                         )
//            ( notation, we instead use                          )
//            (         //: Brief comment                         )
//            (The main reason for this is that /* */ comment     )
//            ( markers do not nest, so making it hard to quickly )
//            ( comment out sections of code.                     )
//            (Apart from this, you can use full doxygen notation.)
//            (A simple perl script converts this notation        )
//            ( to full doxygen format on the fly.                )
//            (                                                   )
//            (The top line indicates the file name.              )
//            ( The \file command below tells Doxygen which       )
//            ( file the comments correspond to, so should        )
//            ( be left blank in general to minimise the          )
//            ( risks of mistakes, e.g., when files are renamed.  )

// This is core/doc/vxl_doc_rules.h
#ifndef vxl_doc_rules_h_
#define vxl_doc_rules_h_
//:
// \file
// \brief give a brief description of the file.
// \author Whoever, (erehwon)
// \date 01/01/01
// \author Someone else
//   Possibly multiple lines.  Don't forget to end the \author, \date and
//   \brief sections with an empty line (i.e., just //).
//
// Here you can write the long description of what's in this file.
// Doxygen will add this to the extended documentation of the file.
//
// \verbatim
//  Modifications
//   Here goes a chronological list of file modifications (with author and date)
// \endverbatim

#include <parent_class.h> // includes go here; avoid unnecessary includes!

//: Brief description of the class
// The long description starts here
// \verbatim
//  Everything between these tags is not formatted but kept "as is".
//  This is similar to the verbatim command in LaTeX.
// \endverbatim
// For a list of items, you do not need verbatim:
// - first item (possibly multi-line)
// - second item
// For code fragments, use the following instead of verbatim:
// \code
//   // This is how to use my_class:
//   my_class a;
//   a.my_value = 1.0;
// \endcode
// The long description ends here

class my_class : public parent_class
{
  //: Brief description of the member variable.
  double my_value;

 public:
  //: Brief description of method. Don't forget the colon, and don't continue on next line.
  // Do not leave any blank lines without the //, or doxygen
  // will think you have ended the documentation comment, and
  // started an ordinary code comment.
  //
  // If you want to start a new paragraph, put a line with just //
  //
  // If you want to do some special stuff such as formulas, have
  // a look at the doxygen documentation http://www.doxygen.org/
  // Be careful, doxygen is not very tolerant: make sure you test build the
  // documentation after using any of the advanced stuff.
  my_class();

  //: Brief description of the enum.
  // Here comes the detailed description of the enum.
  enum vxl_formats {
    VXL_FLOAT,      //!< This is documentation for this element of the enum.
    VXL_COMPLEX,    //!< This works for Doxygen only.
    VXL_DOUBLE      //!< Remark the needed '!<' for this !
  };
};

#endif // vxl_doc_rules_h_
