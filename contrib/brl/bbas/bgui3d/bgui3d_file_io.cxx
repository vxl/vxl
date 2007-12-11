// This is brl/bbas/bgui3d/bgui3d_file_io.cxx
#include "bgui3d_file_io.h"
//:
// \file

#include <vcl_iostream.h>

#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoToVRMLAction.h>
#include <Inventor/actions/SoToVRML2Action.h>
#include <Inventor/VRMLnodes/SoVRMLGroup.h>


//: Export the scene as IV
void
bgui3d_export_iv( SoNode* scene_root, const vcl_string& filename )
{
  SoOutput out;
  out.openFile(filename.c_str());

  SoWriteAction wra(&out);
  wra.apply(scene_root);

  out.closeFile();
}


//: Export the scene as VRML
void
bgui3d_export_vrml(SoNode* scene_root, const vcl_string& filename)
{
  SoOutput out;
  out.openFile(filename.c_str());
  out.setHeaderString("#VRML V1.0 utf8");

  SoToVRMLAction to_vrml;
  to_vrml.apply(scene_root);
  SoNode *vrml_root = to_vrml.getVRMLSceneGraph();
  vrml_root->ref();

  SoWriteAction wra(&out);
  wra.apply(vrml_root);

  out.closeFile();
  vrml_root->unref();
}


//: Export the scene as VRML 2.0
void
bgui3d_export_vrml2(SoNode* scene_root, const vcl_string& filename)
{
  SoOutput out;
  out.openFile(filename.c_str());
  out.setHeaderString("#VRML V2.0 utf8");

  SoToVRML2Action to_vrml2;
  to_vrml2.apply(scene_root);
  SoNode *vrml2_root = to_vrml2.getVRML2SceneGraph();
  vrml2_root->ref();

  SoWriteAction wra(&out);
  wra.apply(vrml2_root);

  out.closeFile();
  vrml2_root->unref();
}


SoNode* bgui3d_import_file(const vcl_string& filename, vcl_ostream& os)
{
  // Open the input file
  SoInput mySceneInput;
  if (!mySceneInput.openFile(filename.c_str())) {
    os << "Cannot open file "<<filename<<'\n';
    return NULL;
  }

  // Read the whole file into a scenegraph
  SoSeparator *myScene = SoDB::readAll(&mySceneInput);
  if (myScene == NULL) {
    os << "Problem reading file "<< filename << '\n';
    return NULL;
  }
  if (mySceneInput.isFileVRML1())
    os << "Read "<< filename << " as a VRML 1.0 File.\n";
  else if (mySceneInput.isFileVRML2())
    os << "Read "<< filename << " as a VRML 2.0 (VRML97) File.\n";
  else
    os << "Read "<< filename << " as an IV File.\n";

  mySceneInput.closeFile();
  return myScene;
}


#if 0 // Ming: temp test file.
  #include <Inventor/SoDB.h>
  #include <Inventor/SoInteraction.h>
  #include <Inventor/SoInput.h>
  #include <Inventor/SoOutput.h>
  #include <Inventor/actions/SoWriteAction.h>
  #include <Inventor/actions/SoToVRML2Action.h>
  #include <Inventor/nodes/SoSeparator.h>
  #include <Inventor/VRMLnodes/SoVRMLGroup.h>

  int testVRML (int argc, char *argv[])
  {
    SoDB::init();
    SoInteraction::init();
    SoInput in;
    in.openFile(argv[1]);
    vcl_cout << "Reading...\n"
    SoSeparator *root = SoDB::readAll(&in);

    if (root) {
      root->ref();
      SbString hdr = in.getHeader();
      in.closeFile();

      vcl_cout << "Converting...\n"
      SoToVRML2Action tovrml2;
      tovrml2.apply(root);
      SoVRMLGroup *newroot = tovrml2.getVRML2SceneGraph();
      newroot->ref();
      root->unref();

      vcl_cout << "Writing...\n"

      SoOutput out;
      out.openFile("out.wrl");
      out.setHeaderString("#VRML V2.0 utf8");
      SoWriteAction wra(&out);
      wra.apply(newroot);
      out.closeFile();

      newroot->unref();
    }

    return 0;
  }
#endif // 0
