// This is v3p/Qv/QvVrmlFile.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "QvVrmlFile.h"

#include <vcl_cmath.h> // for vcl_sqrt()
#include <vcl_cstdio.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

#include "QvVisitor.h"
#include "QvString.h"
#include "QvInput.h"
#include "QvState.h"
#include "QvNode.h"

#include "QvGroup.h"
#include "QvSeparator.h"
#include "QvSwitch.h"

//#include <vbl/vbl_printf.h>
//#include <vbl/vbl_file.h>

struct QvVrmlFileData {
  vcl_vector<QvNode*> nodes;
  vcl_string filename_;
};

// Default ctor
QvVrmlFile::QvVrmlFile(char const* filename):
  p(new QvVrmlFileData)
{
  load(filename);
}

QvVrmlFile::~QvVrmlFile()
{
  delete p;
}

bool QvVrmlFile::load(char const* filename)
{
  p->filename_ = filename;
  vcl_FILE* fp = vcl_fopen(filename, "r");
  if (!fp) {
    vcl_cerr << "QvVrmlFile::load: Can't open [" << filename << "]\n";
    return false;
  }

  // make QvInput
  QvInput in;
  in.setFilePointer(fp);

  //TopologyHierarchyNode::DEFER_SUPERIORS = true;

  vcl_cerr << "VRML_IO: ";
  while (true)
  {
    QvNode* node = 0;
    vcl_cerr << 'R';
    if (!QvNode::read(&in, node)) break;
    if (!node) break;
    QvState state;
    vcl_cerr << 'B';
    node->build(&state);
    p->nodes.push_back(node);
    vcl_cerr << ' ';
  }
  vcl_cerr << "\nVRML_IO: Loaded " << p->nodes.size() << " topology objects\n";
  return true;
}

char const* QvVrmlFile::get_filename()
{
  return p->filename_.c_str();
}

void QvVrmlFile::traverse(QvVisitor* visitor)
{
  for (vcl_vector<QvNode*>::iterator np = p->nodes.begin(); np != p->nodes.end(); ++np)
    visitor->Visit(*np);
}

#include "QvPointSet.h"
#include "QvIndexedLineSet.h"
#include "QvIndexedFaceSet.h"

struct VrmlCentroidVisitor : public QvVisitor
{
  float centroid[3];
  int n;
  int pass;

  double radius;

  void visit(QvVrmlFile& f)
  {
    pass = 0;
    centroid[0] = centroid[1] = centroid[2] = 0;
    n = 0;
    f.traverse(this);
    if (n == 0) {
      radius = 1;
      return;
    }

    centroid [0] *= 1.0f/n;
    centroid [1] *= 1.0f/n;
    centroid [2] *= 1.0f/n;

    pass = 1;
    radius = 0;
    f.traverse(this);
    radius = vcl_sqrt(radius);
  }

  void inc(const point3D& p)
  {
    if (pass == 0) {
      centroid[0] += p.x;
      centroid[1] += p.y;
      centroid[2] += p.z;
      ++n;
    } else {
      double dx = p.x - centroid[0];
      double dy = p.y - centroid[1];
      double dz = p.z - centroid[2];
      double d = dx*dx+dy*dy+dz*dz;
      if (d > radius)
        radius = d;
    }
  }

  // ----

  bool Visit(QvPointSet* ps)
  {
    int nn = (ps->numPoints.value == -1) ? ps->num_ : ps->numPoints.value;
    nn += ps->startIndex.value;
    for (int i = ps->startIndex.value; i < nn; ++i)
      inc(ps->points_[i]);
    return true; // ??
  }

  bool Visit(QvIndexedLineSet* node)
  {
    const point3D* vertexlist = node->vertexlist_;   // vertex data
    int numvertinds = node->numvertinds_;            // no. of vertex indices
    const int* vertindices = node->vertindices_;     // vertex index list

    for (int j = 0; j < numvertinds-1; ++j) {
      int i1 = vertindices[j];
      if (i1 != -1) inc(vertexlist[i1]);
    }
    return true; // ??
  }

  bool Visit(QvIndexedFaceSet* node)
  {
    for (int i = 0; i < node->numvertinds_; ++i) {
      int vert_index = node->vertindices_[i];
      if (vert_index != -1) inc(node->vertexlist_[vert_index]);
    }
    return true; // ??
  }

  bool Visit(QvNode*) { return false; }
  bool Visit(QvGroup*) { return false; }
  bool Visit(QvAsciiText*) { return false; }
  bool Visit(QvCone*) { return false; }
  bool Visit(QvCoordinate3*) { return false; }
  bool Visit(QvCube*) { return false; }
  bool Visit(QvCylinder*) { return false; }
  bool Visit(QvDirectionalLight*) { return false; }
  bool Visit(QvFontStyle*) { return false; }
  bool Visit(QvInfo*) { return false; }
  bool Visit(QvLOD*) { return false; }
  bool Visit(QvMaterial*) { return false; }
  bool Visit(QvMaterialBinding*) { return false; }
  bool Visit(QvMatrixTransform*) { return false; }
  bool Visit(QvNormal*) { return false; }
  bool Visit(QvNormalBinding*) { return false; }
  bool Visit(QvOrthographicCamera*) { return false; }
  bool Visit(QvPerspectiveCamera*) { return false; }
  bool Visit(QvPointLight*) { return false; }
  bool Visit(QvRotation*) { return false; }
  bool Visit(QvScale*) { return false; }
  bool Visit(QvSeparator*) { return false; }
  bool Visit(QvShapeHints*) { return false; }
  bool Visit(QvSphere*) { return false; }
  bool Visit(QvSpotLight*) { return false; }
  bool Visit(QvSwitch*) { return false; }
  bool Visit(QvTexture2*) { return false; }
  bool Visit(QvTexture2Transform*) { return false; }
  bool Visit(QvTextureCoordinate2*) { return false; }
  bool Visit(QvTransform*) { return false; }
  bool Visit(QvTransformSeparator*) { return false; }
  bool Visit(QvTranslation*) { return false; }
  bool Visit(QvWWWAnchor*) { return false; }
  bool Visit(QvWWWInline*) { return false; }
  bool Visit(QvUnknownNode*) { return false; }
  bool Visit(QvLabel*) { return false; }
  bool Visit(QvLightModel*) { return false; }
};

void QvVrmlFile::compute_centroid_radius()
{
  VrmlCentroidVisitor vcv;
  vcv.visit(*this);
  centroid[0] = vcv.centroid[0];
  centroid[1] = vcv.centroid[1];
  centroid[2] = vcv.centroid[2];
  radius = (float)vcv.radius;
  vcl_cerr << "QvVrmlFile::compute_centroid_radius: c = "
           << centroid[0] << ' ' << centroid[1] << ' ' << centroid[2]
           << ", r = " << radius << vcl_endl;
}
