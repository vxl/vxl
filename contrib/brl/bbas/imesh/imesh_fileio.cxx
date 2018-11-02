// This is brl/bbas/imesh/imesh_fileio.cxx
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include "imesh_fileio.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vgl/vgl_point_2d.h>


//: Read a mesh from a file, determine type from extension
bool imesh_read(const std::string& filename, imesh_mesh& mesh)
{
  std::string ext = vul_file::extension(filename);
  if (ext == ".ply2")
    return imesh_read_ply2(filename,mesh);
  if (ext == ".ply")
    return imesh_read_ply(filename,mesh);
  else if (ext == ".obj")
    return imesh_read_obj(filename,mesh);

  return false;
}


//: Read a mesh from a PLY2 file
bool imesh_read_ply2(const std::string& filename, imesh_mesh& mesh)
{
  std::ifstream fh(filename.c_str());
  bool retval = imesh_read_ply2(fh,mesh);
  fh.close();
  return retval;
}


//: Read a mesh from a PLY2 stream
bool imesh_read_ply2(std::istream& is, imesh_mesh& mesh)
{
  unsigned int num_verts, num_faces;
  is >> num_verts >> num_faces;
  std::unique_ptr<imesh_vertex_array<3> > verts(new imesh_vertex_array<3>(num_verts));
  std::unique_ptr<imesh_face_array > faces(new imesh_face_array(num_faces));
  for (unsigned int v=0; v<num_verts; ++v) {
    imesh_vertex<3>& vert = (*verts)[v];
    is >> vert[0] >> vert[1] >> vert[2];
  }
  for (unsigned int f=0; f<num_faces; ++f) {
    std::vector<unsigned int>& face = (*faces)[f];
    unsigned int cnt;
    is >> cnt;
    face.resize(cnt,0);
    for (unsigned int v=0; v<cnt; ++v)
      is >> face[v];
  }

  mesh.set_vertices(std::unique_ptr<imesh_vertex_array_base>(std::move(verts)));
  mesh.set_faces(std::unique_ptr<imesh_face_array_base>(std::move(faces)));
  return true;
}

//: Read a mesh from a PLY2 file
bool imesh_read_ply(const std::string& filename, imesh_mesh& mesh)
{
  std::ifstream fh(filename.c_str());
  bool retval = imesh_read_ply(fh,mesh);
  fh.close();
  return retval;
}


//: Read a mesh from a PLY2 stream
bool imesh_read_ply(std::istream& is, imesh_mesh& mesh)
{
  unsigned int num_verts, num_faces;
  std::string str;
  is >> str;
  bool done=false;
  while (!done) {
    is >> str;
    if (str.compare("element")==0)  {
      is >> str;
      if (str.compare("vertex")==0) {
        is >> num_verts;
      }
      else if (str.compare("face")==0) {
        is >> num_faces;
      }
    }
    else if (str.compare("end_header")==0)  {
      done = true;
    }
  }
  std::unique_ptr<imesh_vertex_array<3> > verts(new imesh_vertex_array<3>(num_verts));
  std::unique_ptr<imesh_face_array > faces(new imesh_face_array(num_faces));
  for (unsigned int v=0; v<num_verts; ++v) {
    imesh_vertex<3>& vert = (*verts)[v];
    is >> vert[0] >> vert[1] >> vert[2];
  }
  for (unsigned int f=0; f<num_faces; ++f) {
    std::vector<unsigned int>& face = (*faces)[f];
    unsigned int cnt;
    is >> cnt;
    face.resize(cnt,0);
    for (unsigned int v=0; v<cnt; ++v)
      is >> face[v];
  }

  mesh.set_vertices(std::unique_ptr<imesh_vertex_array_base>(std::move(verts)));
  mesh.set_faces(std::unique_ptr<imesh_face_array_base>(std::move(faces)));
  return true;
}

//: Write a mesh to a PLY2 file
void imesh_write_ply2(const std::string& filename, const imesh_mesh& mesh)
{
  std::ofstream fh(filename.c_str());
  imesh_write_ply2(fh,mesh);
  fh.close();
}


//: Write a mesh to a PLY2 stream
void imesh_write_ply2(std::ostream& os, const imesh_mesh& mesh)
{
  os << mesh.num_verts() <<'\n'<< mesh.num_faces() <<'\n';
  const imesh_vertex_array_base& verts = mesh.vertices();
  for (unsigned int v=0; v<verts.size(); ++v) {
    os << verts(v,0) << ' '
       << verts(v,1) << ' '
       << verts(v,2) << '\n';
  }

  const imesh_face_array_base& faces = mesh.faces();
  for (unsigned int f=0; f<faces.size(); ++f) {
    os << faces.num_verts(f);
    for (unsigned int v=0; v<faces.num_verts(f); ++v)
      os << ' ' << faces(f,v);
    os << '\n';
  }
}


//: Read texture coordinates from a UV2 file
bool imesh_read_uv2(const std::string& filename, imesh_mesh& mesh)
{
   std::ifstream fh(filename.c_str());
   bool retval = imesh_read_uv2(fh,mesh);
   fh.close();
   return retval;
}


//: Read texture coordinates from a UV2 stream
bool imesh_read_uv2(std::istream& is, imesh_mesh& mesh)
{
   std::vector<vgl_point_2d<double> > uv;
   unsigned int num_verts, num_faces;
   is >> num_verts >> num_faces;
   if (num_verts != mesh.num_verts() && num_verts != mesh.num_edges()*2)
     return false;

   for (unsigned int i=0; i<num_verts; ++i) {
      double u,v;
      is >> u >> v;
      uv.emplace_back(u,v);
   }
   mesh.set_tex_coords(uv);
   return true;
}


//: Read a mesh from a wavefront OBJ file
bool imesh_read_obj(const std::string& filename, imesh_mesh& mesh)
{
  std::ifstream fh(filename.c_str());
  bool retval = imesh_read_obj(fh,mesh);
  fh.close();
  return retval;
}


//: Read a mesh from a wavefront OBJ stream
bool imesh_read_obj(std::istream& is, imesh_mesh& mesh)
{
  std::unique_ptr<imesh_vertex_array<3> > verts(new imesh_vertex_array<3>);
  std::unique_ptr<imesh_face_array> faces(new imesh_face_array);
  std::vector<vgl_vector_3d<double> > normals;
  std::vector<vgl_point_2d<double> > tex;
  std::string last_group = "ungrouped";
  char c;
  while (is >> c)
  {
    switch (c)
    {
      case 'v': // read a vertex
      {
        char c2 = (char)is.peek();
        switch (c2)
        {
          case 'n': // read a normal
          {
            is.ignore();
            double x,y,z;
            is >> x >> y >> z;
            normals.emplace_back(x,y,z);
            break;
          }
          case 't': // read a texture coord
          {
            is.ignore();
            double x,y;
            is >> x >> y;
            is.ignore(256,'\n');
            tex.emplace_back(x,y);
            break;
          }
          default: // read a position
          {
            double x,y,z;
            is >> x >> y >> z;
            verts->push_back(imesh_vertex<3>(x,y,z));
            break;
          }
        }
        break;
      }
      case 'f':
      {
        std::string line;
        std::getline(is,line);
        std::vector<unsigned int> vi, ti, ni;
        unsigned int v;
        std::stringstream ss(line);
        while (ss >> v) {
          vi.push_back(v-1);
          if (ss.peek() == '/') {
            ss.ignore();
            if (ss.peek() != '/') {
              ss >> v;
              ti.push_back(v-1);
              if (ss.peek() == '/') {
                ss.ignore();
                if (ss.peek() >= '0' && ss.peek() <= '9') {
                  ss >> v;
                  ni.push_back(v-1);
                }
                else {
                  std::cerr << "improperly formed face line in OBJ: "<<line<<'\n';
                  return false;
                }
              }
            }
            else {
              ss.ignore();
              if (ss.peek() >= '0' && ss.peek() <= '9') {
                ss >> v;
                ni.push_back(v-1);
              }
              else {
                std::cerr << "improperly formed face line in OBJ: "<<line<<'\n';
                return false;
              }
            }
          }
        }
        faces->push_back(vi);
        break;
      }
      case 'g':
      {
        faces->make_group(last_group);
        is.ignore();
        std::getline(is,last_group);
        break;
      }
      default:
        is.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        break;
    }
  }

  // make the last group
  if (faces->has_groups())
    faces->make_group(last_group);

  if (normals.size() == verts->size())
    verts->set_normals(normals);

  mesh.set_vertices(std::unique_ptr<imesh_vertex_array_base>(std::move(verts)));
  mesh.set_faces(std::unique_ptr<imesh_face_array_base>(std::move(faces)));
  mesh.set_tex_coords(tex);

  return true;
}


//: Write a mesh to a wavefront OBJ file
void imesh_write_obj(const std::string& filename, const imesh_mesh& mesh)
{
  std::ofstream fh(filename.c_str());
  imesh_write_obj(fh,mesh);
  fh.close();
}


//: Write a mesh to a wavefront OBJ stream
// Rotated about X because Y is up in wavefront (not Z)
void imesh_write_obj(std::ostream& os, const imesh_mesh& mesh)
{
  const imesh_vertex_array_base& verts = mesh.vertices();
  for (unsigned int v=0; v<verts.size(); ++v) {
    os << "v "
       << verts(v,0) << ' '
       << verts(v,1) << ' '
       << verts(v,2) << '\n';
  }

  if (verts.has_normals()) {
    for (unsigned int n=0; n<verts.size(); ++n) {
      const vgl_vector_3d<double>& v = verts.normal(n);
      os << "vn "
         << v.x() << ' '
         << v.y() << ' '
         << v.z() << '\n';
    }
  }

  if (mesh.has_tex_coords()) {
    const std::vector<vgl_point_2d<double> >& tex = mesh.tex_coords();
    for (auto t : tex) {
      os << "vt " << t.x() << ' ' << t.y() << '\n';
    }
  }

  const imesh_face_array_base& faces = mesh.faces();
  const std::vector<std::pair<std::string,unsigned int> >& groups = faces.groups();

  bool write_extra = mesh.has_tex_coords() || verts.has_normals();

  if (!groups.empty())
    os << "g " << groups[0].first << '\n';
  unsigned int g=0;
  unsigned int e=0;
  for (unsigned int f=0; f<faces.size(); ++f)
  {
    while (g < groups.size() && groups[g].second <= f) {
      ++g;
      if (g < groups.size())
        os << "g " << groups[g].first << '\n';
      else {
        os << "g ungrouped\n";
      }
    }
    os << 'f';
    for (unsigned int v=0; v<faces.num_verts(f); ++v) {
      os << ' ' << faces(f,v)+1;
      if (write_extra) {
        os << '/';
        if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_CORNER) {
          os << ++e;
        }
        if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT)
          os << faces(f,v)+1;
        if (verts.has_normals())
          os << '/' << faces(f,v)+1;
      }
    }
    os << '\n';
  }
}

void imesh_write_kml(std::ostream& os, const imesh_mesh& mesh)
{
  const imesh_face_array_base& faces = mesh.faces();
  const imesh_vertex_array_base& verts = mesh.vertices();

  if (faces.size() <= 1) {
      // single mesh face is probably ground plane, which we do not want to render
      return;
  }

  os.precision(12);
  for (unsigned int f=0; f<faces.size(); ++f)
  {
    // originally it was id=buildingX_faceY, but we have to pass a mesh number
    os << "      <Polygon id=\"building_face" << f << "\">\n"
       << "        <extrude>0</extrude>\n"
       << "        <tessellate>0</tessellate>\n"
       << "        <altitudeMode>absolute</altitudeMode>\n"
       << "        <outerBoundaryIs>\n"
       << "          <LinearRing>\n"
       << "            <coordinates>" << std::endl;

    for (unsigned int v=0; v<faces.num_verts(f); ++v) {
      unsigned int idx = faces(f,v);
      double x = verts(idx, 0);
      double y = verts(idx, 1);
      double z = verts(idx, 2);
      os << "             " << x  << ", " << y << ", " << z << std::endl;
    }

    //Now print the first vertex again to close the polygon
    unsigned int idx = faces(f,0);
    double x = verts(idx, 0);
    double y = verts(idx, 1);
    double z = verts(idx, 2);
    os << "             " << x  << ", " << y << ", " << z << std::endl
       << "            </coordinates>\n"
       << "          </LinearRing>\n"
       << "        </outerBoundaryIs>\n"
       << "      </Polygon>" << std::endl;
  }
}

void imesh_write_kml_collada(std::ostream& os, const imesh_mesh& mesh)
{
  // get mesh faces as triangles
  if (mesh.faces().regularity() != 3)
  {
    std::cerr << "ERROR! only triangle meshes are supported.\n";
    return;
  }
  const auto& tris =
      static_cast<const imesh_regular_face_array<3>&>(mesh.faces());
  const imesh_vertex_array<3>& verts = mesh.vertices<3>();
  const unsigned int nverts = verts.size();
  const unsigned int nfaces = tris.size();

  std::string geometry_id = "geometry";
  std::string geometry_position_id = "geometry_position";
  std::string geometry_position_array_id = "geometry_position_array";
  std::string geometry_uv_id = "geometry_uv";
  std::string geometry_uv_array_id = "geometry_uv_array";
  std::string geometry_vertex_id = "geometry_vertex";
  std::string geometry_normal_id = "geometry_normal";

  // Write the COLLADA XML
  os <<"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
     << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n"

     << "  <asset>\n"
     << "    <contributor>\n"
     << "      <authoring_tool>VXL imesh library</authoring_tool>\n"
     << "    </contributor>\n"
#if 0 // When we figure out how to get a date string we can write this
     << "    <created>2008-04-08T13:07:52-08:00</created>\n"
     << "    <modified>2008-04-08T13:07:52-08:00</modified>\n"
#endif
     << "    <unit name=\"meters\" meter=\"1\"/>\n"
     << "    <up_axis>Z_UP</up_axis>\n"
     << "  </asset>\n";

  // Write materials.  Use a single grey material
  os << "  <library_materials>" << std::endl
     << "    <material id=\"GreyID\" name=\"Grey\">"<< std::endl
     << "       <instance_effect url=\"#Grey-effect\"/>"<< std::endl
     << "    </material>"<< std::endl
     << "  </library_materials>"<< std::endl

     << "  <library_effects>"<< std::endl
     << "    <effect id=\"Grey-effect\" name=\"Grey-effect\">\n"
     << "      <profile_COMMON>\n"
     << "         <technique sid=\"COMMON\">\n"
     << "            <phong>\n"
     << "              <diffuse>\n"
     << "                 <color>1.000000 1.000000 1.000000 1</color>\n"
     << "              </diffuse>\n"
     << "            </phong>\n"
     << "         </technique>\n"
     << "         <extra>\n"
     << "         <technique profile=\"GOOGLEEARTH\">\n"
     << "           <double_sided>1</double_sided>\n"
     << "         </technique>\n"
     << "         </extra>\n"
     << "      </profile_COMMON>\n"
     << "    </effect>\n"
     << "  </library_effects>\n";

  // write geometry
  os << "  <library_geometries>\n"
     <<"    <geometry id=\"" << geometry_id << "\" name=\"" << geometry_id << "\">\n"
     <<"      <mesh>\n"
     <<"        <source id=\"" << geometry_position_id << "\">\n"
     <<"        <float_array id=\"" << geometry_position_array_id << "\" count=\"" << nverts*3 << "\">\n";

  for (unsigned int v=0; v<nverts; ++v)
    os << "          "<< verts[v][0] << ' ' << verts[v][1] << ' ' << verts[v][2] << '\n';

  os <<"\n        </float_array>\n"
     <<"        <technique_common>\n"
     <<"          <accessor source=\"#" << geometry_position_array_id << "\" count=\"" << nverts << "\" stride=\"3\">\n"
     <<"            <param name=\"X\" type=\"float\"/>\n"
     <<"            <param name=\"Y\" type=\"float\"/>\n"
     <<"            <param name=\"Z\" type=\"float\"/>\n"
     <<"          </accessor>\n"
     <<"        </technique_common>\n"
     <<"      </source>\n"
     <<"      <source id=\"" << "geometry_normal" << "\">\n"

     <<"        <float_array id=\"" << "geometry_normal_array" << "\" count=\"" << nfaces*3 << "\">\n";
  for (unsigned int f=0; f<nfaces; ++f) {
    const vgl_vector_3d<double>& n = tris.normal(f);
    os << "          " << n.x() << ' ' << n.y() << ' ' << n.z() << '\n';
  }

  os <<"\n        </float_array>\n"
     <<"        <technique_common>\n"
     <<"          <accessor source=\"#" << "geometry_normal_array" << "\" count=\"" << nfaces << "\" stride=\"3\">\n"
     <<"            <param name=\"X\" type=\"float\"/>\n"
     <<"            <param name=\"Y\" type=\"float\"/>\n"
     <<"            <param name=\"Z\" type=\"float\"/>\n"
     <<"          </accessor>\n"
     <<"        </technique_common>\n"
     <<"      </source>\n"

     <<"      <vertices id=\"" <<geometry_vertex_id << "\">\n"
     <<"        <input semantic=\"POSITION\" source=\"#" << geometry_position_id << "\"/>\n"
     <<"      </vertices>\n"
     <<"      <triangles material=\"Grey\" count=\"" << nfaces << "\">\n"
     <<"        <input semantic=\"VERTEX\" source=\"#" << geometry_vertex_id <<  "\" offset=\"0\"/>\n"
     <<"        <input semantic=\"NORMAL\" source=\"#" << "geometry_normal" <<  "\" offset=\"1\"/>\n"

     <<"        <p>\n";

  for (unsigned int f=0; f<nfaces; ++f) {
    os << "          "
       << tris[f][0] << ' ' << f <<"  "
       << tris[f][1] << ' ' << f <<"  "
       << tris[f][2] << ' ' << f <<'\n';
  }
  os << "        </p>\n"
     << "      </triangles>\n"
     << "    </mesh>\n"
     << "  </geometry>\n"
     << "  </library_geometries>\n";

  // write the scene
  os << "  <library_visual_scenes>\n"
     << "    <visual_scene id=\"vis_scene\">\n"
     << "      <node id=\"Model\" name=\"Model\">\n"
     << "        <node id=\"mesh\" name=\"mesh\">\n"
     << "          <instance_geometry url=\"#geometry\">\n"
     << "            <bind_material>\n"
     << "              <technique_common>\n"
     << "                <instance_material symbol=\"Grey\" target=\"#GreyID\"/>\n"
     << "              </technique_common>\n"
     << "            </bind_material>\n"
     << "          </instance_geometry>\n"
     << "        </node>"
     << "      </node>"
     << "    </visual_scene>\n"
     << "  </library_visual_scenes>\n"
     << "  <scene>\n"
     << "    <instance_visual_scene url=\"#vis_scene\"/>\n"
     << "  </scene>\n"
     << "</COLLADA>\n";
}

void imesh_write_vrml(std::ostream& os, const imesh_mesh& mesh)
{
  // get mesh faces as triangles
  if (mesh.faces().regularity() != 3)
  {
    std::cerr << "ERROR! only triangle meshes are supported.\n";
    return;
  }
  const auto& tris =
    static_cast<const imesh_regular_face_array<3>&>(mesh.faces());
  const imesh_vertex_array_base& vertsb = mesh.vertices();
  unsigned d = vertsb.dim();
  if (d!=2&&d!=3)
  {
    std::cerr << "vertex dimension must be 2 or 3.\n";
    return;
  }
  const unsigned int nfaces = tris.size();
  os << "#VRML V2.0 utf8\n\n"
     << "Transform {\n"
     << "  translation 0 0 0\n"
     << "  children\n"
     << " Shape {\n";

  //write appearance
  os << "  appearance Appearance {\n"
     << "    material Material{}\n"
     << "    texture ImageTexture {\n"
     << "      url \""<< mesh.tex_source() <<"\"\n"
     << "    }\n"
     << "  }\n";

  //write coordinates in 2 or 3d
  os << " geometry IndexedFaceSet\n"
     << "  {\n"
     << "   coord Coordinate{\n"
     << "    point [\n";
  if (d == 2) {
    const imesh_vertex_array<2>& verts2= mesh.vertices<2>();
    for (auto i : verts2)
      os << "    " << i[0] << ' ' << i[1] << ' ' << 0.0 << '\n';
  }
  else {
    const imesh_vertex_array<3>& verts3= mesh.vertices<3>();
    for (const auto & i : verts3)
      os << "    " << i[0] << ' ' << i[1] << ' ' << i[2] << '\n';
  }
  os << "    ]}\n";

  //write faces (all triangles)
  os << "  coordIndex [\n";
  for (unsigned i=0;i<nfaces;++i) {
    os << "    "
       << tris[i][0] << ' '
       << tris[i][1] << ' '
       << tris[i][2] << ' '
       << -1 << '\n';
  }
  os << "  ]\n";

  //write texture coordinates
  if (mesh.has_tex_coords() == imesh_mesh::TEX_COORD_ON_VERT) {
    os << " texCoord TextureCoordinate {\n"
       << "   point [\n";

    //write tex coordinates (should be same number as vertices above)
    const std::vector<vgl_point_2d<double> >& tc = mesh.tex_coords();
    for (auto i : tc)
      os << "    " << i.x() << ' ' << i.y() << ",\n";

    //close texture coordinates
    os << "    ]}\n";

    //write face mapping again
    os << "   texCoordIndex [\n";
    for (unsigned i=0;i<nfaces;++i) {
      os << "    "
         << tris[i][0] << ' '
         << tris[i][1] << ' '
         << tris[i][2] << ' '
         << -1 << '\n';
    }
    os << "  ]\n";
  }

  //
  os << "solid TRUE\n"
     << "convex FALSE\n"
     << "creaseAngle 0\n";

#if 0
  os << "  colorPerVertex FALSE\n"
     << "  color Color {\n"
     << "   color [ ]\n"
     << "  }\n"
     << "  colorIndex [ ]\n";
#endif

  //close geometry indexed face set
  os << " }\n";

  //close shape
  os << "}\n";

  //close transform
  os << "}\n";
}
