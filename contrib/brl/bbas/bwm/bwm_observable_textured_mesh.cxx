#include <iostream>
#include "bwm_observable_textured_mesh.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vgl/vgl_point_2d.h>

void bwm_observable_textured_mesh::save_gml(std::ostream& os, int obj_count, vpgl_lvcs* lvcs)
{
  if (lvcs) {
      //bmsh3d_textured_mesh_mc* mesh = static_cast<bmsh3d_textured_mesh_mc*>(object_);
      os << "<cityObjectMember>\n";
      os << "<Building>\n";
      os << "<gml:description>Building #" << obj_count << "</gml:description>\n";
      os << "<gml:name>Building #" << obj_count << "</gml:name>\n";

      std::map<int, bmsh3d_face*>::iterator fit;
      for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
        bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;

        os << "<boundedBy>";
        os << "<WallSurface>";
        os << "<lod4MultiSurface><gml:MultiSurface>\n";
        os << "<gml:surfaceMember>";
        os << "<TexturedSurface orientation=\"+\">";
        os << "<gml:baseSurface>";
        os << "<gml:Polygon>";
        os << "<gml:exterior>";
        os << "<gml:LinearRing>\n";

        for (unsigned j=0; j<face->vertices().size(); j++) {
          bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
           double x,y,z;
            lvcs->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),vpgl_lvcs::wgs84,x,y,z,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
            os << "<gml:pos srsDimension=\"3\">";
            os << x << " " << y << " " << z;
            os << "</gml:pos>\n";
        }

        //Now print the first vertex again to close the polygon
        bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(0);
        double x,y,z;
        lvcs->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),vpgl_lvcs::wgs84,x,y,z,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
        os << "<gml:pos srsDimension=\"3\">";
        os << x << " " << y << " " << z;
        os << "</gml:pos>\n";

        os << "</gml:LinearRing>";
        os << "</gml:exterior>";
        os << "</gml:Polygon>";
        os << "</gml:baseSurface>";

        // texture
        os << "<appearance><SimpleTexture>\n";
        os << "<textureMap>" << vul_file::strip_directory(face->tex_map_uri().c_str()) << "</textureMap>\n";
        os << "<textureCoordinates>";
        for (unsigned j=0; j<face->vertices().size(); j++) {
          v = (bmsh3d_vertex*) face->vertices(j);
          //bmsh3d_textured_vertex_3d* tv = (bmsh3d_textured_vertex_3d*) face->vertices(j);
          vgl_point_2d<double> pt_tex = face->tex_coords(v->id());
          os << pt_tex.x() << " " << pt_tex.y() << " ";
        }

        //Now print the first vertex again to close the polygon
        v = (bmsh3d_vertex*) face->vertices(0);
        vgl_point_2d<double> pt_tex = face->tex_coords(v->id());
        os << pt_tex.x() << " " << pt_tex.y() << " ";

        os <<"</textureCoordinates>\n";
        os <<"<textureType>specific</textureType>\n";
        os <<"</SimpleTexture></appearance>\n";

        os << "</TexturedSurface>\n";
        os << "</gml:surfaceMember>";
        os << "</gml:MultiSurface></lod4MultiSurface>";
        os << "</WallSurface>";
        os << "</boundedBy>\n";
      }
  }
}

void bwm_observable_textured_mesh::save_kml(std::ostream& os, int obj_count, vpgl_lvcs* lvcs,
                                            double ground_height, double x_offset, double y_offset )
{
  if (lvcs) {
    object_->build_IFS_mesh();
    object_->IFS_to_MHE();
    object_->orient_face_normals();
    object_->build_IFS_mesh();

    int poly_count = 0;
    if (object_->facemap().size() <= 1) {
      // single mesh face is probably ground plane, which we do not want to render
      return;
    }

    std::map<int, bmsh3d_face*>::iterator fit;
    for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
      bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;

        os << "      <Polygon id=\"building" << obj_count << "_face" << poly_count << "\">\n";
        os << "        <extrude>0</extrude>\n";
        os << "        <tessellate>0</tessellate>\n";
        os << "        <altitudeMode>relativeToGround</altitudeMode>\n";
        os << "        <outerBoundaryIs>\n";
        os << "          <LinearRing>\n";
        os << "            <coordinates>\n";

        for (unsigned j=0; j<face->vertices().size(); j++) {
         bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
          os << "             " << v->pt().x()+x_offset << ", ";
         os << v->pt().y()+y_offset << ", ";
         os << v->pt().z() - ground_height << "\n";
        }

        //Now print the first vertex again to close the polygon
        bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(0);
        os << "             " << v->pt().x()+x_offset << ", ";
        os << v->pt().y()+y_offset << ", ";
        os << v->pt().z() - ground_height << "\n";

        os << "            </coordinates>\n";
        os << "          </LinearRing>\n";
        os << "        </outerBoundaryIs>\n";
        os << "      </Polygon>\n";
    }
  }
}


void bwm_observable_textured_mesh::save_kml_collada(std::ostream& os, vpgl_lvcs* lvcs,
                                                    std::string geometry_id,
                                                    std::string geometry_position_id,
                                                    std::string geometry_position_array_id,
                                                    std::string geometry_uv_id,
                                                    std::string geometry_uv_array_id,
                                                    std::string geometry_vertex_id,
                                                    std::string material_name)
{
  int nverts = num_vertices();
  int nfaces = num_faces();

  os <<"    <geometry id=\"" << geometry_id.c_str() << "\" name=\"" << geometry_id.c_str() << "\">\n";
  os <<"      <mesh>\n";
  os <<"        <source id=\"" << geometry_position_id.c_str() << "\">\n";
  os <<"        <float_array id=\"" << geometry_position_array_id.c_str() << "\" count=\"" << nverts*3 << "\">\n";

  // map vertex ID's to indices.
  std::map<int,int> vert_indices;


  int vert_idx = 0;
  std::map<int, bmsh3d_vertex*>::iterator vit;
  for (vit = object_->vertexmap().begin(); vit != object_->vertexmap().end(); vit++, vert_idx++) {
    bmsh3d_vertex* v = (bmsh3d_vertex*)vit->second;
    vert_indices[v->id()] = vert_idx;
    double x,y,z;
    lvcs->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),vpgl_lvcs::wgs84,x,y,z,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
    os << x << " " << y << " " << z << " ";
  }

  os <<"\n        </float_array>\n";
  os <<"        <technique_common>\n";
  os <<"          <accessor source=\"#" << geometry_position_array_id.c_str() << "\" count=\"" << nverts << "\" stride=\"3\">\n";
  os <<"            <param name=\"X\" type=\"float\"/>\n";
  os <<"            <param name=\"Y\" type=\"float\"/>\n";
  os <<"            <param name=\"Z\" type=\"float\"/>\n";
  os <<"          </accessor>\n";
  os <<"        </technique_common>\n";
  os <<"      </source>\n";
  os <<"      <source id=\"" << geometry_uv_id.c_str() << "\">\n";

  // determine total number of corners in mesh
  int ncorners = 0;
  std::map<int, bmsh3d_face*>::iterator fit;
  for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
    bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
    ncorners += face->vertices().size();
  }

  os <<"        <float_array id=\"" << geometry_uv_array_id.c_str() << "\" count=\"" << ncorners*2 << "\">\n";
  for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
    bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
    for (unsigned j=0; j<face->vertices().size(); j++) {
      bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
      vgl_point_2d<double> pt = face->tex_coords(v->id());
      os << pt.x() << " " << pt.y() << " ";
      }
  }

  os <<"\n        </float_array>\n";
  os <<"        <technique_common>\n";
  os <<"          <accessor source=\"#" << geometry_uv_array_id.c_str() << "\" count=\"" << ncorners << "\" stride=\"2\">\n";
  os <<"            <param name=\"S\" type=\"float\"/>\n";
  os <<"            <param name=\"T\" type=\"float\"/>\n";
  os <<"          </accessor>\n";
  os <<"        </technique_common>\n";
  os <<"      </source>\n";

  os <<"      <vertices id=\"" << geometry_vertex_id.c_str() << "\">\n";
  os <<"        <input semantic=\"POSITION\" source=\"#" << geometry_position_id.c_str() << "\"/>\n";
  os <<"      </vertices>\n";
  os <<"      <triangles material=\"" << material_name.c_str() << "\" count=\"" << nfaces << "\">\n";
  os <<"        <input semantic=\"VERTEX\" source=\"#" << geometry_vertex_id.c_str() <<  "\" offset=\"0\"/>\n";
  os <<"        <input semantic=\"TEXCOORD\" source=\"#" << geometry_uv_id.c_str() << "\" offset=\"1\" set=\"0\"/>\n";


  int tex_idx = 0;
  os <<"        <p>";
  for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
    bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;

    if (face->vertices().size() != 3) {
      std::cerr << "ERROR! only triangle meshes are supported. Face has "<<face->vertices().size()<<" vertices.\n";
    }

   for (unsigned j=0; j< 3; j++) {
      bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
      os << vert_indices[v->id()] << " " << tex_idx << " ";
   }
  }
}

void bwm_observable_textured_mesh::save_x3d(std::ostream &os, vpgl_lvcs* lvcs)
{
  if (!lvcs)
    return;

  if (!os.bad()) {
    std::string texmap_url = tex_map_uri();

    os << "Transform {\n";
    os << "  children\n";
    os << "  Shape {\n";
    os << "    appearance Appearance {\n";
    os << "      material Material{}\n";
    os << "      texture ImageTexture {\n";
    os << "        url \"" << texmap_url.c_str() << "\"\n";
    os << "      }\n";
    os << "    }\n";
    os << "    geometry IndexedFaceSet {\n";
    os << "      coord Coordinate {\n";
    os << "        point [\n";

    // map vertex ID's to indices.
    std::map<int,int> vert_indices;

    std::map<int, bmsh3d_vertex*>::iterator vit;
    int idx = 0;
    for (vit = object_->vertexmap().begin(); vit != object_->vertexmap().end(); vit++, idx++) {
      bmsh3d_vertex* v = (bmsh3d_vertex*)vit->second;
      vert_indices[v->id()] = idx;
      double x,y,z;
      lvcs->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),vpgl_lvcs::wgs84,x,y,z,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
      os <<"       " << x << " " << y << " " << z << ",\n";
    }
    os << "        ]\n";
    os << "      }\n";
    os << "      coordIndex[\n";

    std::map<int, bmsh3d_face*>::iterator fit;
    for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
      bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
      os << "             ";
      for (unsigned j=0; j<face->vertices().size(); j++) {
        bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
        os << vert_indices[v->id()] << " ";
      }
      os << "-1,\n";
    }
    os << "      ]\n\n";

    os << "      texCoord TextureCoordinate {\n";
    os << "        point [\n";

    for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
      bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
      for (unsigned j=0; j<face->vertices().size(); j++) {
        bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
        vgl_point_2d<double> pt = face->tex_coords(v->id());
        os << "           " << pt.x() << " " << pt.y() << ",\n";
      }
    }
    os << "        ]\n";
    os << "      }\n\n";

    os << "      texCoordIndex[\n";
    int tex_coord_idx = 0;
    for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
      bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
      os << "                ";
      for (unsigned j=0; j < face->vertices().size(); j++) {
        os << tex_coord_idx++ << " ";
      }
      os << "-1,\n";
    }
  }
}
