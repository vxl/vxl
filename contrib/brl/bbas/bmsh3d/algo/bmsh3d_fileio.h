#ifndef _bmsh3d_fileio_h_
#define _bmsh3d_fileio_h_
//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/algo/bmsh3d_fileio.h
//:
// \file
// \brief mesh file I/O
//
// 
// \author
//  MingChing Chang  Apr 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_string.h>

#include <bmsh3d/bmsh3d_mesh.h>
//#include <bmsh3d/bmsh3d_richmesh.h>

//: List of supported mesh file formats
typedef enum {
  BOGUS_BMSH3D_FILE  = 0,
  BMSH3D_FILE_XYZ,
  BMSH3D_FILE_XYZN1,
  BMSH3D_FILE_XYZNW,
  BMSH3D_FILE_P3D,
  BMSH3D_FILE_3PI,
  BMSH3D_FILE_PLY,
  BMSH3D_FILE_PLY2,
  BMSH3D_FILE_OFF,
  BMSH3D_FILE_OBJ,
  BMSH3D_FILE_M,
  BMSH3D_FILE_IV,
  BMSH3D_FILE_WRL,
  BMSH3D_FILE_CMS,   //coarse-scale medial scaffold.
  BMSH3D_FILE_SG,    //medial scaffold graph.
} BMSH3D_FILE_TYPE;

class SbColor;

// ply format mode
enum bmsh3d_storage_mode {
    BINARY,
    ASCII  
};

// #################################################################
//    POINT CLOUD FILE I/O
// #################################################################

bool bmsh3d_load_xyz (bmsh3d_pt_set* pointset, const char* file);
bool bmsh3d_save_xyz (bmsh3d_pt_set* pointset, const char* file);

bool bmsh3d_load_xyz (vcl_vector<vgl_point_3d<double> >& pts, const char* file);
bool bmsh3d_save_xyz (vcl_vector<vgl_point_3d<double> >& pts, const char* file);

bool bmsh3d_load_xyz (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file);
bool bmsh3d_save_xyz (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file);

bool bmsh3d_load_xyzn1 (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file);
bool bmsh3d_save_xyzn1 (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file);

bool bmsh3d_load_xyznw (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file);
bool bmsh3d_save_xyznw (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& ori_pts,
                         const char* file);

bool bmsh3d_load_p3d (bmsh3d_pt_set* pointset, const char* file);
bool bmsh3d_save_p3d (bmsh3d_pt_set* pointset, const char* file);

bool bmsh3d_load_p3d (vcl_vector<vgl_point_3d<double> >& pts, const char* file);
bool bmsh3d_save_p3d (vcl_vector<vgl_point_3d<double> >& pts, const char* file);

bool bmsh3d_load_p3d (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file);
bool bmsh3d_save_p3d (vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts, const char* file);

int read_num_genes_sphere_from_file (const char* file);

//: read the .CON file as a contour on a slice
bool bmsh3d_load_con (bmsh3d_pt_set* pointset, const char* pcFile_CON, double z);

// #################################################################
//    MESH (INDEXED FACE SET) FILE I/O
// #################################################################

bool save_unmeshed_p3d (bmsh3d_mesh* M, const char* file);

bool bmsh3d_load_ply2 (vcl_vector<vgl_point_3d<double> >* pts, 
                        vcl_vector<vcl_vector<int> >* faces, const char* file);
bool bmsh3d_save_ply2 (const vcl_vector<vgl_point_3d<double> >& pts, 
                        const vcl_vector<vcl_vector<int> >& faces, const char* file);

//: Load a mesh files (only PLY and PLY2 are currently supported)
// "format" should be upper case
bool bmsh3d_load(bmsh3d_mesh* M, const char* file, const char* format = "DEFAULT");

//: save a mesh to a .ply file 
// ascii_mode = true : save an ascii PLY file
// ascii_mode = false : save a binary PLY file
bool bmsh3d_save_ply (bmsh3d_mesh* M, const char* file, bool ascii_mode = true, vcl_string comment="");

//: load a .ply file (ascii or binary)
bool bmsh3d_load_ply (bmsh3d_mesh* M, const char* file);
bool bmsh3d_load_ply_v (bmsh3d_mesh* M, const char* file); 
bool bmsh3d_load_ply_f (bmsh3d_mesh* M, const char* file);

//: save mesh to a .ply2 file
bool bmsh3d_save_ply2 (bmsh3d_mesh* M, const char* file);
//: load a .ply2 file
bool bmsh3d_load_ply2 (bmsh3d_mesh* M, const char* file);
bool bmsh3d_load_ply2_v (bmsh3d_mesh* M, const char* file);
bool bmsh3d_load_ply2_f (bmsh3d_mesh* M, const char* file);

void setup_IFS_M_label_Fs_vids (bmsh3d_mesh* M, const int label, 
                                vcl_vector<bmsh3d_vertex*>& vertices,
                                vcl_vector<bmsh3d_face*>& faces);

//Save the labelled mesh faces into file.
bool bmsh3d_save_label_faces_ply2 (bmsh3d_mesh* M, const int label, const char* file);

//: Load/Save Geomview OFF file.
bool bmsh3d_load_off (bmsh3d_mesh* M, const char* file);
bool bmsh3d_save_off (bmsh3d_mesh* M, const char* file);

//: Load/Save Wavefront OBJ file.
bool bmsh3d_load_obj (bmsh3d_mesh* M, const char* file);
bool bmsh3d_save_obj (bmsh3d_mesh* M, const char* file);

//: Load/Save Hugues Hoppe's .m file.
bool bmsh3d_load_m (bmsh3d_mesh* M, const char* file);
bool bmsh3d_save_m (bmsh3d_mesh* M, const char* file);

bool bmsh3d_load_wrl_iv (bmsh3d_mesh* M, const char* file);
bool bmsh3d_save_wrl_iv (bmsh3d_mesh* M, const char* file);

//: Read IV as ASCII indexed-face-set.
bool bmsh3d_load_iv_ifs (bmsh3d_mesh* M, const char* file);
bool bmsh3d_load_iv_ifs_intp (const char *pcIVFile, const char *pcG3DFile, int option, 
                               float fMaxPerturb, float fIVRecursiveThreshold);
bool bmsh3d_save_iv_ifs (bmsh3d_mesh* M, const char* file);
vcl_string get_suffix (const vcl_string& filename);
bool bmsh3d_read_list_file (const char* file, 
                             vcl_vector<vcl_string>& data_files, 
                             vcl_vector<vcl_string>& align_files);

bool bmsh3d_save_list_file (const vcl_string& list_file, 
                             const vcl_vector<vcl_string>& data_files, 
                             const vcl_vector<vcl_string>& align_files);

bool bmsh3d_save_list_view_run_file (const vcl_string& list_view_run, 
                                      const vcl_string& list_file);

// ============================================================================
// Rich Mesh
// ============================================================================

//: Load a rich mesh given a list of vertex properties and face properties to read
/*bool bmsh3d_load_ply (bmsh3d_richmesh* M, 
                       const char* file, 
                       const vcl_vector<vcl_string >& vertex_property_list,
                       const vcl_vector<vcl_string >& face_property_list);*/

//: Load a rich mesh given a list of vertex properties and face properties to read
/*bool bmsh3d_save_ply (bmsh3d_richmesh* M, 
                       const char* file, 
                       const vcl_vector<vcl_string >& vertex_property_list,
                       const vcl_vector<vcl_string >& face_property_list,
                       bmsh3d_storage_mode mode = ASCII);*/



bool bmsh3d_save_xml (bmsh3d_mesh* mesh, const char* file);

// #################################################################
//    PROCESS NUAGES SLICE FILES
// #################################################################

void sli_cons_to_nuages_cnt (vcl_string sli_file, vcl_string cnt_file);


#endif

