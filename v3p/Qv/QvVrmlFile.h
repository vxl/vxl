// This is v3p/Qv/QvVrmlFile.h
#ifndef QvVrmlFile_h_
#define QvVrmlFile_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME QvVrmlFile - Parse VRML 1.0 file
// .INCLUDE QvVrmlFile.h
// .FILE QvVrmlFile.cxx
//
// .SECTION Description
//    QvQvVrmlFile is used to parse a VRML 1.0 file, so that the QvVisitor
//    can be used later to traverse it.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Jan 99
//
// .SECTION Modifications:
//   990109 AWF Initial version.
//
//-----------------------------------------------------------------------------

struct QvVrmlFileData;
class  QvVisitor;
class  QvNode;

class QvVrmlFile
{
 public:
  // Constructors/Destructors--------------------------------------------------

  QvVrmlFile(char const* filename);
  ~QvVrmlFile();

  bool load(char const* filename);
  char const* get_filename();

  void traverse(QvVisitor* visitor);
  void draw();
  void compute_centroid_radius();

  float centroid[3];
  float radius;

 protected:
  // Data Members--------------------------------------------------------------
  QvVrmlFileData* p;

  void traverse(QvVisitor* visitor, QvNode* node, int indent);

  // Helpers-------------------------------------------------------------------
};

#endif // QvVrmlFile_h_
