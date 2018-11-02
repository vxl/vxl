// This is brl/bbas/bgui3d/bgui3d_file_io.h
#ifndef bgui3d_file_io_h_
#define bgui3d_file_io_h_
//:
// \file
// \brief Read and write scene graphs as IV or VRML files
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date May 27, 2004
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <string>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// forward declarations
class SoNode;

//: Export the scene as IV
void bgui3d_export_iv( SoNode* scene_root, const std::string& filename );

//: Export the scene as VRML
void bgui3d_export_vrml( SoNode* scene_root, const std::string& filename);

//: Export the scene as VRML 2.0
void bgui3d_export_vrml2( SoNode* scene_root, const std::string& filename);


//: Read a file and parse into a scenegraph
// \returns the root node of the scenegraph or NULL on failure
// \note currently handles IV, VRML, and VRML2 files
SoNode* bgui3d_import_file(const std::string& filename, std::ostream& os = std::cout);


#endif // bgui3d_file_io_h_
