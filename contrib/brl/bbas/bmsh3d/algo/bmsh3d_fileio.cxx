//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/bmsh3d_fileio.cxx 
//:
// \file
// \brief 3D mesh file I/O
//
//
// \author
//  MingChing Chang  Feb 10, 2005
//
// \verbatim
//  Modifications
//    6/13/2005   Nhon Trinh     implemented bmsh3d_load_ply(...)
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vul/vul_file.h>
#include <vul/vul_string.h>
#include <vgl/vgl_point_3d.h>

#include <vnl/vnl_numeric_traits.h>

#include "bmsh3d_fileio.h"

//#include <rply/rply.h>

// #################################################################
//    POINT CLOUD FILE I/O
// #################################################################

// ============================== XYZ ==============================

bool bmsh3d_load_xyz (bmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .XYZ file %s\n", file);
    return false; 
  }
  assert (pointset->vertexmap().size() == 0);

  //Read in (x, y, z) points.
  int ret;
  do {
    double x, y, z;
    ret = vcl_fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    if (ret != EOF) {
      bmsh3d_vertex* V = pointset->_new_vertex ();
      V->get_pt().set (x, y, z);
      pointset->_add_vertex (V);
    }
  }
  while (ret != EOF);

  vul_printf (vcl_cerr, "  %d points loaded from %s.\n", 
               pointset->vertexmap().size(), file);

  fclose (fp);
  return true;
}

bool bmsh3d_save_xyz (bmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open output .XYZ file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : %d points ...\n", 
               file, pointset->vertexmap().size());
  
  vcl_map<int, bmsh3d_vertex*>::iterator it = pointset->vertexmap().begin();
  for (; it != pointset->vertexmap().end(); it++) {
    bmsh3d_vertex* v = (*it).second;
    vcl_fprintf (fp, "%.16f %.16f %.16f\n", v->pt().x(), v->pt().y(), v->pt().z());
  }

  vcl_fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

bool bmsh3d_load_xyz (vcl_vector<vgl_point_3d<double> >& pts, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .XYZ file %s\n", file);
    return false; 
  }
  assert (pts.size() == 0);

  //Read in (x, y, z) points.
  int ret;
  vgl_point_3d<double> P;  
  do {
    double x, y, z;
    ret = vcl_fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    if (ret != EOF) {
      P.set (x, y, z);
      pts.push_back (P);
    }
  }
  while (ret != EOF);

  vul_printf (vcl_cerr, "  %d points loaded from %s.\n", pts.size(), file);

  fclose (fp);
  return true;
}

bool bmsh3d_save_xyz (vcl_vector<vgl_point_3d<double> >& pts, const char* file)
{
  vcl_FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open output .XYZ file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : %d points ...\n", file, pts.size());
  
  vcl_vector<vgl_point_3d<double> >::iterator it = pts.begin();
  for (; it != pts.end(); it++) {
    vgl_point_3d<double> P = (*it);
    vcl_fprintf (fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

bool bmsh3d_load_xyz (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .XYZ file %s\n", file);
    return false; 
  }
  assert (idpts.size() == 0);

  //Read in (x, y, z) points.
  int ret;
  int id = 0;
  vgl_point_3d<double> P;  
  do {
    double x, y, z;
    ret = vcl_fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    if (ret != EOF) {
      P.set (x, y, z);
      idpts.push_back (vcl_pair<int, vgl_point_3d<double> > (id, P));
      id++;
    }
  }
  while (ret != EOF);

  vul_printf (vcl_cerr, "  %d points loaded from %s.\n", idpts.size(), file);

  fclose (fp);
  return true;
}
bool bmsh3d_save_xyz (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file)
{
  vcl_FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open output .XYZ file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : %d points ...\n", file, idpts.size());
  
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > >::iterator it = idpts.begin();
  for (; it != idpts.end(); it++) {
    vgl_point_3d<double> P = (*it).second;
    vcl_fprintf (fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

//: Oriented points: point + normal.
bool bmsh3d_load_xyzn1 (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .XYZN1 file %s\n", file);
    return false; 
  }
  assert (ori_pts.size() == 0);

  //Read in # points
  int num_pts;
  vcl_fscanf (fp, "%d\n", &num_pts);

  //Read in (x, y, z) points.
  int ret;
  vgl_point_3d<double> P;  
  vgl_vector_3d<double> N;  
  do {
    double x, y, z, nx, ny, nz;
    //w is the local feature size, not used.
    ret = vcl_fscanf (fp, "%lf %lf %lf %lf %lf %lf\n", &x, &y, &z, &nx, &ny, &nz);
    if (ret != EOF) {
      P.set (x, y, z);
      N.set (nx, ny, nz);
      ori_pts.push_back (vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> >(P, N));
    }
  }
  while (ret != EOF);

  assert (ori_pts.size() == num_pts);

  vul_printf (vcl_cerr, "  %d oriented points loaded from %s.\n", 
              ori_pts.size(), file);

  fclose (fp);
  return true;
}

bool bmsh3d_save_xyzn1 (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file)
{
  vcl_FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open output .XYZNW file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : %d oriented points ...\n", 
              file, ori_pts.size());
  
  //# of points.
  vcl_fprintf (fp, "%d\n", ori_pts.size());

  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >::iterator it = ori_pts.begin();
  for (; it != ori_pts.end(); it++) {
    vgl_point_3d<double> P = (*it).first;
    vgl_vector_3d<double> N = (*it).second;
    vcl_fprintf (fp, "%.16f %.16f %.16f %.16f %.16f %.16f\n", 
                 P.x(), P.y(), P.z(), N.x(), N.y(), N.z());
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

//: Oriented points: point + normal + local feature size.
bool bmsh3d_load_xyznw (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .XYZNW file %s\n", file);
    return false; 
  }
  assert (ori_pts.size() == 0);

  //Read in (x, y, z) points.
  int ret;
  vgl_point_3d<double> P;  
  vgl_vector_3d<double> N;  
  do {
    double x, y, z, nx, ny, nz, w;
    //w is the local feature size, not used.
    ret = vcl_fscanf (fp, "%lf %lf %lf %lf %lf %lf %lf\n", &x, &y, &z, &nx, &ny, &nz, &w);
    if (ret != EOF) {
      P.set (x, y, z);
      N.set (nx, ny, nz);
      ori_pts.push_back (vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> >(P, N));
    }
  }
  while (ret != EOF);

  vul_printf (vcl_cerr, "  %d oriented points loaded from %s.\n", 
              ori_pts.size(), file);

  fclose (fp);
  return true;
}

bool bmsh3d_save_xyznw_vector (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& oriented_points,
                                const char* file)
{
  vcl_FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open output .XYZNW file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : %d oriented points ...\n", 
               file, oriented_points.size());
  
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >::iterator it = oriented_points.begin();
  for (; it != oriented_points.end(); it++) {
    vgl_point_3d<double> P = (*it).first;
    vgl_vector_3d<double> N = (*it).second;
    double w = 1.0;
    vcl_fprintf (fp, "%.16f %.16f %.16f %.16f %.16f %.16f %lf\n", 
                 P.x(), P.y(), P.z(), N.x(), N.y(), N.z(), w);
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

// ============================== P3D ==============================

bool bmsh3d_load_p3d (bmsh3d_pt_set* pointset, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .P3D file %s\n", file);
    return false; 
  }

  assert (pointset->vertexmap().size() == 0); ///clear();

  unsigned int dim;
  vcl_fscanf (fp, "%u\n", &dim);
  assert (dim==3);
  unsigned int numGenes;
  vcl_fscanf (fp, "%u\n", &numGenes);

  vul_printf (vcl_cerr, "  loading %s : %d points ...\n", 
               file, numGenes);

  for (unsigned int i=0; i<numGenes; i++) {
    bmsh3d_vertex* v = pointset->_new_vertex ();
    double x, y, z;
    vcl_fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    v->get_pt().set (x, y, z);

    pointset->_add_vertex (v);
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

bool bmsh3d_save_p3d (bmsh3d_pt_set* pointset, const char* file)
{
  vcl_FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open output .P3D file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : %d points ...\n", 
               file, pointset->vertexmap().size());

  vcl_fprintf (fp, "%d\n", 3);
  vcl_fprintf (fp, "%u\n", pointset->vertexmap().size());
  
  vcl_map<int, bmsh3d_vertex*>::iterator it = pointset->vertexmap().begin();
  for (; it != pointset->vertexmap().end(); it++) {
    bmsh3d_vertex* v = (*it).second;
    vcl_fprintf (fp, "%.16f %.16f %.16f\n", v->pt().x(), v->pt().y(), v->pt().z());
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

bool bmsh3d_load_p3d (vcl_vector<vgl_point_3d<double> >& pts, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .P3D file %s\n", file);
    return false; 
  }

  assert (pts.size() == 0);

  unsigned int dim;
  vcl_fscanf (fp, "%u\n", &dim);
  assert (dim==3);
  unsigned int numGenes;
  vcl_fscanf (fp, "%u\n", &numGenes);
  pts.resize (numGenes);
  vul_printf (vcl_cerr, "  loading %s : %d points into a vector storage...\n", file, numGenes);

  for (unsigned int i=0; i<numGenes; i++) {
    vgl_point_3d<double> P;
    double x, y, z;
    vcl_fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    P.set (x, y, z);
    pts[i] = P;
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

bool bmsh3d_save_p3d (vcl_vector<vgl_point_3d<double> >& pts, const char* file)
{
  vcl_FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open output .P3D file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : %d points ...\n", file, pts.size());

  vcl_fprintf (fp, "%d\n", 3);
  vcl_fprintf (fp, "%u\n", pts.size());
  
  vcl_vector<vgl_point_3d<double> >::iterator it = pts.begin();
  for (; it != pts.end(); it++) {
    vgl_point_3d<double> P = (*it);
    vcl_fprintf (fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

bool bmsh3d_load_p3d (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts,
                       const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .P3D file %s\n", file);
    return false; 
  }

  assert (idpts.size() == 0);

  unsigned int dim;
  vcl_fscanf (fp, "%u\n", &dim);
  assert (dim==3);
  unsigned int numGenes;
  vcl_fscanf (fp, "%u\n", &numGenes);

  vul_printf (vcl_cerr, "  loading %s : %d points into a vector storage...\n", file, numGenes);

  for (unsigned int i=0; i<numGenes; i++) {
    vgl_point_3d<double> P;
    double x, y, z;
    vcl_fscanf (fp, "%lf %lf %lf\n", &x, &y, &z);
    P.set (x, y, z);

    idpts.push_back (vcl_pair<int, vgl_point_3d<double> > (i, P));
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

bool bmsh3d_save_p3d (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file)
{
  vcl_FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open output .P3D file %s\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : %d points ...\n", file, idpts.size());

  vcl_fprintf (fp, "%d\n", 3);
  vcl_fprintf (fp, "%u\n", idpts.size());
  
  vcl_vector<vcl_pair<int, vgl_point_3d<double> > >::iterator it = idpts.begin();
  for (; it != idpts.end(); it++) {
    vgl_point_3d<double> P = (*it).second;
    vcl_fprintf (fp, "%.16f %.16f %.16f\n", P.x(), P.y(), P.z());
  }

  fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

// #################################################################################

//: Open the file and read in # of dbsk3d_bnd_pts
int read_num_genes_sphere_from_file (const char* pcFile_P3D)
{
  FILE* fp;
  if ((fp = fopen (pcFile_P3D, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .P3D file %s\n", pcFile_P3D);
    return false; 
  }

  int dim;
  vcl_fscanf (fp, "%d\n", &dim);
  assert (dim==3);
  int numGenes;
  vcl_fscanf (fp, "%d\n", &numGenes);
  fclose (fp);

  vul_printf (vcl_cerr, "  reading data from %s: %d original genes.\n", pcFile_P3D, numGenes);
  return numGenes;
}

bool dbsk3d_load_con (bmsh3d_pt_set* geneset, const char* pcFile_CON, double z)
{

#define MAX_LEN 1024

  double x, y;
  char buffer[MAX_LEN];
  int nPoints;

  //1)If file open fails, return.
  vcl_ifstream fp (pcFile_CON, vcl_ios::in);
  if (!fp){
    vcl_cout<<" : Unable to Open "<<pcFile_CON<<vcl_endl;
    return false;
  }

  //: need to release the memory!!
  geneset->vertexmap().clear();

  //2)Read in file header.
  fp.getline (buffer,MAX_LEN); //CONTOUR
  fp.getline (buffer,MAX_LEN); //OPEN/CLOSE
  fp >> nPoints;
  vcl_cout << "Number of Points from Contour:" << nPoints << vcl_endl;
  
  //3)Read in 'nPoints' of lines of data.
  for (int i=0; i<nPoints; i++) {
    fp >> x >> y;
    bmsh3d_vertex *v = geneset->_new_vertex ();
    ///v->set_id (i);
    ///assert (x > -100000);
    v->get_pt().set (x, y, z);
  
    geneset->_add_vertex (v);
  }

  fp.close();
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

// #################################################################
//    MESH
// #################################################################

bool save_unmeshed_p3d (bmsh3d_mesh* M, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen (file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open output .P3D file %s\n", file);
    return false; 
  }
  unsigned int num = M->count_unmeshed_pts();
  if (num == 0)
    return false;

  vul_printf (vcl_cerr, "  saving unmeshed %u points to %s...\n", num, file);
  vcl_fprintf (fp, "%d\n", 3);
  vcl_fprintf (fp, "%u\n", num);
  
  vcl_map<int, bmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  unsigned int count=0;
  for (; it != M->vertexmap().end(); it++) {
    bmsh3d_vertex* v = (*it).second;
    if (v->b_meshed() == false) {
      vcl_fprintf (fp, "%f %f %f\n", v->pt().x(), v->pt().y(), v->pt().z());
      count++;
    }
  }

  assert (count == num);
  fclose (fp);
  return true;
}

bool bmsh3d_load_ply2 (vcl_vector<vgl_point_3d<double> >& pts, 
                        vcl_vector<vcl_vector<int> >* faces, 
                        const char* file)
{
  assert (0);
  return true;
}

bool bmsh3d_save_ply2 (const vcl_vector<vgl_point_3d<double> >& pts, 
                        const vcl_vector<vcl_vector<int> >& faces, 
                        const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen(file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open PLY2 file %s to write.\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : \n\t%d points, %d faces ...\n", 
              file, pts.size(), faces.size());

  vcl_fprintf (fp, "%d\n", pts.size());
  vcl_fprintf (fp, "%d\n", faces.size());

  for (unsigned int i=0; i<pts.size(); i++) {
    vcl_fprintf (fp, "%.16f ", pts[i].x());
    vcl_fprintf (fp, "%.16f ", pts[i].y());
    vcl_fprintf (fp, "%.16f ", pts[i].z());
    vcl_fprintf (fp, "\n");
  }

  for (unsigned int i=0; i<faces.size(); i++) {
    vcl_fprintf (fp, "%d ", faces[i].size());
    assert (faces[i].size() != 0);
    for (unsigned int j=0; j<faces[i].size(); j++)
      vcl_fprintf (fp, "%d ", faces[i][j]);
    vcl_fprintf (fp, "\n");
  }

  vcl_fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}


// ===================== GENERIC ==================================
//: Load a mesh files (only PLY and PLY2 are currently supported)
bool bmsh3d_load (bmsh3d_mesh* M, const char* file, const char* format)
{
  vcl_string file_format(format);
  file_format = "." + file_format;
  
  // if no type is given, determine it using file extension
  if (file_format == ".DEFAULT")
  {
    file_format = vul_file::extension(file);
    vul_string_upcase(file_format);
    ///file_format = vul_file::extension(file);
  }

  vcl_cout << "format = " << file_format << "\n";
  // call appropriate load functions to load the M
  if (file_format == ".PLY")
  {
    return bmsh3d_load_ply(M, file);
  }

  if (file_format == ".PLY2")
  {
    return bmsh3d_load_ply2(M, file);
  }

  vcl_cout << "  loading failed. Unknown file format.\n";
  return false;
}

// ============================= PLY2 ==============================

bool bmsh3d_save_ply2 (bmsh3d_mesh* M, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen(file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open PLY2 file %s to write.\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : \n\t%d points, %d faces ...\n", 
               file, M->vertexmap().size(), M->facemap().size());

  vcl_fprintf (fp, "%d\n", M->vertexmap().size());
  vcl_fprintf (fp, "%d\n", M->facemap().size());

  //Use v->vid() to re-index vertices, starting with id 0.
  int vidcounter = 0;
  vcl_map<int, bmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); it++) {
    bmsh3d_vertex* V = (*it).second;
    V->set_vid (vidcounter++);

    vcl_fprintf (fp, "%.16f ", V->pt().x());
    vcl_fprintf (fp, "%.16f ", V->pt().y());
    vcl_fprintf (fp, "%.16f ", V->pt().z());
    vcl_fprintf (fp, "\n");
  }

  vcl_map<int, bmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    bmsh3d_face* F = (*fit).second;
    F->_ifs_track_ordered_vertices ();

    vcl_fprintf (fp, "%d ", F->vertices().size());
    for (unsigned j=0; j<F->vertices().size(); j++) {
      bmsh3d_vertex* V = F->vertices(j);
      vcl_fprintf (fp, "%d ", V->vid());
    }
    vcl_fprintf (fp, "\n");

  }
  vcl_fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");

  return true;
}

bool bmsh3d_load_ply2 (bmsh3d_mesh* M, const char* file)
{
  vcl_FILE* fp;
  assert (M->vertexmap().size() == 0);
  assert (M->facemap().size() == 0);

  if ((fp = vcl_fopen(file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input PLY2 file %s.\n", file);
    return false; 
  }

  int vertex_N, face_N;
  vcl_fscanf (fp, "%d\n", &vertex_N);
  vcl_fscanf (fp, "%d\n", &face_N);

  vul_printf (vcl_cerr, "  loading %s : \n\t%d points, %d faces ...\n", 
               file, vertex_N, face_N);

  double p[3];
  for (int i=0; i<vertex_N; i++) {
    bmsh3d_vertex* point = M->_new_vertex ();

    vcl_fscanf (fp, "%lf ", &p[0]);
    vcl_fscanf (fp, "%lf ", &p[1]);
    vcl_fscanf (fp, "%lf ", &p[2]);
    vcl_fscanf (fp, "\n");
    point->get_pt().set (p);

    M->_add_vertex (point);
  }

  for (int i=0; i<face_N; i++){
    bmsh3d_face* F = M->_new_face ();

    int num_pt_per_face;
    vcl_fscanf (fp, "%d ", &num_pt_per_face);
    for (int j=0; j<num_pt_per_face; j++) {
      int ind;
      vcl_fscanf (fp, "%d ", &ind);

      bmsh3d_vertex* V = M->vertexmap (ind);
      F->_ifs_add_vertex (V);
      V->set_meshed (true);
    }
    vcl_fscanf (fp, "\n");

    M->_add_face (F);
  }
  vcl_fclose (fp);
  vul_printf (vcl_cerr, "\tdone.\n");

  return true;
}

bool bmsh3d_load_ply2_v (bmsh3d_mesh* M, const char* file)
{
  vcl_FILE* fp;
  assert (M->vertexmap().size() == 0);
  assert (M->facemap().size() == 0);

  if ((fp = vcl_fopen(file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input PLY2 file %s.\n", file);
    return false; 
  }
  int vertex_N, face_N;
  vcl_fscanf (fp, "%d\n", &vertex_N);
  vcl_fscanf (fp, "%d\n", &face_N);
  vul_printf (vcl_cerr, "  bmsh3d_load_ply2_v() %s: \n\t%d points, %d faces ...\n", 
              file, vertex_N, face_N);

  double p[3];
  for (int i=0; i<vertex_N; i++) {
    bmsh3d_vertex* V = M->_new_vertex ();
    vcl_fscanf (fp, "%lf ", &p[0]);
    vcl_fscanf (fp, "%lf ", &p[1]);
    vcl_fscanf (fp, "%lf ", &p[2]);
    vcl_fscanf (fp, "\n");
    V->get_pt().set (p);
    M->_add_vertex (V);
  }

  vcl_fclose (fp);  
  vul_printf (vcl_cerr, "  %d vertices loaded.\n", vertex_N);
  return true;
}

#define TINY_ERROR  1E-3

bool bmsh3d_load_ply2_f (bmsh3d_mesh* M, const char* file)
{
  vcl_FILE* fp;

  if ((fp = vcl_fopen(file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input PLY2 file %s.\n", file);
    return false; 
  }

  int vertex_N, face_N;
  vcl_fscanf (fp, "%d\n", &vertex_N);
  vcl_fscanf (fp, "%d\n", &face_N);

  vul_printf (vcl_cerr, "  bmsh3d_load_ply2_f (%s): \n", file);
  vul_printf (vcl_cerr, "\tSkip %d points and load %d faces ...\n", vertex_N, face_N);

  assert (M->vertexmap().size() == vertex_N);
  double p[3];
  for (int i=0; i<vertex_N; i++) {
    bmsh3d_vertex* V = M->vertexmap(i);
    vcl_fscanf (fp, "%lf ", &p[0]);
    vcl_fscanf (fp, "%lf ", &p[1]);
    vcl_fscanf (fp, "%lf ", &p[2]);
    vcl_fscanf (fp, "\n");
    //assert that M has exactly these vertices.
    assert (vcl_fabs (V->pt().x() - p[0]) < TINY_ERROR);
    assert (vcl_fabs (V->pt().y() - p[1]) < TINY_ERROR);
    assert (vcl_fabs (V->pt().z() - p[2]) < TINY_ERROR);
  }

  assert (M->facemap().size() == 0);
  for (int i=0; i<face_N; i++){
    bmsh3d_face* F = M->_new_face ();

    int num_pt_per_face;
    vcl_fscanf (fp, "%d ", &num_pt_per_face);
    for (int j=0; j<num_pt_per_face; j++) {
      int ind;
      vcl_fscanf (fp, "%d ", &ind);

      bmsh3d_vertex* V = M->vertexmap (ind);
      F->_ifs_add_vertex (V);
      V->set_meshed (true);
    }
    vcl_fscanf (fp, "\n");

    M->_add_face (F);
  }
  vcl_fclose (fp);
  vul_printf (vcl_cerr, "  %d faces loaded.\n", face_N);
  return true;
}

void setup_IFS_M_label_Fs_vids (bmsh3d_mesh* M, const int label, 
                                vcl_vector<bmsh3d_vertex*>& vertices,
                                vcl_vector<bmsh3d_face*>& faces)
{
  int vid_counter = 0;

  //Go through all vertices and set vid to -1.
  vcl_map<int, bmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    bmsh3d_vertex* V = (*vit).second;
    V->set_vid (-1);
  }

  //Go through all labelled faces and set vid for each incident vertex.
  vcl_map<int, bmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    bmsh3d_face* F = (*fit).second;
    if (F->is_visited (label) == false)
      continue;

    faces.push_back (F);

    vcl_vector<bmsh3d_vertex*> fv;
    F->get_ordered_Vs (fv);
    for (unsigned int i=0; i<fv.size(); i++) {
      bmsh3d_vertex* V = fv[i];
      if (V->vid() >= 0)
        continue; //skip V that already in the IFS set.
      V->set_vid (vid_counter++);
      vertices.push_back (V);
    }
  }
}

//Save the labelled mesh faces into file.
bool bmsh3d_save_label_faces_ply2 (bmsh3d_mesh* M, const int label, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen(file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open PLY2 file %s to write.\n", file);
    return false;  
  }

  //Go through all labelled faces & set vid for each incident vertex.
  vcl_vector<bmsh3d_vertex*> vertices;
  vcl_vector<bmsh3d_face*> faces;
  setup_IFS_M_label_Fs_vids (M, label, vertices, faces);

  vul_printf (vcl_cerr, "  saving %s : \n\t%d points, %d faces ...\n", 
              file, vertices.size(), faces.size());

  vcl_fprintf (fp, "%d\n", vertices.size());
  vcl_fprintf (fp, "%d\n", faces.size());

  for (unsigned int i=0; i<vertices.size(); i++) {
    bmsh3d_vertex* V = vertices[i];
    assert (V->vid() == i);

    vcl_fprintf (fp, "%.16f ", V->pt().x());
    vcl_fprintf (fp, "%.16f ", V->pt().y());
    vcl_fprintf (fp, "%.16f ", V->pt().z());
    vcl_fprintf (fp, "\n");
  }

  for (unsigned int i=0; i<faces.size(); i++) {
    bmsh3d_face* F = faces[i];
    assert (F->is_visited (label));

    vcl_vector<bmsh3d_vertex*> vertices;
    F->get_ordered_Vs (vertices);

    vcl_fprintf (fp, "%d ", vertices.size());
    for (unsigned j=0; j<vertices.size(); j++) {
      bmsh3d_vertex* V = vertices[j];
      vcl_fprintf (fp, "%d ", V->vid());
    }
    vcl_fprintf (fp, "\n");

  }
  vcl_fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");

  return true;
}

// ====================== Hugues Hoppe's .M Mesh File ==========================

bool bmsh3d_load_m (bmsh3d_mesh* M, const char* file)
{
  vcl_FILE* fp;
  if ((fp = vcl_fopen(file, "r")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open input .M file %s.\n", file);
    return false; 
  }

  //Read in mesh vertices.
  int ret;
  do {
    int id;
    char type[128];
    double x, y, z, r, g, b;
    int tri[3];
    ret = vcl_fscanf (fp, "%s", type);
    if (ret == EOF)
      break;
    
    if (vcl_strcmp (type, "Vertex")==0) {
      vcl_fscanf (fp, "%d %lf %lf %lf {rgb=(%lf %lf %lf)}\n", 
                  &id, &x, &y, &z, &r, &g, &b);

      bmsh3d_vertex* V = M->_new_vertex ();
      V->set_id (id);
      V->get_pt().set (x, y, z);
      M->_add_vertex (V);

      if (id > M->vertex_id_counter())
        M->set_vertex_id_counter (id+1);
    }
    else if (vcl_strcmp (type, "Face")==0) {
      vcl_fscanf (fp, "%d %d %d %d\n", &id, &tri[0], &tri[1], &tri[2]);
      bmsh3d_face* F = M->_new_face ();
      for (int i=0; i<3; i++) {
        bmsh3d_vertex* V = M->vertexmap (tri[i]);
        F->_ifs_add_vertex (V);
        V->set_meshed (true);
      }
      M->_add_face (F);
    }
  }
  while (ret != EOF);

  vcl_fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");

  return true;
}

bool bmsh3d_save_m (bmsh3d_mesh* M, const char* file)
{
  vcl_FILE* fp;
  if ((fp = vcl_fopen(file, "w")) == NULL) {
    ///vul_printf (vcl_cerr, "  can't open .M file %s to write.\n", file);
    return false; 
  }
  vul_printf (vcl_cerr, "  saving %s : \n\t%d points, %d faces ...\n", 
               file, M->vertexmap().size(), M->facemap().size());

  vcl_fprintf (fp, "%d\n", M->vertexmap().size());
  vcl_fprintf (fp, "%d\n", M->facemap().size());

  vcl_map<int, bmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); it++) {
    bmsh3d_vertex* v = (*it).second;

    vcl_fprintf (fp, "%.16f ", v->pt().x());
    vcl_fprintf (fp, "%.16f ", v->pt().y());
    vcl_fprintf (fp, "%.16f ", v->pt().z());
    vcl_fprintf (fp, "\n");
  }

  vcl_map<int, bmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    bmsh3d_face* F = (*fit).second;

    vcl_fprintf (fp, "%d ", F->vertices().size());
    for (unsigned j=0; j<F->vertices().size(); j++) {
      bmsh3d_vertex* V = F->vertices(j);
      vcl_fprintf (fp, "%d ", V->id());
    }
    vcl_fprintf (fp, "\n");

  }
  vcl_fclose (fp);
  vul_printf (vcl_cerr, "  done.\n");

  return true;
}

//: For the input "abc\def\prefix-ghi-jkl-mno.txt"
//  return the file suffix ".txt"
vcl_string get_suffix (const vcl_string& filename)
{
  int start,end;
  start=filename.find_last_of ('.');
  end=filename.find_last_of ('\0');
  if(start == end)
    return "\0";
  return filename.substr (start, end);
}

//################### LIST FILE ###############################

bool bmsh3d_read_list_file (const char* file,
                             vcl_vector<vcl_string>& data_files, 
                             vcl_vector<vcl_string>& align_files)
{
  vcl_string filename (file);
  if (get_suffix (filename) == "")
    filename += ".txt";
  vul_printf (vcl_cerr, "bmsh3d_read_list_file(): %s.\n", filename.c_str());

  vcl_ifstream  in;
  vcl_string    linestr;
  in.open (filename.c_str());
  if (in == false) {
    vul_printf (vcl_cerr, "Can't open listfile %s\n", filename.c_str());
    return false; 
  }

  //Read in each line of dataset in the list file.
  int file_count = 0;
  int af_count = 0;
  while (in) {
    linestr.clear();
    vcl_getline (in, linestr);

    if (linestr.length() == 0 || vcl_strncmp (linestr.c_str(), "#", 1) == 0)
      continue; //skip empty line and comments.

    char file[256] = "", af[256] = "";
    vcl_sscanf (linestr.c_str(), "%s %s", file, af);

    data_files.push_back (vcl_string (file));
    align_files.push_back (vcl_string (af));
  }

  in.close();
  vul_printf (vcl_cerr, "\t%d files (%d alignment files) read from %s.\n", 
              data_files.size(), align_files.size(), filename.c_str());
  return true;
}    

bool bmsh3d_save_list_file (const vcl_string& list_file, 
                             const vcl_vector<vcl_string>& data_files, 
                             const vcl_vector<vcl_string>& align_files)
{
  vul_printf (vcl_cerr, "  Saving list file %s.\n", list_file.c_str());
  vcl_FILE* fp;
  assert (data_files.size() == align_files.size());
  if ((fp = vcl_fopen (list_file.c_str(), "w")) == NULL) {
    vul_printf (vcl_cerr, "Can't open output txt file %s\n", list_file);
    return false;
  }
  for (unsigned int i=0; i<data_files.size(); i++) {
    vcl_fprintf (fp, "%s %s\n", data_files[i].c_str(), align_files[i].c_str());
  }
  vcl_fclose (fp);
  return true;
}

bool bmsh3d_save_list_view_run_file (const vcl_string& list_view_run, 
                                      const vcl_string& list_file)
{
  vul_printf (vcl_cerr, "  Saving list view run file %s.\n", list_file.c_str());
  vcl_FILE* fp;
  if ((fp = vcl_fopen (list_view_run.c_str(), "w")) == NULL) {
    vul_printf (vcl_cerr, "Can't open output txt file %s\n", list_file);
    return false;
  }
  vcl_fprintf (fp, "dbmsh3dappw -list %s\n", list_file.c_str());
  vcl_fclose (fp);
  return true;
}

//===================xml==============
//
bool bmsh3d_save_xml (bmsh3d_mesh* mesh, const char* file)
{
  FILE* fp;
  if ((fp = vcl_fopen(file, "w")) == NULL) {
    vcl_fprintf (stderr, "Can't open xml file %s to write.\n", file);
    return false; 
  }
  vcl_fprintf (stderr, "Saving %s : \n\t%d points, %d faces ...\n", 
               file, mesh->vertexmap().size(), mesh->facemap().size());

      vcl_fprintf (fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>");

      vcl_fprintf (fp, "\n");
      vcl_fprintf (fp, "<CityModel xmlns=\"http://www.citygml.org/citygml/1/0/0\" xmlns:gml=\"http://www.opengis.net/gml\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.citygml.org/citygml/1/0/0 http://www.citygml.org/citygml/1/0/0/CityGML.xsd\">");

      vcl_fprintf (fp, "\n");
      vcl_fprintf (fp, "<gml:description>CityModel description</gml:description>");
      vcl_fprintf (fp, "<gml:name>CityModel name</gml:name>");

      vcl_fprintf (fp, "\n");
      vcl_fprintf (fp, "<cityObjectMember>");
  //  <Building gml:id="GEB_TH_IFC_Building_Variant_GEB_0">");

      vcl_fprintf (fp, "\n");
      vcl_fprintf (fp, "<Building>");

      vcl_fprintf (fp, "\n");
      vcl_fprintf (fp, "<gml:description>Building Description</gml:description>");
      vcl_fprintf (fp, " <gml:name>Building Name</gml:name>");
      vcl_fprintf (fp, "<consistsOfBuildingPart>");
 //       <BuildingPart gml:id="GEB_TH_IFC_Building_Variant_GEB_TEIL_1">
       vcl_fprintf (fp, "<BuildingPart>");
       vcl_fprintf (fp, " <gml:description>BuildingPart Description</gml:description>");
       vcl_fprintf (fp, "<gml:name>BuildingPart Name</gml:name>");
 
  vcl_map<int, bmsh3d_face*>::iterator fit = mesh->facemap().begin();
  for (; fit != mesh->facemap().end(); fit++) {
    bmsh3d_face* face = (*fit).second;
      vcl_fprintf (fp, "\n");

       vcl_fprintf (fp, "<boundedBy>");
       vcl_fprintf (fp, "<WallSurface>");
       vcl_fprintf (fp, "<lod4MultiSurface><gml:MultiSurface>");
      vcl_fprintf (fp, "\n");

    vcl_fprintf (fp, "<gml:surfaceMember>");
    
    vcl_fprintf (fp, "<TexturedSurface orientation=\"+\">");
                  
    vcl_fprintf (fp, "<gml:baseSurface>");
                    
    vcl_fprintf (fp, "<gml:Polygon>");
                      
    vcl_fprintf (fp, "<gml:exterior>");
                        
    vcl_fprintf (fp, "<gml:LinearRing>");
             vcl_fprintf (fp, "\n");
                   
     for (unsigned j=0; j<face->vertices().size(); j++) {

      bmsh3d_vertex* v = face->vertices(j);  
      vcl_fprintf (fp, "\n");
      vcl_fprintf (fp, "<gml:pos srsDimension=\"3\">");
      vgl_point_3d<double > pt = v->pt();
      vcl_fprintf (fp, "%.16f ", v->pt().x());
      vcl_fprintf (fp, "%.16f ", v->pt().y());
      vcl_fprintf (fp, "%.16f ", v->pt().z());
      vcl_fprintf (fp, "</gml:pos>");
      vcl_fprintf (fp, "\n");
      }
//Now print the first vertex again to close the polygon
   bmsh3d_vertex* v = face->vertices(0);  
   vcl_fprintf (fp, "\n");
   vcl_fprintf (fp, "<gml:pos srsDimension=\"3\">");
   vgl_point_3d<double > pt = v->pt();  
   vcl_fprintf (fp, "%.16f ", v->pt().x());
   vcl_fprintf (fp, "%.16f ", v->pt().y());
   vcl_fprintf (fp, "%.16f ", v->pt().z());
   vcl_fprintf (fp, "</gml:pos>");
   vcl_fprintf (fp, "\n");

   vcl_fprintf (fp, "</gml:LinearRing>");
                      
    vcl_fprintf (fp, "</gml:exterior>");
                    
    vcl_fprintf (fp, "</gml:Polygon>");
                  
    vcl_fprintf (fp, "</gml:baseSurface>");
                
    vcl_fprintf (fp, "</TexturedSurface>");
      vcl_fprintf (fp, "\n");

              
    vcl_fprintf (fp, "</gml:surfaceMember>");

   vcl_fprintf (fp, "</gml:MultiSurface></lod4MultiSurface>");
  vcl_fprintf (fp, "     </WallSurface>");
  vcl_fprintf (fp, "  </boundedBy>");         

    vcl_fprintf (fp, "\n");

  }
  vcl_fprintf (fp, "       </BuildingPart>");
  vcl_fprintf (fp, "     </consistsOfBuildingPart>");
  vcl_fprintf (fp, "   </Building>");
  vcl_fprintf (fp, "  </cityObjectMember>");       
  vcl_fprintf (fp, " </CityModel>");

  vcl_fclose (fp);
  vcl_fprintf (stderr, "\tdone.\n");

  return true;
}



