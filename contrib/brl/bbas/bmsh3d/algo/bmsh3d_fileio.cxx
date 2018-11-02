// This is brl/bbas/bmsh3d/algo/bmsh3d_fileio.cxx
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <cstring>
#include "bmsh3d_fileio.h"
//:
// \file
// \brief 3D mesh file I/O
//
// \author MingChing Chang
// \date   Feb 10, 2005
//
// \verbatim
//  Modifications
//    June 13, 2005   Nhon Trinh     implemented bmsh3d_load_ply(...)
// \endverbatim
//
//-------------------------------------------------------------------------

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

// #################################################################
//    POINT CLOUD FILE I/O
// #################################################################

// ============================== XYZ ==============================

bool bmsh3d_load_xyz(bmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .XYZ file " << file << '\n';
#endif // DEBUG
    return false;
  }
  assert (pointset->vertexmap().size() == 0);

  // Read in (x, y, z) points.
  int ret = 0;
  do {
    double x, y, z;
    ret = std::fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
    if (ret != EOF) {
      if (ret != 3) return false;
      bmsh3d_vertex* V = pointset->_new_vertex();
      V->get_pt().set(x, y, z);
      pointset->_add_vertex(V);
    }
  }
  while (ret != EOF);

  std::cerr << "  " << pointset->vertexmap().size()
           << " points loaded from " << file << ".\n";

  std::fclose(fp);
  return true;
}

bool bmsh3d_save_xyz(bmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .XYZ file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " : "
           << pointset->vertexmap().size() << " points ...\n";

  auto it = pointset->vertexmap().begin();
  for (; it != pointset->vertexmap().end(); ++it) {
    bmsh3d_vertex* v = (*it).second;
    std::fprintf(fp, "%.16f %.16f %.16f\n", v->pt().x(), v->pt().y(), v->pt().z());
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

bool bmsh3d_load_xyz(std::vector<vgl_point_3d<double> >& pts, const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .XYZ file " << file << '\n';
#endif // DEBUG
    return false;
  }
  assert (pts.size() == 0);

  // Read in (x, y, z) points.
  int ret;
  vgl_point_3d<double> P;
  do {
    double x, y, z;
    ret = std::fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
    if (ret != EOF) {
      if (ret != 3) return false;
      P.set(x, y, z);
      pts.push_back(P);
    }
  }
  while (ret != EOF);

  std::cerr << "  " <<  pts.size() << " points loaded from " << file << ".\n";

  std::fclose(fp);
  return true;
}

bool bmsh3d_save_xyz(std::vector<vgl_point_3d<double> >& pts, const char* file)
{
  std::FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .XYZ file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " : " << pts.size() << " points ...\n";

  auto it = pts.begin();
  for (; it != pts.end(); ++it) {
    vgl_point_3d<double> P = (*it);
    std::fprintf(fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

bool bmsh3d_load_xyz(std::vector<std::pair<int, vgl_point_3d<double> > >& idpts, const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .XYZ file " << file << '\n';
#endif // DEBUG
    return false;
  }
  assert (idpts.size() == 0);

  // Read in (x, y, z) points.
  int ret;
  int id = 0;
  vgl_point_3d<double> P;
  do {
    double x, y, z;
    ret = std::fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
    if (ret != EOF) {
      if (ret != 3) return false;
      P.set(x, y, z);
      idpts.emplace_back(id, P);
      ++id;
    }
  }
  while (ret != EOF);

  std::cerr << "  " << idpts.size() << " points loaded from " << file << ".\n";

  std::fclose(fp);
  return true;
}

bool bmsh3d_save_xyz(std::vector<std::pair<int, vgl_point_3d<double> > >& idpts, const char* file)
{
  std::FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .XYZ file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " : " << idpts.size() << " points ...\n";

  auto it = idpts.begin();
  for (; it != idpts.end(); ++it) {
    vgl_point_3d<double> P = (*it).second;
    std::fprintf(fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

//: Oriented points: point + normal.
bool bmsh3d_load_xyzn1(std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                       const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .XYZN1 file " << file << '\n';
#endif // DEBUG
    return false;
  }
  assert (ori_pts.size() == 0);

  // Read in # points
  unsigned int num_pts;
  int ret = std::fscanf(fp, "%d\n", &num_pts); if (ret != 1) return false;

  // Read in (x, y, z) points.
  vgl_point_3d<double> P;
  vgl_vector_3d<double> N;
  do {
    double x, y, z, nx, ny, nz;
    // w is the local feature size, not used.
    ret = std::fscanf(fp, "%lf %lf %lf %lf %lf %lf\n", &x, &y, &z, &nx, &ny, &nz);
    if (ret != EOF) {
      if (ret != 6) return false;
      P.set(x, y, z);
      N.set(nx, ny, nz);
      ori_pts.emplace_back(P, N);
    }
  }
  while (ret != EOF);

  assert (ori_pts.size() == num_pts);

  std::cerr << "  " << ori_pts.size()
           << " oriented points loaded from " << file << ".\n";

  std::fclose(fp);
  return true;
}

bool bmsh3d_save_xyzn1(std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                       const char* file)
{
  std::FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .XYZN1 file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " : "
           << ori_pts.size() << " oriented points ...\n";

  // # of points.
  std::fprintf(fp, "%ld\n", (long)ori_pts.size());

  auto it = ori_pts.begin();
  for (; it != ori_pts.end(); ++it) {
    vgl_point_3d<double> P = (*it).first;
    vgl_vector_3d<double> N = (*it).second;
    std::fprintf(fp, "%.16f %.16f %.16f %.16f %.16f %.16f\n",
                P.x(), P.y(), P.z(), N.x(), N.y(), N.z());
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

//: Oriented points: point + normal + local feature size.
bool bmsh3d_load_xyznw(std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                       const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .XYZNW file " << file << '\n';
#endif // DEBUG
    return false;
  }
  assert (ori_pts.size() == 0);

  // Read in (x, y, z) points.
  int ret;
  vgl_point_3d<double> P;
  vgl_vector_3d<double> N;
  do {
    double x, y, z, nx, ny, nz, w;
    // w is the local feature size, not used.
    ret = std::fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf\n", &x, &y, &z, &nx, &ny, &nz, &w);
    if (ret != EOF) {
      if (ret != 7) return false;
      P.set(x, y, z);
      N.set(nx, ny, nz);
      ori_pts.emplace_back(P, N);
    }
  }
  while (ret != EOF);

  std::cerr << "  " << ori_pts.size()
           << " oriented points loaded from " << file << ".\n";

  std::fclose(fp);
  return true;
}

bool bmsh3d_save_xyznw_vector(std::vector<std::pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& oriented_points,
                              const char* file)
{
  std::FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .XYZNW file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " : "
           << oriented_points.size() << " oriented points ...\n";

  auto it = oriented_points.begin();
  for (; it != oriented_points.end(); ++it) {
    vgl_point_3d<double> P = (*it).first;
    vgl_vector_3d<double> N = (*it).second;
    double w = 1.0;
    std::fprintf(fp, "%.16f %.16f %.16f %.16f %.16f %.16f %lf\n",
                P.x(), P.y(), P.z(), N.x(), N.y(), N.z(), w);
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

// ============================== P3D ==============================

bool bmsh3d_load_p3d(bmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .P3D file " << file << '\n';
#endif // DEBUG
    return false;
  }

  assert (pointset->vertexmap().size() == 0); // clear();

  unsigned int dim;
  int ret = std::fscanf(fp, "%u\n", &dim); if (ret!=1) return false;
  assert (dim==3);
  unsigned int numGenes;
  ret = std::fscanf(fp, "%u\n", &numGenes); if (ret!=1) return false;

  std::cerr << "  loading " << file << " : " << numGenes << " points ...\n";

  for (unsigned int i=0; i<numGenes; ++i) {
    bmsh3d_vertex* v = pointset->_new_vertex();
    double x, y, z;
    ret = std::fscanf(fp, "%lf %lf %lf\n", &x, &y, &z); if (ret!=3) return false;
    v->get_pt().set (x, y, z);

    pointset->_add_vertex(v);
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

bool bmsh3d_save_p3d(bmsh3d_pt_set* pointset, const char* file)
{
  std::FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .P3D file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " : "
           << pointset->vertexmap().size() << " points ...\n";

  std::fprintf(fp, "3\n%lu\n", (long unsigned)pointset->vertexmap().size());

  auto it = pointset->vertexmap().begin();
  for (; it != pointset->vertexmap().end(); ++it) {
    bmsh3d_vertex* v = (*it).second;
    std::fprintf(fp, "%.16f %.16f %.16f\n", v->pt().x(), v->pt().y(), v->pt().z());
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

bool bmsh3d_load_p3d(std::vector<vgl_point_3d<double> >& pts, const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .P3D file " << file << '\n';
#endif // DEBUG
    return false;
  }
  assert (pts.size() == 0);

  unsigned int dim;
  int ret = std::fscanf(fp, "%u\n", &dim); if (ret!=1) return false;
  assert (dim==3);
  unsigned int numGenes;
  ret = std::fscanf(fp, "%u\n", &numGenes); if (ret!=1) return false;
  pts.resize (numGenes);
  std::cerr << "  loading " << file << " : "
           << numGenes << " points into a vector storage...\n";

  for (unsigned int i=0; i<numGenes; ++i) {
    vgl_point_3d<double> P;
    double x, y, z;
    ret = std::fscanf(fp, "%lf %lf %lf\n", &x, &y, &z); if (ret!=3) return false;
    P.set (x, y, z);
    pts[i] = P;
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

bool bmsh3d_save_p3d(std::vector<vgl_point_3d<double> >& pts, const char* file)
{
  std::FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .P3D file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " : " << pts.size() << " points ...\n";

  std::fprintf(fp, "3\n%lu\n", (long unsigned)pts.size());

  auto it = pts.begin();
  for (; it != pts.end(); ++it) {
    vgl_point_3d<double> P = (*it);
    std::fprintf(fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

bool bmsh3d_load_p3d(std::vector<std::pair<int, vgl_point_3d<double> > >& idpts,
                     const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .P3D file " << file << '\n';
#endif // DEBUG
    return false;
  }

  assert (idpts.size() == 0);

  unsigned int dim;
  int ret = std::fscanf(fp, "%u\n", &dim); if (ret!=1) return false;
  assert (dim==3);
  unsigned int numGenes;
  ret = std::fscanf(fp, "%u\n", &numGenes); if (ret!=1) return false;

  std::cerr << "  loading " << file << " : "
           << numGenes << " points into a vector storage...\n";

  for (unsigned int i=0; i<numGenes; ++i) {
    vgl_point_3d<double> P;
    double x, y, z;
    ret = std::fscanf(fp, "%lf %lf %lf\n", &x, &y, &z); if (ret!=3) return false;
    P.set (x, y, z);

    idpts.emplace_back(i, P);
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

bool bmsh3d_save_p3d(std::vector<std::pair<int, vgl_point_3d<double> > >& idpts, const char* file)
{
  std::FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .P3D file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " : " << idpts.size() << " points ...\n";

  std::fprintf(fp, "3\n%lu\n", (long unsigned)idpts.size());

  auto it = idpts.begin();
  for (; it != idpts.end(); ++it) {
    vgl_point_3d<double> P = (*it).second;
    std::fprintf(fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}

// #################################################################################

//: Open the file and read in # of dbsk3d_bnd_pts
int read_num_genes_sphere_from_file(const char* pcFile_P3D)
{
  FILE* fp;
  if (!(fp = std::fopen(pcFile_P3D, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .P3D file " << file << '\n';
#endif // DEBUG
    return false;
  }

  int dim;
  int ret = std::fscanf(fp, "%d\n", &dim); if (ret!=1) return false;
  assert (dim==3);
  int numGenes;
  ret = std::fscanf(fp, "%d\n", &numGenes); if (ret!=1) return false;
  std::fclose(fp);

  std::cerr << "  reading data from " << pcFile_P3D << ": "
           << numGenes << " original genes.\n";
  return numGenes;
}

bool dbsk3d_load_con(bmsh3d_pt_set* geneset, const char* pcFile_CON, double z)
{
#define MAX_LEN 1024

  double x, y;
  char buffer[MAX_LEN];
  int nPoints;

  // 1)If file open fails, return.
  std::ifstream fp (pcFile_CON, std::ios::in);
  if (!fp) {
    std::cout<<" : Unable to Open "<<pcFile_CON<<std::endl;
    return false;
  }

  // need to release the memory!!
  geneset->vertexmap().clear();

  // 2)Read in file header.
  fp.getline(buffer,MAX_LEN); // CONTOUR
  fp.getline(buffer,MAX_LEN); // OPEN/CLOSE
  fp >> nPoints;
  std::cout << "Number of Points from Contour:" << nPoints << std::endl;

  // 3)Read in 'nPoints' of lines of data.
  for (int i=0; i<nPoints; ++i) {
    fp >> x >> y;
    bmsh3d_vertex *v = geneset->_new_vertex();
#if 0
    assert(x > -100000);
    v->set_id(i);
#endif // 0
    v->get_pt().set (x, y, z);

    geneset->_add_vertex(v);
  }

  fp.close();
  std::cerr << "  done.\n";
  return true;
}

// #################################################################
//    MESH
// #################################################################

bool save_unmeshed_p3d(bmsh3d_mesh* M, const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .P3D file " << file << '\n';
#endif // DEBUG
    return false;
  }
  unsigned int num = M->count_unmeshed_pts();
  if (num == 0)
    return false;

  std::cerr << "  saving unmeshed " << num << " points to " << file << "...\n";
  std::fprintf(fp, "3\n%u\n", num);

  auto it = M->vertexmap().begin();
  unsigned int count=0;
  for (; it != M->vertexmap().end(); ++it) {
    bmsh3d_vertex* v = (*it).second;
    if (! v->b_meshed()) {
      std::fprintf(fp, "%f %f %f\n", v->pt().x(), v->pt().y(), v->pt().z());
      ++count;
    }
  }

  assert (count == num);
  std::fclose(fp);
  return true;
}

bool bmsh3d_load_ply2(std::vector<vgl_point_3d<double> >& /*pts*/,
                      std::vector<std::vector<int> >* /*faces*/,
                      const char* /*file*/)
{
  assert (0);
  return false;
}

bool bmsh3d_save_ply2(const std::vector<vgl_point_3d<double> >& pts,
                      const std::vector<std::vector<int> >& faces,
                      const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .PLY2 file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " :\n\t" << pts.size() << " points, "
           << faces.size() << " faces ...\n";

  std::fprintf(fp, "%lu\n", (long unsigned)pts.size());
  std::fprintf(fp, "%lu\n", (long unsigned)faces.size());

  for (const auto & pt : pts) {
    std::fprintf(fp, "%.16f ", pt.x());
    std::fprintf(fp, "%.16f ", pt.y());
    std::fprintf(fp, "%.16f\n",pt.z());
  }

  for (const auto & face : faces) {
    std::fprintf(fp, "%lu", (long unsigned)face.size());
    assert (face.size() != 0);
    for (unsigned int j=0; j<face.size(); ++j)
      std::fprintf(fp, " %d", face[j]);
    std::fprintf(fp, "\n");
  }

  std::fclose(fp);
  std::cerr << "  done.\n";
  return true;
}


// ===================== GENERIC ==================================

//: Load a mesh files (only PLY and PLY2 are currently supported)
bool bmsh3d_load(bmsh3d_mesh* M, const char* file, const char* format)
{
  std::string file_format(format);
  file_format = "." + file_format;

  // if no type is given, determine it using file extension
  if (file_format == ".DEFAULT")
  {
    file_format = vul_file::extension(file);
    vul_string_upcase(file_format);
  }

  std::cout << "format = " << file_format << '\n';
  // call appropriate load functions to load the M
  if (file_format == ".PLY")
  {
    return bmsh3d_load_ply(M, file);
  }

  if (file_format == ".PLY2")
  {
    return bmsh3d_load_ply2(M, file);
  }

  std::cout << "  loading failed. Unknown file format.\n";
  return false;
}

// ============================= PLY2 ==============================

bool bmsh3d_save_ply2(bmsh3d_mesh* M, const char* file)
{
  FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .PLY2 file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " :\n\t" << M->vertexmap().size()
           << " points, " << M->facemap().size() << " faces ...\n";

  std::fprintf(fp, "%lu\n", (long unsigned)M->vertexmap().size());
  std::fprintf(fp, "%lu\n", (long unsigned)M->facemap().size());

  // Use v->vid() to re-index vertices, starting with id 0.
  int vidcounter = 0;
  auto it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); ++it) {
    bmsh3d_vertex* V = (*it).second;
    V->set_vid(vidcounter++);

    std::fprintf(fp, "%.16f ", V->pt().x());
    std::fprintf(fp, "%.16f ", V->pt().y());
    std::fprintf(fp, "%.16f\n",V->pt().z());
  }

  auto fit = M->facemap().begin();
  for (; fit != M->facemap().end(); ++fit) {
    bmsh3d_face* F = (*fit).second;
    F->_ifs_track_ordered_vertices();

    std::fprintf(fp, "%lu", (long unsigned)F->vertices().size());
    for (unsigned j=0; j<F->vertices().size(); ++j) {
      bmsh3d_vertex* V = F->vertices(j);
      std::fprintf(fp, " %d", V->vid());
    }
    std::fprintf(fp, "\n");
  }
  std::fclose(fp);
  std::cerr << "  done.\n";

  return true;
}

bool bmsh3d_load_ply2(bmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;
  assert (M->vertexmap().size() == 0);
  assert (M->facemap().size() == 0);

  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .PLY2 file " << file << '\n';
#endif // DEBUG
    return false;
  }

  unsigned int vertex_N, face_N;
  int ret = std::fscanf(fp, "%d\n", &vertex_N); if (ret!=1) return false;
      ret = std::fscanf(fp, "%d\n", &face_N); if (ret!=1) return false;

  std::cerr << "  loading " << file << " :\n\t" << vertex_N << " points, "
           << face_N << " faces ...\n";

  double p[3];
  for (unsigned int i=0; i<vertex_N; ++i) {
    bmsh3d_vertex* point = M->_new_vertex();

    ret = std::fscanf(fp, "%lf ", &p[0]); if (ret!=1) return false;
    ret = std::fscanf(fp, "%lf ", &p[1]); if (ret!=1) return false;
    ret = std::fscanf(fp, "%lf ", &p[2]); if (ret!=1) return false;
    ret = std::fscanf(fp, "\n"); if (ret!=0) return false;
    point->get_pt().set (p);

    M->_add_vertex(point);
  }

  for (unsigned int i=0; i<face_N; ++i) {
    bmsh3d_face* F = M->_new_face();

    int num_pt_per_face;
    ret = std::fscanf(fp, "%d", &num_pt_per_face); if (ret!=1) return false;
    for (int j=0; j<num_pt_per_face; ++j) {
      int ind;
      ret = std::fscanf(fp, " %d", &ind); if (ret!=1) return false;

      bmsh3d_vertex* V = M->vertexmap(ind);
      F->_ifs_add_vertex(V);
      V->set_meshed(true);
    }
    ret = std::fscanf(fp, "\n"); if (ret!=0) return false;

    M->_add_face(F);
  }
  std::fclose(fp);
  std::cerr << "\tdone.\n";

  return true;
}

bool bmsh3d_load_ply2_v(bmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;
  assert (M->vertexmap().size() == 0);
  assert (M->facemap().size() == 0);

  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .PLY2 file " << file << '\n';
#endif // DEBUG
    return false;
  }
  unsigned int vertex_N, face_N;
  int ret = std::fscanf(fp, "%d\n", &vertex_N); if (ret!=1) return false;
      ret = std::fscanf(fp, "%d\n", &face_N); if (ret!=1) return false;
  std::cerr << "  bmsh3d_load_ply2_v() " << file << ":\n\t"
           << vertex_N << " points, " << face_N << " faces ...\n";

  double p[3];
  for (unsigned int i=0; i<vertex_N; ++i) {
    bmsh3d_vertex* V = M->_new_vertex();
    ret = std::fscanf(fp, "%lf ", &p[0]); if (ret!=1) return false;
    ret = std::fscanf(fp, "%lf ", &p[1]); if (ret!=1) return false;
    ret = std::fscanf(fp, "%lf ", &p[2]); if (ret!=1) return false;
    ret = std::fscanf(fp, "\n"); if (ret!=0) return false;
    V->get_pt().set (p);
    M->_add_vertex(V);
  }

  std::fclose(fp);
  std::cerr << "  " << vertex_N << " vertices loaded.\n";
  return true;
}

#define TINY_ERROR  1E-3

bool bmsh3d_load_ply2_f(bmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;

  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .PLY2 file " << file << '\n';
#endif // DEBUG
    return false;
  }

  unsigned int vertex_N, face_N;
  int ret = std::fscanf(fp, "%d\n", &vertex_N); if (ret!=1) return false;
  ret = std::fscanf(fp, "%d\n", &face_N); if (ret!=1) return false;

  std::cerr << "  bmsh3d_load_ply2_f(" << file << "):\n"
           << "\tSkip " << vertex_N << " points and load " << face_N << " faces ...\n";

  assert (M->vertexmap().size() == vertex_N);
  double p[3];
  for (unsigned int i=0; i<vertex_N; ++i) {
    bmsh3d_vertex* V = M->vertexmap(i);
    ret = std::fscanf(fp, "%lf ", &p[0]); if (ret!=1) return false;
    ret = std::fscanf(fp, "%lf ", &p[1]); if (ret!=1) return false;
    ret = std::fscanf(fp, "%lf ", &p[2]); if (ret!=1) return false;
    ret = std::fscanf(fp, "\n");          if (ret!=0) return false;
    // assert that M has exactly these vertices.
    assert (std::fabs(V->pt().x() - p[0]) < TINY_ERROR);
    assert (std::fabs(V->pt().y() - p[1]) < TINY_ERROR);
    assert (std::fabs(V->pt().z() - p[2]) < TINY_ERROR);
  }

  assert (M->facemap().size() == 0);
  for (unsigned int i=0; i<face_N; ++i) {
    bmsh3d_face* F = M->_new_face();

    int num_pt_per_face;
    ret = std::fscanf(fp, "%d", &num_pt_per_face); if (ret!=1) return false;
    for (int j=0; j<num_pt_per_face; ++j) {
      int ind;
      ret = std::fscanf(fp, " %d", &ind); if (ret!=1) return false;

      bmsh3d_vertex* V = M->vertexmap(ind);
      F->_ifs_add_vertex(V);
      V->set_meshed(true);
    }
    ret = std::fscanf(fp, "\n"); if (ret!=0) return false;

    M->_add_face(F);
  }
  std::fclose(fp);
  std::cerr << "  " << face_N << " faces loaded.\n";
  return true;
}

void setup_IFS_M_label_Fs_vids(bmsh3d_mesh* M, const int label,
                               std::vector<bmsh3d_vertex*>& vertices,
                               std::vector<bmsh3d_face*>& faces)
{
  int vid_counter = 0;

  // Go through all vertices and set vid to -1.
  auto vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); ++vit) {
    bmsh3d_vertex* V = (*vit).second;
    V->set_vid(-1);
  }

  // Go through all labelled faces and set vid for each incident vertex.
  auto fit = M->facemap().begin();
  for (; fit != M->facemap().end(); ++fit) {
    bmsh3d_face* F = (*fit).second;
    if (! F->is_visited(label))
      continue;

    faces.push_back(F);

    std::vector<bmsh3d_vertex*> fv;
    F->get_ordered_Vs (fv);
    for (auto V : fv) {
      if (V->vid() >= 0)
        continue; // skip V that already in the IFS set.
      V->set_vid(vid_counter++);
      vertices.push_back(V);
    }
  }
}

// Save the labelled mesh faces into file.
bool bmsh3d_save_label_faces_ply2(bmsh3d_mesh* M, const int label, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen(file, "w")) == nullptr) {
#ifdef DEBUG
     std::cerr << "  can't open PLY2 file " << file << " to write.\n";
#endif // DEBUG
    return false;
  }

  // Go through all labelled faces & set vid for each incident vertex.
  std::vector<bmsh3d_vertex*> vertices;
  std::vector<bmsh3d_face*> faces;
  setup_IFS_M_label_Fs_vids (M, label, vertices, faces);

  std::cerr << "  saving " << file << " :\n\t"
           << vertices.size() << " points, " << faces.size() << " faces ...\n";

  std::fprintf(fp, "%lu\n", (long unsigned)vertices.size());
  std::fprintf(fp, "%lu\n", (long unsigned)faces.size());

  for (unsigned int i=0; i<vertices.size(); ++i) {
    bmsh3d_vertex* V = vertices[i];
    assert (V->vid() == (int)i);

    std::fprintf(fp, "%.16f ", V->pt().x());
    std::fprintf(fp, "%.16f ", V->pt().y());
    std::fprintf(fp, "%.16f\n",V->pt().z());
  }

  for (auto F : faces) {
    assert (F->is_visited(label));

    std::vector<bmsh3d_vertex*> vertices;
    F->get_ordered_Vs (vertices);

    std::fprintf(fp, "%lu", (long unsigned)vertices.size());
    for (auto V : vertices) {
      std::fprintf(fp, " %d", V->vid());
    }
    std::fprintf(fp, "\n");
  }
  std::fclose(fp);
  std::cerr << "  done.\n";

  return true;
}

// ====================== Hugues Hoppe's .M Mesh File ==========================

bool bmsh3d_load_m(bmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;
  if (!(fp = std::fopen(file, "r"))) {
#ifdef DEBUG
    std::cerr << "  can't open input .M file " << file << '\n';
#endif // DEBUG
    return false;
  }

  // Read in mesh vertices.
  do {
    int id;
    char type[128];
    double x, y, z, r, g, b;
    int tri[3];
    int ret = std::fscanf(fp, "%s", type);
    if (ret == EOF)
      break;
    if (ret != 1) return false;

    if (std::strcmp(type, "Vertex")==0) {
      ret = std::fscanf(fp, "%d %lf %lf %lf {rgb=(%lf %lf %lf)}\n",
                       &id, &x, &y, &z, &r, &g, &b);
      if (ret!=7) return false;

      bmsh3d_vertex* V = M->_new_vertex();
      V->set_id (id);
      V->get_pt().set (x, y, z);
      M->_add_vertex(V);

      if (id > M->vertex_id_counter())
        M->set_vertex_id_counter(id+1);
    }
    else if (std::strcmp(type, "Face")==0) {
      ret = std::fscanf(fp, "%d %d %d %d\n", &id, &tri[0], &tri[1], &tri[2]); if (ret!=4) return false;
      bmsh3d_face* F = M->_new_face();
      for (int i : tri) {
        bmsh3d_vertex* V = M->vertexmap(i);
        F->_ifs_add_vertex(V);
        V->set_meshed(true);
      }
      M->_add_face(F);
    }
  }
  while (true); // jumping out of this loop with "break" when at EOF

  std::fclose(fp);
  std::cerr << "  done.\n";

  return true;
}

bool bmsh3d_save_m(bmsh3d_mesh* M, const char* file)
{
  std::FILE* fp;
  if (!(fp = std::fopen(file, "w"))) {
#ifdef DEBUG
    std::cerr << "  can't open output .M file " << file << '\n';
#endif // DEBUG
    return false;
  }
  std::cerr << "  saving " << file << " :\n\t" << M->vertexmap().size()
           << " points, " << M->facemap().size() << " faces ...\n";

  std::fprintf(fp, "%lu\n", (long unsigned)M->vertexmap().size());
  std::fprintf(fp, "%lu\n", (long unsigned)M->facemap().size());

  auto it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); ++it) {
    bmsh3d_vertex* v = (*it).second;

    std::fprintf(fp, "%.16f ", v->pt().x());
    std::fprintf(fp, "%.16f ", v->pt().y());
    std::fprintf(fp, "%.16f\n",v->pt().z());
  }

  auto fit = M->facemap().begin();
  for (; fit != M->facemap().end(); ++fit) {
    bmsh3d_face* F = (*fit).second;

    std::fprintf(fp, "%lu", (long unsigned)F->vertices().size());
    for (unsigned j=0; j<F->vertices().size(); ++j) {
      bmsh3d_vertex* V = F->vertices(j);
      std::fprintf(fp, " %d", V->id());
    }
    std::fprintf(fp, "\n");
  }
  std::fclose(fp);
  std::cerr << "  done.\n";

  return true;
}

//: For the input "abc/def/prefix-ghi-jkl-mno.txt"
//  Return the file suffix ".txt"
std::string get_suffix(const std::string& filename)
{
  int startf=filename.find_last_of('.');
  int endf=filename.find_last_of('\0');
  if (startf == endf)
    return "\0";
  return filename.substr (startf, endf);
}

//################### LIST FILE ###############################

bool bmsh3d_read_list_file(const char* file,
                           std::vector<std::string>& data_files,
                           std::vector<std::string>& align_files)
{
  std::string filename(file);
  if (get_suffix(filename) == "")
    filename += ".txt";
  std::cerr << "bmsh3d_read_list_file(): " << filename.c_str() << ".\n";

  std::ifstream  in;
  std::string    linestr;
  in.open (filename.c_str());
  if (!in) {
    std::cerr << "Can't open listfile " << filename.c_str() << '\n';
    return false;
  }

  // Read in each line of dataset in the list file.
  while (in) {
    linestr.clear();
    std::getline(in, linestr);

    if (linestr.length() == 0 || std::strncmp(linestr.c_str(), "#", 1) == 0)
      continue; // skip empty line and comments.

    char f[256] = "", af[256] = "";
    std::sscanf(linestr.c_str(), "%s %s", f, af);

    data_files.emplace_back(f);
    align_files.emplace_back(af);
  }

  in.close();
  std::cerr << '\t' << data_files.size() << " files (" << align_files.size()
           << " alignment files) read from " << filename.c_str() << ".\n";

  return true;
}

bool bmsh3d_save_list_file(const std::string& list_file,
                           const std::vector<std::string>& data_files,
                           const std::vector<std::string>& align_files)
{
  std::cerr << "  Saving list file " << list_file.c_str() << ".\n";
  std::FILE* fp;
  assert (data_files.size() == align_files.size());
  if ((fp = std::fopen(list_file.c_str(), "w")) == nullptr) {
    std::cerr << "Can't open output txt file " << list_file << '\n';
    return false;
  }
  for (unsigned int i=0; i<data_files.size(); ++i) {
    std::fprintf(fp, "%s %s\n", data_files[i].c_str(), align_files[i].c_str());
  }
  std::fclose(fp);
  return true;
}

bool bmsh3d_save_list_view_run_file(const std::string& list_view_run,
                                    const std::string& list_file)
{
  std::cerr << "  Saving list view run file " << list_file.c_str() << ".\n";
  std::FILE* fp;
  if ((fp = std::fopen(list_view_run.c_str(), "w")) == nullptr) {
    std::cerr << "Can't open output txt file " << list_file << '\n';
    return false;
  }
  std::fprintf(fp, "dbmsh3dappw -list %s\n", list_file.c_str());
  std::fclose(fp);
  return true;
}

//===================xml==============
//
bool bmsh3d_save_xml(bmsh3d_mesh* mesh, const char* file)
{
  FILE* fp;
  if ((fp = std::fopen(file, "w")) == nullptr) {
    std::cerr << "Can't open xml file " << file << " to write.\n";
    return false;
  }
  std::cerr << "Saving " << file << " :\n\t" << mesh->vertexmap().size()
           << " points, " << mesh->facemap().size() << " faces ...\n";

  std::fprintf(fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>");
  std::fprintf(fp, "\n<CityModel xmlns=\"http://www.citygml.org/citygml/1/0/0\" xmlns:gml=\"http://www.opengis.net/gml\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.citygml.org/citygml/1/0/0 http://www.citygml.org/citygml/1/0/0/CityGML.xsd\">");

  std::fprintf(fp, "\n<gml:description>CityModel description</gml:description><gml:name>CityModel name</gml:name>");
  std::fprintf(fp, "\n<cityObjectMember>");

  //  <Building gml:id="GEB_TH_IFC_Building_Variant_GEB_0">");
  std::fprintf(fp, "\n<Building>");
  std::fprintf(fp, "\n<gml:description>Building Description</gml:description>");
  std::fprintf(fp, "<gml:name>Building Name</gml:name>");
  std::fprintf(fp, "<consistsOfBuildingPart>");
  //       <BuildingPart gml:id="GEB_TH_IFC_Building_Variant_GEB_TEIL_1">
  std::fprintf(fp, "<BuildingPart>");
  std::fprintf(fp, "<gml:description>BuildingPart Description</gml:description>");
  std::fprintf(fp, "<gml:name>BuildingPart Name</gml:name>");

  auto fit = mesh->facemap().begin();
  for (; fit != mesh->facemap().end(); ++fit)
  {
    bmsh3d_face* face = (*fit).second;
    std::fprintf(fp, "\n<boundedBy>");
    std::fprintf(fp, "<WallSurface>");
    std::fprintf(fp, "<lod4MultiSurface><gml:MultiSurface>\n");
    std::fprintf(fp, "<gml:surfaceMember>");
    std::fprintf(fp, "<TexturedSurface orientation=\"+\">");
    std::fprintf(fp, "<gml:baseSurface>");
    std::fprintf(fp, "<gml:Polygon>");
    std::fprintf(fp, "<gml:exterior>");
    std::fprintf(fp, "<gml:LinearRing>\n");

    for (unsigned j=0; j<face->vertices().size(); ++j)
    {
      bmsh3d_vertex* v = face->vertices(j);
      std::fprintf(fp, "\n<gml:pos srsDimension=\"3\">");
      std::fprintf(fp, "%.16f ", v->pt().x());
      std::fprintf(fp, "%.16f ", v->pt().y());
      std::fprintf(fp, "%.16f",  v->pt().z());
      std::fprintf(fp, "</gml:pos>\n");
    }
    // Now print the first vertex again to close the polygon
    bmsh3d_vertex* v = face->vertices(0);
    std::fprintf(fp, "\n<gml:pos srsDimension=\"3\">");
    std::fprintf(fp, "%.16f ", v->pt().x());
    std::fprintf(fp, "%.16f ", v->pt().y());
    std::fprintf(fp, "%.16f",  v->pt().z());
    std::fprintf(fp, "</gml:pos>\n");
    std::fprintf(fp, "</gml:LinearRing>");
    std::fprintf(fp, "</gml:exterior>");
    std::fprintf(fp, "</gml:Polygon>");
    std::fprintf(fp, "</gml:baseSurface>");
    std::fprintf(fp, "</TexturedSurface>\n");
    std::fprintf(fp, "</gml:surfaceMember>");
    std::fprintf(fp, "</gml:MultiSurface></lod4MultiSurface>");
    std::fprintf(fp, "</WallSurface>");
    std::fprintf(fp, "</boundedBy>\n");
  }
  std::fprintf(fp, "</BuildingPart>");
  std::fprintf(fp, "</consistsOfBuildingPart>");
  std::fprintf(fp, "</Building>");
  std::fprintf(fp, "</cityObjectMember>");
  std::fprintf(fp, "</CityModel>");

  std::fclose(fp);
  std::cerr << "\tdone.\n";

  return true;
}
