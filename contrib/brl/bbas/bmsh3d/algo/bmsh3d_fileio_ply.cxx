//---------------------------------------------------------------------
// This is brcv/shp/bmsh3d/bmsh3d_fileio_ply.cxx 
//:
// \file
// \brief PLY mesh file I/O
//
// \author
//  MingChing Chang and Nhon Trinh 
//
// \verbatim
//  Modifications
//    Feb 10, 2005   Creation
//    6/13/2005      Nhon Trinh     implemented bmsh3d(...)
//    May 07, 2005   mcchang        move to a separate file.
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

#include <rply.h>

#include <bmsh3d/algo/bmsh3d_fileio.h>

// ============================== PLY ==============================

class bmsh3d_fileio_parsed_mesh_
{
public:
  bmsh3d_mesh* mesh;
  double p[3];
  vcl_vector<int > vertex_indices;
};

int bmsh3d_fileio_vertex_cb_(p_ply_argument argument);
int bmsh3d_fileio_face_cb_(p_ply_argument argument);


//: load a .ply file
// the current implementation uses the RPLY library in db3p.
// Known failed cases: binary PLY file using PC line ending (CRLF) in header
bool bmsh3d_load_ply (bmsh3d_mesh* M, const char* file)
{
  long nvertices, ntriangles;
  bmsh3d_fileio_parsed_mesh_ parsed_mesh;
  parsed_mesh.mesh = M;

  // OPEN file
  p_ply ply = ply_open(file, NULL, 0, NULL);
  if (!ply) 
    return false;

  // Read HEADER
  if (!ply_read_header(ply)) return false;

  // vertex
  nvertices = ply_set_read_cb(ply, "vertex", "x", 
    bmsh3d_fileio_vertex_cb_, (void*) (&parsed_mesh), 0);
  ply_set_read_cb(ply, "vertex", "y", 
    bmsh3d_fileio_vertex_cb_, (void*) (&parsed_mesh), 1);
  ply_set_read_cb(ply, "vertex", "z", 
    bmsh3d_fileio_vertex_cb_, (void*) (&parsed_mesh), 2);

  // face: some 3rd party program (3D object converter) use "vertex_index"!
  ntriangles = ply_set_read_cb(ply, "face", "vertex_indices",
    bmsh3d_fileio_face_cb_, (void*) (&parsed_mesh), 0);
  
  vul_printf (vcl_cerr, "  loading %s : \n\t%d points, %d faces ...\n", 
               file, nvertices, ntriangles);

  // Read DATA
  if (!ply_read(ply)) return false;

  // CLOSE file
  ply_close(ply);

  vul_printf (vcl_cerr, "  done.\n");
  return true;
}


bool bmsh3d_load_ply_v (bmsh3d_mesh* M, const char* file)
{
  long nvertices, ntriangles = 0;
  bmsh3d_fileio_parsed_mesh_ parsed_mesh;
  parsed_mesh.mesh = M;

  // OPEN file
  p_ply ply = ply_open(file, NULL, 0, NULL);
  if (!ply) 
    return false;

  // Read HEADER
  if (!ply_read_header(ply)) 
    return false;

  // vertex
  nvertices = ply_set_read_cb(ply, "vertex", "x", 
    bmsh3d_fileio_vertex_cb_, (void*) (&parsed_mesh), 0);
  ply_set_read_cb(ply, "vertex", "y", 
    bmsh3d_fileio_vertex_cb_, (void*) (&parsed_mesh), 1);
  ply_set_read_cb(ply, "vertex", "z", 
    bmsh3d_fileio_vertex_cb_, (void*) (&parsed_mesh), 2);

  // face: some 3rd party program (3D object converter) use "vertex_index"!
  ntriangles = ply_set_read_cb(ply, "face", "vertex_indices",
    bmsh3d_fileio_face_cb_, (void*) (&parsed_mesh), 0);
  
  vul_printf (vcl_cerr, "  loading %s : \n\t%d points, %d faces ...\n", 
               file, nvertices, ntriangles);

  // Read DATA
  if (!ply_read(ply)) 
    return false;

  // CLOSE file
  ply_close(ply);

  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

bool bmsh3d_load_ply_f (bmsh3d_mesh* M, const char* file)
{
  //Temp solution: load to a tmpM and copy faces to M.
  bmsh3d_mesh* tmpM = new bmsh3d_mesh;
  if (bmsh3d_load_ply (tmpM, file) == false)
    return false;
  
  //Copy faces from tmpM to M.
  vcl_map <int, bmsh3d_face*>::iterator it = tmpM->facemap().begin();
  for (; it != tmpM->facemap().end(); it++) {
    int id = (*it).first;
    bmsh3d_face* tmpF = (*it).second;

    bmsh3d_face* F = M->_new_face (id);
    for (unsigned int j=0; j<tmpF->vertices().size(); j++) {
      bmsh3d_vertex* tmpV = tmpF->vertices(j);
      bmsh3d_vertex* V = M->vertexmap (tmpV->id());
      assert (V);
      F->_ifs_add_vertex (V);
      V->set_meshed (true);
    }
    M->_add_face (F);
  }

  vul_printf (vcl_cerr, "  bmsh3d_load_ply_f(): %d faces added to M.\n", 
              M->facemap().size());
  delete tmpM;
  return true;
}

//: Call-back function for a "vertex" element
int bmsh3d_fileio_vertex_cb_(p_ply_argument argument) 
{
  long index;
  void* temp;
  ply_get_argument_user_data(argument, &temp, &index);

  bmsh3d_fileio_parsed_mesh_* parsed_mesh = 
    (bmsh3d_fileio_parsed_mesh_*) temp;

  switch (index)
  {
  case 0: // "x" coordinate
    parsed_mesh->p[0] = ply_get_argument_value(argument);     
    break;
  case 1: // "y" coordinate
    parsed_mesh->p[1] = ply_get_argument_value(argument);     
    break;
  case 2: // "z" coordinate
    parsed_mesh->p[2] = ply_get_argument_value(argument);     

    // INSERT VERTEX INTO THE MESH
    bmsh3d_vertex* V = 
      parsed_mesh->mesh->_new_vertex ();
    V->get_pt().set(parsed_mesh->p);
    parsed_mesh->mesh->_add_vertex (V);
    break;
  };
  return 1;
}

//: Call-back function for a "face" element
int bmsh3d_fileio_face_cb_(p_ply_argument argument) 
{
  void* temp;
  ply_get_argument_user_data(argument, &temp, NULL);
  bmsh3d_fileio_parsed_mesh_* parsed_mesh = 
    (bmsh3d_fileio_parsed_mesh_*) temp;

  long length, value_index, value;
  ply_get_argument_property(argument, NULL, &length, &value_index);
  value = (int) ply_get_argument_value(argument);     

  if (value_index == -1)
  {
    parsed_mesh->vertex_indices.clear();
    parsed_mesh->vertex_indices.reserve(length);
    return 1;
  }
  
  parsed_mesh->vertex_indices.push_back(value);

  // create a face when all the indices have been read
  if (value_index == length-1)
  {
    bmsh3d_mesh* M = parsed_mesh->mesh;
    // INSERT FACE INTO THE MESH
    bmsh3d_face* F = M->_new_face ();
    // get pointers to the vertices of the new face
    for (unsigned int k=0; k < parsed_mesh->vertex_indices.size(); ++k ) 
    {
      // get pointer to the vertex from its index
      int ind = parsed_mesh->vertex_indices[k];
      bmsh3d_vertex* V = 
        M->vertexmap(ind);
      assert(V);
      F->_ifs_add_vertex (V);
    }
    M->_add_face (F);
  }
  return 1;
}

// ----------------------------------------------------------------------------
//: save mesh to a .ply file
// This implementation uses LITTLE_ENDIAN and uses '\n' for new line
bool bmsh3d_save_ply (bmsh3d_mesh* M, const char* file, bool ascii_mode)
{
  // determine storage mode
  e_ply_storage_mode storage_mode = (ascii_mode) ? PLY_ASCII : PLY_LITTLE_ENDIAN;

  // OPEN FILE
  p_ply oply = ply_create(file, storage_mode, NULL, 0, NULL);

  vul_printf (vcl_cerr, "  saving %s : \n\t%d points, %d faces ...\n", 
               file, M->vertexmap().size(), M->facemap().size());

  // HEADER SECTION
  // vertex
  ply_add_element(oply, "vertex", M->vertexmap().size());
  ply_add_scalar_property(oply, "x", PLY_DOUBLE); //PLY_FLOAT
  ply_add_scalar_property(oply, "y", PLY_DOUBLE); //PLY_FLOAT
  ply_add_scalar_property(oply, "z", PLY_DOUBLE); //PLY_FLOAT

  // face
  ply_add_element(oply, "face", M->facemap().size());
  ply_add_list_property(oply, "vertex_indices", PLY_UCHAR, PLY_INT);

  // comment
  ply_add_comment(oply, "created by bmsh3d_save_ply");

  // object info
  ply_add_obj_info(oply, "an bmsh3d_mesh object");

  // end header
  ply_write_header(oply);

  

  // DATA SECTION

  // reset the vid's of the vertices to reset its order in the vertex list,
  // which will be saved to the PLY file
  M->assign_IFS_vertex_vid_by_vertex_order();

  // traverse thru all vertices and write to ply file
  vcl_map<int, bmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); it++) 
  {
    bmsh3d_vertex* v = (*it).second;
    vgl_point_3d<double > pt = v->pt();

    ply_write(oply, pt.x());
    ply_write(oply, pt.y());
    ply_write(oply, pt.z());
    
  }

  // faces
  vcl_map<int, bmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) 
  {
    bmsh3d_face* f = (*fit).second;

    // a. write the number of vertices of the face
    ply_write(oply,f->vertices().size());

    // b. write id's of the vertices
    for (unsigned int j=0; j< f->vertices().size(); ++j)
    {
      bmsh3d_vertex* v = f->vertices(j);
      assert(v);

      // vid's contain the order of the vertices in the vertex map
      unsigned int id = v->vid();

      ply_write(oply, id);
    }
  }

  // CLOSE PLY FILE
  ply_close(oply);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}

// ======================================================================================
//              RICH MESH

// ----------------------------------------------------------------------------
class bmsh3d_fileio_parsed_richmesh_: public bmsh3d_fileio_parsed_mesh_
{
public:
  // for use on vertices
  vcl_vector<vcl_string > vertex_property_list;
  vcl_vector<double > vertex_property_values;
  int num_vertex_values_read;


  vcl_vector<vcl_string > face_property_list;
  vcl_vector<double > face_property_values;
  int num_face_values_read;
  bool all_vertex_indices_of_face_read;
};


//: Call-back function for a "vertex" element
/*int bmsh3d_fileio_richvertex_cb_(p_ply_argument argument) 
{
  long index;
  void* temp;
  ply_get_argument_user_data(argument, &temp, &index);

  bmsh3d_fileio_parsed_richmesh_* parsed_mesh = 
    (bmsh3d_fileio_parsed_richmesh_*) temp;

  // "x" coordinate
  if (index == 0) 
  {
    parsed_mesh->p[0] = ply_get_argument_value(argument); 
    ++parsed_mesh->num_vertex_values_read;
  }
  
  // "y" coordinate
  else if (index == 1) 
  {
    parsed_mesh->p[1] = ply_get_argument_value(argument);     
    ++parsed_mesh->num_vertex_values_read;
  }

  // "z" coordinate
  else if (index == 2) 
  {
    parsed_mesh->p[2] = ply_get_argument_value(argument); 
    ++parsed_mesh->num_vertex_values_read;
  }
  
  // other properties
  else if (index < (long)(3 + parsed_mesh->vertex_property_list.size()))
  {
    parsed_mesh->vertex_property_values[index-3] = 
      ply_get_argument_value(argument);
    ++parsed_mesh->num_vertex_values_read;
  }

  // insert vertex into the mesh if we're at the end of property list
  if (parsed_mesh->num_vertex_values_read == 
    (3 + parsed_mesh->vertex_property_list.size()))
  {
    // INSERT VERTEX INTO THE MESH
    // i. create new
    bmsh3d_richvertex* V = (bmsh3d_richvertex*) 
      parsed_mesh->mesh->_new_vertex ();
    
    // ii. set properties of the vertex
    V->get_pt().set(parsed_mesh->p);
    for (unsigned int i=0; i < parsed_mesh->vertex_property_list.size(); ++i)
    {
      V->add_scalar_property(parsed_mesh->vertex_property_list[i], 
        parsed_mesh->vertex_property_values[i]);
    }
    // iii. push into the mesh
    parsed_mesh->mesh->_add_vertex (V);
  
    // reset counter
    parsed_mesh->num_vertex_values_read = 0;
  }
  
  
  
  return 1;
}

//: Call-back function for a "face" element
int bmsh3d_fileio_richface_cb_ (p_ply_argument argument) 
{
  long index;
  void* temp;
  ply_get_argument_user_data(argument, &temp, &index);
  bmsh3d_fileio_parsed_richmesh_* parsed_mesh = 
    (bmsh3d_fileio_parsed_richmesh_*) temp;

  
  // read in list of vertices
  if (index == 0) {
    long length, value_index, value;
    ply_get_argument_property(argument, NULL, &length, &value_index);
    value = (int) ply_get_argument_value(argument);     

    assert(length > 0);
    // case: read in number of vertices in the face
    if (value_index == -1) {
      parsed_mesh->vertex_indices.clear();
      parsed_mesh->vertex_indices.reserve(length);
      return 1;
    }
  
    // case: read in indices of the vertex
    parsed_mesh->vertex_indices.push_back(value);

    // flag to indicate whether all vertex indices of the face have been read
    parsed_mesh->all_vertex_indices_of_face_read = ( (value_index+1) >= length );
  }

  // other properties of the face
  // only read in the desired number of property values
  else if (index < (long)(1 + parsed_mesh->face_property_list.size())) {
    parsed_mesh->face_property_values[index-1] = ply_get_argument_value(argument);
    ++parsed_mesh->num_face_values_read;
  }

  // insert face into the mesh when everything about the face has been read (vertex indices
  // and other properties)
  if (parsed_mesh->num_face_values_read == parsed_mesh->face_property_list.size() &&
      parsed_mesh->all_vertex_indices_of_face_read ) {
    bmsh3d_mesh* M = parsed_mesh->mesh;
    
    //Create a new mesh face
    bmsh3d_richface* F = (bmsh3d_richface*)(M->_new_face ());
    
    // a. List of vertices
    // get pointers to the vertices of the new face and insert to the face
    for (unsigned int k=0; k < parsed_mesh->vertex_indices.size(); ++k ) {
      // get pointer to the vertex from its index
      int ind = parsed_mesh->vertex_indices[k];
      bmsh3d_vertex* V = M->vertexmap(ind);
      assert (V);
      F->_ifs_add_vertex (V);
    }

    // b. extra properties
    for (unsigned i=0; i < parsed_mesh->face_property_list.size(); ++i) {
      F->add_scalar_property(parsed_mesh->face_property_list[i], 
        parsed_mesh->face_property_values[i]);
    }

    M->_add_face (F);
    // reset other variable of parsed mesh
    parsed_mesh->num_face_values_read = 0;
    parsed_mesh->all_vertex_indices_of_face_read = false;
  }

  return 1;
}*/

// ----------------------------------------------------------------------------
//: Load rich mesh
// Limitation: CAN only load vertex properties, CANNOT load face properties
/*bool bmsh3d_load_ply(bmsh3d_richmesh* M, 
                      const char* file, 
                      const vcl_vector<vcl_string >& vertex_property_list,
                      const vcl_vector<vcl_string >& face_property_list)
{
  if (!face_property_list.empty())
  {
    vcl_cerr << "\nWARNING: The current implementation cannot load face properties."
      << " None of the specified face properties will be loaded.\n";
  }


  long nvertices, ntriangles;

  bmsh3d_fileio_parsed_richmesh_ parsed_mesh;
  parsed_mesh.mesh = M;
  
  parsed_mesh.vertex_property_list = vertex_property_list;
  parsed_mesh.vertex_property_values.resize(vertex_property_list.size(), 0);
  parsed_mesh.num_vertex_values_read = 0;

  parsed_mesh.face_property_list = face_property_list;
  parsed_mesh.face_property_values.resize(face_property_list.size(), 0);
  parsed_mesh.num_face_values_read = 0;
  parsed_mesh.all_vertex_indices_of_face_read = false;


  // OPEN file
  p_ply ply = ply_open(file, NULL, 0, NULL);
  if (!ply) return false;

  // Read HEADER
  if (!ply_read_header(ply)) return false;

  // vertex
  nvertices = ply_set_read_cb(ply, "vertex", "x", 
    bmsh3d_fileio_richvertex_cb_, (void*) (&parsed_mesh), 0);
  ply_set_read_cb(ply, "vertex", "y", 
    bmsh3d_fileio_richvertex_cb_, (void*) (&parsed_mesh), 1);
  ply_set_read_cb(ply, "vertex", "z", 
    bmsh3d_fileio_richvertex_cb_, (void*) (&parsed_mesh), 2);

  for (unsigned int i=0; i<vertex_property_list.size(); ++i) {
    ply_set_read_cb(ply, "vertex", 
      vertex_property_list[i].c_str(), 
      bmsh3d_fileio_richvertex_cb_, 
      (void*)(&parsed_mesh), 3+i);
  }

  
  // face
  //ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", 
    //bmsh3d_fileio_face_cb_, (void*) (&parsed_mesh), 0);

  ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", 
    bmsh3d_fileio_richface_cb_, (void*) (&parsed_mesh), 0);

  for (unsigned int i=0; i<face_property_list.size(); ++i)
  {
    ply_set_read_cb(ply, "face", 
      face_property_list[i].c_str(), 
      bmsh3d_fileio_richface_cb_, 
      (void*)(&parsed_mesh), 1+i);
  }
  
  // Read DATA
  if (!ply_read(ply)) return false;

  // CLOSE file
  ply_close(ply);

  return true;
}

// ----------------------------------------------------------------------------
//: Save a rich mesh given a list of vertex properties and face properties to read
bool bmsh3d_save_ply (bmsh3d_richmesh* mesh, 
                       const char* file, 
                       const vcl_vector<vcl_string >& vertex_property_list,
                       const vcl_vector<vcl_string >& face_property_list,
                       bmsh3d_storage_mode mode)
{
  if (!face_property_list.empty())
  {
    vcl_cerr << "\nWARNING: The current implementation cannot save face properties."
      << " None of the specified face properties will be saved.\n";
  }

  // determine storage mode
  e_ply_storage_mode storage_mode = (mode==ASCII) ? PLY_ASCII : PLY_LITTLE_ENDIAN;;

  // OPEN FILE
  p_ply oply = ply_create(file, storage_mode, NULL, 0, NULL);

  vul_printf (vcl_cerr, "  saving %s : \n\t%d points, %d faces ...\n", 
               file, mesh->vertexmap().size(), mesh->facemap().size());

  // HEADER SECTION
  // vertex
  ply_add_element(oply, "vertex", mesh->vertexmap().size());
  ply_add_scalar_property(oply, "x", PLY_FLOAT);
  ply_add_scalar_property(oply, "y", PLY_FLOAT);
  ply_add_scalar_property(oply, "z", PLY_FLOAT);
  for (unsigned i=0; i<vertex_property_list.size(); ++i)
  {
    ply_add_scalar_property(oply, vertex_property_list[i].c_str(), PLY_DOUBLE);
  }

  // face
  ply_add_element(oply, "face", mesh->facemap().size());
  ply_add_list_property(oply, "vertex_indices", PLY_UCHAR, PLY_INT);
  for (unsigned i=0; i<face_property_list.size(); ++i)
  {
    ply_add_scalar_property(oply, face_property_list[i].c_str(), PLY_DOUBLE);
  }
  
  
  // comment
  ply_add_comment(oply, "created by bmsh3d_save_ply");

  // object info
  ply_add_obj_info(oply, "an bmsh3d_richmesh object");

  // end header
  ply_write_header(oply);

  

  // DATA SECTION

  // reset the vid's of the vertices to reset its order in the vertex list,
  // which will be saved to the PLY file
  mesh->assign_IFS_vertex_vid_by_vertex_order();

  // traverse thru all vertices and write to ply file
  vcl_map<int, bmsh3d_vertex*>::iterator it = mesh->vertexmap().begin();
  for (; it != mesh->vertexmap().end(); it++) {
    bmsh3d_richvertex* v = (bmsh3d_richvertex*) (*it).second;
    vgl_point_3d<double > pt = v->pt();

    ply_write(oply, pt.x());
    ply_write(oply, pt.y());
    ply_write(oply, pt.z());

    // extra properties to save
    for (unsigned i=0; i<vertex_property_list.size(); ++i)
    {
      vcl_string tag = vertex_property_list[i];
      double value;

      // if can't find the value, save it with max value possible for double
      if (!v->get_scalar_property(tag, value))
      {
        value = vnl_numeric_traits<double>::maxval;
      }
      ply_write(oply, value);
    }
  }

  // faces
  vcl_map<int, bmsh3d_face*>::iterator fit = mesh->facemap().begin();
  for (; fit != mesh->facemap().end(); fit++) {
    //bmsh3d_face* f = (*fit).second;

    bmsh3d_richface* f = (bmsh3d_richface*)(*fit).second;

    // a. write the number of vertices of the face
    ply_write(oply,f->vertices().size());

    // b. write id's of the vertices
    for (unsigned int j=0; j< f->vertices().size(); ++j) {
      bmsh3d_vertex* v = f->vertices(j);
      assert(v);
      
      // vid's contain the order of the vertices in the vertex map
      unsigned int id = v->vid();

      ply_write(oply, id);
    }

    // c. write extra properties of the rich face
    for (unsigned i=0; i<face_property_list.size(); ++i) {
      vcl_string tag = face_property_list[i];
      double value;

      // if can't find the value, save it with max value possible for double
      if (!f->get_scalar_property(tag, value))
      {
        value = vnl_numeric_traits<double>::maxval;
      }
      ply_write(oply, value);
    }
  }

  // CLOSE PLY FILE
  ply_close(oply);
  vul_printf (vcl_cerr, "  done.\n");
  return true;
}
*/