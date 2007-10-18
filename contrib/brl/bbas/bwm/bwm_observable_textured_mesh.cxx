#include "bwm_observable_textured_mesh.h"

#include <vul/vul_file.h>

void bwm_observable_textured_mesh::save_gml(FILE* fp, int obj_count, bgeo_lvcs* lvcs)
{
  if (lvcs) {
      //bmsh3d_textured_mesh_mc* mesh = static_cast<bmsh3d_textured_mesh_mc*>(object_);
      vcl_fprintf (fp, "<cityObjectMember>\n");
      vcl_fprintf (fp, "<Building>\n");
      vcl_fprintf (fp, "<gml:description>Building #%d</gml:description>\n",obj_count);
      vcl_fprintf (fp, "<gml:name>Building #%d</gml:name>\n",obj_count);
    
      vcl_map<int, bmsh3d_face*>::iterator fit;
      for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
        bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
    
        vcl_fprintf (fp, "<boundedBy>");
        vcl_fprintf (fp, "<WallSurface>");
        vcl_fprintf (fp, "<lod4MultiSurface><gml:MultiSurface>\n");
        vcl_fprintf (fp, "<gml:surfaceMember>");
        vcl_fprintf (fp, "<TexturedSurface orientation=\"+\">");
        vcl_fprintf (fp, "<gml:baseSurface>");
        vcl_fprintf (fp, "<gml:Polygon>");
        vcl_fprintf (fp, "<gml:exterior>");
        vcl_fprintf (fp, "<gml:LinearRing>\n");
    
        for (unsigned j=0; j<face->vertices().size(); j++) {
          bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
           double x,y,z;
            lvcs->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
            vcl_fprintf (fp, "<gml:pos srsDimension=\"3\">");
            vcl_fprintf (fp, "%.16f ", x);
            vcl_fprintf (fp, "%.16f ", y);
            vcl_fprintf (fp, "%.16f ", z);
            vcl_fprintf (fp, "</gml:pos>\n");
        }
    
        //Now print the first vertex again to close the polygon
        bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(0);  
        double x,y,z;
        lvcs->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS); 
        vcl_fprintf (fp, "<gml:pos srsDimension=\"3\">");
        vcl_fprintf (fp, "%.16f ", x);
        vcl_fprintf (fp, "%.16f ", y);
        vcl_fprintf (fp, "%.16f ", z);
        vcl_fprintf (fp, "</gml:pos>\n");

        vcl_fprintf (fp, "</gml:LinearRing>");
        vcl_fprintf (fp, "</gml:exterior>");
        vcl_fprintf (fp, "</gml:Polygon>");
        vcl_fprintf (fp, "</gml:baseSurface>");

        // texture
        vcl_fprintf(fp, "<appearance><SimpleTexture>\n");
        vcl_fprintf(fp, "<textureMap>%s</textureMap>\n",vul_file::strip_directory(face->tex_map_uri().c_str()));
        vcl_fprintf(fp, "<textureCoordinates>");
        for (unsigned j=0; j<face->vertices().size(); j++) {
          v = (bmsh3d_vertex*) face->vertices(j);
          //bmsh3d_textured_vertex_3d* tv = (bmsh3d_textured_vertex_3d*) face->vertices(j);  
          vgl_point_2d<double> pt_tex = face->tex_coords(v->id());
          vcl_fprintf (fp, "%.8f ", pt_tex.x());
          vcl_fprintf (fp, "%.8f ", pt_tex.y());
        }

        //Now print the first vertex again to close the polygon
        v = (bmsh3d_vertex*) face->vertices(0);
        vgl_point_2d<double> pt_tex = face->tex_coords(v->id());
        vcl_fprintf (fp, "%.8f ", pt_tex.x());
        vcl_fprintf (fp, "%.8f ", pt_tex.y());

        vcl_fprintf(fp,"</textureCoordinates>\n");
        vcl_fprintf(fp,"<textureType>specific</textureType>\n");
        vcl_fprintf(fp,"</SimpleTexture></appearance>\n");
  
        vcl_fprintf (fp, "</TexturedSurface>\n");     
        vcl_fprintf (fp, "</gml:surfaceMember>");
        vcl_fprintf (fp, "</gml:MultiSurface></lod4MultiSurface>");
        vcl_fprintf (fp, "</WallSurface>");
        vcl_fprintf (fp, "</boundedBy>\n");         

      }
  }
}

void bwm_observable_textured_mesh::save_kml(FILE* fp, int obj_count, bgeo_lvcs* lvcs, 
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
      
    vcl_map<int, bmsh3d_face*>::iterator fit;
    for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
      bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;

        vcl_fprintf(fp, "      <Polygon id=\"building%d_face%d\">\n",obj_count,poly_count);
        vcl_fprintf(fp, "        <extrude>0</extrude>\n");
        vcl_fprintf(fp, "        <tessellate>0</tessellate>\n");
        vcl_fprintf(fp, "        <altitudeMode>relativeToGround</altitudeMode>\n");
        vcl_fprintf(fp, "        <outerBoundaryIs>\n");
        vcl_fprintf(fp, "          <LinearRing>\n");
        vcl_fprintf(fp, "            <coordinates>\n");
    
        for (unsigned j=0; j<face->vertices().size(); j++) {
         bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
          vcl_fprintf (fp, "             %.16f, ", v->pt().x()+x_offset);
         vcl_fprintf (fp, "%.16f, ", v->pt().y()+y_offset);
         vcl_fprintf (fp, "%.16f \n", v->pt().z() - ground_height);
        }

        //Now print the first vertex again to close the polygon
        bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(0);  
        vcl_fprintf (fp, "             %.16f, ", v->pt().x()+x_offset);
        vcl_fprintf (fp, "%.16f, ", v->pt().y()+y_offset);
        vcl_fprintf (fp, "%.16f \n", v->pt().z() - ground_height);

        vcl_fprintf(fp, "            </coordinates>\n");
        vcl_fprintf(fp, "          </LinearRing>\n");
        vcl_fprintf(fp, "        </outerBoundaryIs>\n");
        vcl_fprintf(fp, "      </Polygon>\n");
    }
  }
}


void bwm_observable_textured_mesh::save_kml_collada(FILE* dae_fp, bgeo_lvcs* lvcs,
                                                    vcl_string geometry_id,
                                                    vcl_string geometry_position_id,
                                                    vcl_string geometry_position_array_id,
                                                    vcl_string geometry_uv_id,
                                                    vcl_string geometry_uv_array_id, 
                                                    vcl_string geometry_vertex_id, 
                                                    vcl_string material_name)
{
       
  int nverts = num_vertices();
  int nfaces = num_faces();

   
  vcl_fprintf(dae_fp,"    <geometry id=\"%s\" name=\"%s\">\n",geometry_id.c_str(),geometry_id.c_str());
  vcl_fprintf(dae_fp,"      <mesh>\n");
  vcl_fprintf(dae_fp,"        <source id=\"%s\">\n",geometry_position_id.c_str());
  vcl_fprintf(dae_fp,"        <float_array id=\"%s\" count=\"%d\">\n"          ,geometry_position_array_id.c_str(),nverts*3);
    
  // map vertex ID's to indices.
  vcl_map<int,int> vert_indices;
  
 
  int vert_idx = 0;
  vcl_map<int, bmsh3d_vertex*>::iterator vit;
  for (vit = object_->vertexmap().begin(); vit != object_->vertexmap().end(); vit++, vert_idx++) {
    bmsh3d_vertex* v = (bmsh3d_vertex*)vit->second;
    vert_indices[v->id()] = vert_idx;
    double x,y,z;  
    lvcs->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
    vcl_fprintf(dae_fp,"%f %f %f ",x,y,z);
  }

  vcl_fprintf(dae_fp,"\n        </float_array>\n"); 
  vcl_fprintf(dae_fp,"        <technique_common>\n");
  vcl_fprintf(dae_fp,"          <accessor source=\"#%s\" count=\"%d\" stride=\"3\">\n",geometry_position_array_id.c_str(),nverts);
  vcl_fprintf(dae_fp,"            <param name=\"X\" type=\"float\"/>\n");
  vcl_fprintf(dae_fp,"            <param name=\"Y\" type=\"float\"/>\n");
  vcl_fprintf(dae_fp,"            <param name=\"Z\" type=\"float\"/>\n");
  vcl_fprintf(dae_fp,"          </accessor>\n");
  vcl_fprintf(dae_fp,"        </technique_common>\n");
  vcl_fprintf(dae_fp,"      </source>\n");
  vcl_fprintf(dae_fp,"      <source id=\"%s\">\n",geometry_uv_id.c_str());
      
  // determine total number of corners in mesh
  int ncorners = 0;
  vcl_map<int, bmsh3d_face*>::iterator fit;
  for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
    bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
    ncorners += face->vertices().size();
  }
  
  vcl_fprintf(dae_fp,"        <float_array id=\"%s\" count=\"%d\">\n"          ,geometry_uv_array_id.c_str(),ncorners*2);
  for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
    bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
    for (unsigned j=0; j<face->vertices().size(); j++) {      
      bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
      vgl_point_2d<double> pt = face->tex_coords(v->id());
      vcl_fprintf(dae_fp,"%f %f ",pt.x(),pt.y());
      }
  }
      
  vcl_fprintf(dae_fp,"\n        </float_array>\n");
  vcl_fprintf(dae_fp,"        <technique_common>\n");
  vcl_fprintf(dae_fp,"          <accessor source=\"#%s\" count=\"%d\" stride=\"2\">\n",geometry_uv_array_id.c_str(),ncorners);
  vcl_fprintf(dae_fp,"            <param name=\"S\" type=\"float\"/>\n");
  vcl_fprintf(dae_fp,"            <param name=\"T\" type=\"float\"/>\n");
  vcl_fprintf(dae_fp,"          </accessor>\n");
  vcl_fprintf(dae_fp,"        </technique_common>\n");
  vcl_fprintf(dae_fp,"      </source>\n");

  vcl_fprintf(dae_fp,"      <vertices id=\"%s\">\n",geometry_vertex_id.c_str());
  vcl_fprintf(dae_fp,"        <input semantic=\"POSITION\" source=\"#%s\"/>\n",geometry_position_id.c_str());
  vcl_fprintf(dae_fp,"      </vertices>\n");
  vcl_fprintf(dae_fp,"      <triangles material=\"%s\" count=\"%d\">\n",material_name.c_str(),nfaces);
  vcl_fprintf(dae_fp,"        <input semantic=\"VERTEX\" source=\"#%s\" offset=\"0\"/>\n",geometry_vertex_id.c_str());
  vcl_fprintf(dae_fp,"        <input semantic=\"TEXCOORD\" source=\"#%s\" offset=\"1\" set=\"0\"/>\n",geometry_uv_id.c_str());

      
  int tex_idx = 0;
  vcl_fprintf(dae_fp,"        <p>");
  for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
    bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;

    if (face->vertices().size() != 3) {
      vcl_cerr << "ERROR! only triangle meshes are supported. Face has "<<face->vertices().size()<<" vertices.\n";
    }
        
   for (unsigned j=0; j< 3; j++) {
      bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
      vcl_fprintf(dae_fp,"%d %d ",vert_indices[v->id()],tex_idx++);
   }
  }
}

void bwm_observable_textured_mesh::save_x3d(FILE* fp, bgeo_lvcs* lvcs)
{
  if (!lvcs)
    return;
    
  if (fp) {
    vcl_string texmap_url = tex_map_uri();
  
    vcl_fprintf(fp, "Transform {\n");
    vcl_fprintf(fp, "  children\n");
    vcl_fprintf(fp, "  Shape {\n");
    vcl_fprintf(fp, "    appearance Appearance {\n");
    vcl_fprintf(fp, "      material Material{}\n");
    vcl_fprintf(fp, "      texture ImageTexture {\n");
    vcl_fprintf(fp, "        url \"%s\"\n",texmap_url.c_str());
    vcl_fprintf(fp, "      }\n");
    vcl_fprintf(fp, "    }\n");
    vcl_fprintf(fp, "    geometry IndexedFaceSet {\n");
    vcl_fprintf(fp, "      coord Coordinate {\n");
    vcl_fprintf(fp, "        point [\n");

    // map vertex ID's to indices.
    vcl_map<int,int> vert_indices;

    vcl_map<int, bmsh3d_vertex*>::iterator vit;
    int idx = 0;
    for (vit = object_->vertexmap().begin(); vit != object_->vertexmap().end(); vit++, idx++) {
      bmsh3d_vertex* v = (bmsh3d_vertex*)vit->second;
      vert_indices[v->id()] = idx;
      double x,y,z;
      lvcs->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
      vcl_fprintf(fp,"       %0.8f %0.8f %0.8f,\n",x,y,z);
    }
    vcl_fprintf(fp, "        ]\n");
    vcl_fprintf(fp, "      }\n");
    vcl_fprintf(fp, "      coordIndex[\n");
    
    vcl_map<int, bmsh3d_face*>::iterator fit;
    for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
      bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
      vcl_fprintf(fp, "             ");   
      for (unsigned j=0; j<face->vertices().size(); j++) {
        bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
        vcl_fprintf( fp, "%d ",vert_indices[v->id()]);
      }
      vcl_fprintf(fp, "-1,\n");
    }
    vcl_fprintf(fp, "      ]\n\n");

    vcl_fprintf(fp, "      texCoord TextureCoordinate {\n");
    vcl_fprintf(fp, "        point [\n");

    for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
      bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
      for (unsigned j=0; j<face->vertices().size(); j++) {
        bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(j);
        vgl_point_2d<double> pt = face->tex_coords(v->id());
        vcl_fprintf(fp, "           %0.8f %0.8f,\n",pt.x(),pt.y());
      }
    }
    vcl_fprintf(fp, "        ]\n");
    vcl_fprintf(fp, "      }\n\n");  

    vcl_fprintf(fp, "      texCoordIndex[\n");
    int tex_coord_idx = 0;
    for (fit = object_->facemap().begin(); fit!= object_->facemap().end(); fit++) {
      bmsh3d_textured_face_mc* face = (bmsh3d_textured_face_mc*)fit->second;
      vcl_fprintf(fp, "                ");    
      for (unsigned j=0; j < face->vertices().size(); j++) {
        vcl_fprintf(fp, "%d ",tex_coord_idx++);
      }
      vcl_fprintf(fp, "-1,\n");
    }
  }
}
