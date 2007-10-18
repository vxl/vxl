#include "bwm_texture_map_generator.h"
//#include "bwm_observer.h"
//#include "bwm_observable_mesh.h"

#include <vil1/vil1_image.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_copy.h>
#include <vil/vil_crop.h>
#include <vil/vil_save.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>

#include <bmsh3d/bmsh3d_textured_mesh_mc.h>
#include <bmsh3d/bmsh3d_face_mc.h>
#include <bmsh3d/bmsh3d_textured_face_mc.h>
#include <bmsh3d/bmsh3d_vertex.h>

#include <bgeo/bgeo_lvcs.h>



bool bwm_texture_map_generator::generate_texture_map(bwm_observable_mesh_sptr obj, 
                                                     vcl_string texture_filename, 
                                                     bgeo_lvcs lvcs)
{

  if (observers_.size() == 0) {
    vcl_cerr << "Error: Cannot create texture map, zero observers!\n";
    return false;
  }
  

  // find best observer for each mesh face
  
  //vcl_vector<vil_image_view_base_sptr> img_orig_view;
  vcl_vector<vgl_point_2d<int> > img_sizes;
  vcl_vector<vsol_box_2d> bounding_box;

  for (unsigned obs_idx = 0; obs_idx < observers_.size(); obs_idx++) {
    vgui_image_tableau_sptr img_tab(observers_[obs_idx]->get_image_tableau());
    vgl_point_2d<int> img_size(img_tab->width(),img_tab->height());
    img_sizes.push_back(img_size);
    //img_orig_view.push_back(img_tab->get_image_view());
    vsol_box_2d box;
    bounding_box.push_back(box);
  }
  
 

  bmsh3d_textured_mesh_mc* mesh = new bmsh3d_textured_mesh_mc(obj->get_object());
  // delete old object here?
  obj->set_object(mesh);
  mesh->build_IFS_mesh();
  mesh->IFS_to_MHE();
  mesh->orient_face_normals();
  mesh->build_IFS_mesh();
  //vcl_vector<bmsh3d_textured_face_mc*> tex_faces;

  vcl_map<int, int> best_face_observer_idx;

  // project all mesh vertices with each observer
  vcl_map<int, bmsh3d_vertex*> mesh_verts = mesh->vertexmap();
  vcl_vector<vcl_map<int, vgl_point_2d<double> > > vert_projections;

  for (unsigned obs_idx = 0; obs_idx < observers_.size(); obs_idx++) {
    vcl_map<int, vgl_point_2d<double> > observer_vert_projections;

    vcl_map<int, bmsh3d_vertex*>::iterator vit;
    for (vit = mesh_verts.begin(); vit!=mesh_verts.end(); vit++) {
      bmsh3d_vertex* vert = (bmsh3d_vertex*)vit->second;
      vgl_point_3d<double> world_pt = vert->pt();
      vgl_point_2d<double> image_pt;
      observers_[obs_idx]->proj_point(world_pt,image_pt);

      observer_vert_projections[vert->id()] = image_pt;
    } //for each vertex
    vert_projections.push_back(observer_vert_projections);
  }// for each observer

  // find best observer for each face
  vcl_map<int, bmsh3d_face*>::iterator fit;
  for (fit = mesh->facemap().begin(); fit != mesh->facemap().end(); fit++) {
    bmsh3d_textured_face_mc* tex_face = (bmsh3d_textured_face_mc*)fit->second;
    tex_face->set_tex_map_uri(texture_filename);

    vcl_vector<bmsh3d_vertex*> face_vertices = tex_face->vertices();

    int best_observer_idx = -1;
    double best_observer_score = 0.0;

    for (unsigned obs_idx = 0; obs_idx < observers_.size(); obs_idx++) {
      
      // test each vertex for visibility, assume face visibility is AND of all tests
      bool is_visible = true;
      for (unsigned j=0; j < face_vertices.size(); j++){
        vgl_point_2d<double> vert = vert_projections[obs_idx][face_vertices[j]->id()];
        // make sure vertex projection is within image
        if ( (vert.x() < 0) || (vert.x() >= img_sizes[obs_idx].x()) ||
             (vert.y() < 0) || (vert.y() >= img_sizes[obs_idx].y()) ) {
           is_visible = false;
           break;
        }
      }
      //
      // TODO: check if face is occluded
      //
      if (is_visible) {
        // compute normal in lvcs coordinates!
        vgl_vector_3d<double> face_normal = compute_face_normal_lvcs(tex_face,lvcs);

        vgl_vector_3d<double> face_normal_global = compute_normal_ifs(face_vertices);
        vgl_vector_3d<double> face_normal_phe = tex_face->compute_normal();

        //vcl_cout << "face normal local = " << face_normal << vcl_endl;
        //vcl_cout << "face normal phe =    " << face_normal_phe << vcl_endl;
        //vcl_cout << "face normal global = "<< face_normal_global << vcl_endl << vcl_endl;


        face_normal = face_normal / face_normal.length(); // not gauranteed to be normalized
        vgl_vector_3d<double> camera_direction = observers_[obs_idx]->camera_direction();//lvcs);

        // just use angle for now, maybe incorporate distance to camera later?
        double score = dot_product(face_normal,camera_direction);

        if (score > best_observer_score) {
          best_observer_score = score;
          best_observer_idx = obs_idx;
        }
      }
    }
    
    if (best_observer_score > 0) {
      for (unsigned j=0; j < face_vertices.size(); j++){
        bmsh3d_vertex* face_vert = (bmsh3d_vertex*)face_vertices[j];
        vgl_point_2d<double> vert_proj = vert_projections[best_observer_idx][face_vert->id()];
        // update bounding box of mesh
        bounding_box[best_observer_idx].add_point(vert_proj.x(),vert_proj.y());
        best_face_observer_idx[tex_face->id()] = best_observer_idx;
      }
    }
    else {
      // use -1 to indicate face not visible from any observer
      best_face_observer_idx[tex_face->id()] = -1;
    }

  } // for each face

  // determine crop region for each observers image
  vcl_vector<vgl_point_2d<int> > crop_points;
  vcl_vector<vgl_point_2d<int> > crop_sizes;

  // cropped images will be stacked horizontally, so
  // calculate max height for image and x offsets for each region
  int tex_width, tex_height = 0;
  vcl_vector<int> x_offsets;
  x_offsets.push_back(0); // for beginning of first image

  for (unsigned obs_idx = 0; obs_idx < observers_.size(); obs_idx++) {
    if (!bounding_box[obs_idx].empty()) {
      int min_x = vcl_floor(bounding_box[obs_idx].get_min_x());
       int max_x = vcl_ceil(bounding_box[obs_idx].get_max_x());
       int min_y = vcl_floor(bounding_box[obs_idx].get_min_y());
       int max_y = vcl_ceil(bounding_box[obs_idx].get_max_y());
       vgl_point_2d<int> crop_point(min_x,min_y);
       vgl_point_2d<int> crop_size(max_x - min_x + 1, max_y - min_y + 1);

       if (crop_size.y() > tex_height) {
         tex_height = crop_size.y();
       }
       x_offsets.push_back(x_offsets.back() + crop_size.x());

       crop_points.push_back(crop_point);
       crop_sizes.push_back(crop_size);
    }
    else {
      x_offsets.push_back(x_offsets.back());

      vgl_point_2d<int> zero(0,0);
      crop_points.push_back(zero);
      crop_sizes.push_back(zero);
    }
  }
  tex_width = x_offsets.back();

  // now loop through each face's tex coords and normalize 
  for (fit = mesh->facemap().begin(); fit != mesh->facemap().end(); fit++) {

    bmsh3d_textured_face_mc* tex_face = (bmsh3d_textured_face_mc*)fit->second;
    vcl_vector<bmsh3d_vertex*> face_vertices = tex_face->vertices();
    int best_obs = best_face_observer_idx[tex_face->id()];

    if (best_obs >= 0) {
      for (unsigned v=0; v<face_vertices.size(); v++) {
        vgl_point_2d<double> vert_proj = vert_projections[best_obs][face_vertices[v]->id()];
        double x = (vert_proj.x() - crop_points[best_obs].x() + x_offsets[best_obs]) / (double)tex_width;
        double y = 1 - ((vert_proj.y() - crop_points[best_obs].y()) / (double)tex_height);

        vgl_point_2d<double> vert_proj_norm(x,y);

        // add coord to texture coordinate list
        tex_face->set_tex_coord(face_vertices[v]->id(), vert_proj_norm);
      }
    }
    else {
      // set all texture coordinates for this face to (0,0)
      for (unsigned v=0; v<face_vertices.size(); v++) {
        vgl_point_2d<double> vert_proj(0.0,0.0);
        tex_face->set_tex_coord(face_vertices[v]->id(), vert_proj);
      }
    }
  }

  // crop out bounding box from original images and copy to the texture image
  //vil_image_view_base_sptr tex_map_view = vil_crop(img_orig,bounding_box.get_min_x(),bounding_box.width(),bounding_box.get_min_y(),bounding_box.height());
 
  vil_image_view<vxl_byte> tex_map_view(tex_width,tex_height,3);
  for (unsigned obs_idx = 0; obs_idx < observers_.size(); obs_idx++) {
    if ( (crop_sizes[obs_idx].x() > 0) && (crop_sizes[obs_idx].y() > 0) ) {
      vgui_image_tableau_sptr img_tab = observers_[obs_idx]->get_image_tableau();
      vil_image_resource_sptr img_orig_res = img_tab->get_image_resource();


      if (img_orig_res->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
        vcl_cerr << "texture_map generator: unsupported image type "<<img_orig_res->pixel_format()<<vcl_endl;
        continue;
      }
      vil_image_resource_sptr img_orig_cropped = vil_crop(img_orig_res,
                                                      crop_points[obs_idx].x(),crop_sizes[obs_idx].x(),
                                                      crop_points[obs_idx].y(),crop_sizes[obs_idx].y());


      vil_image_view<vxl_byte> cropped_view = img_orig_cropped->get_view();
      vcl_cout << "cropped_view nplanes = "<<cropped_view.nplanes()<<vcl_endl;
      vcl_cout << "tex_map_view nplanes = "<<tex_map_view.nplanes()<<vcl_endl;
      vil_copy_to_window(cropped_view,tex_map_view,x_offsets[obs_idx],0);
    }
  }
  vil_save(tex_map_view,texture_filename.c_str());
  
  return true;
}

vgl_vector_3d<double> bwm_texture_map_generator::compute_face_normal_lvcs(bmsh3d_face* face, bgeo_lvcs lvcs) {

  vgl_vector_3d<double> normal;

  vcl_vector<bmsh3d_vertex*> verts = face->vertices();

  double cx,cy,cz;
  vgl_point_3d<double> centroid = face->compute_center_pt();
  lvcs.global_to_local(centroid.x(), centroid.y(), centroid.z(),bgeo_lvcs::wgs84,cx,cy,cz,bgeo_lvcs::DEG,bgeo_lvcs::METERS);

  double vx,vy,vz;
  bmsh3d_vertex* last_v = (bmsh3d_vertex*) verts.back();
  lvcs.global_to_local(last_v->pt().x(),last_v->pt().y(),last_v->pt().z(),bgeo_lvcs::wgs84,vx,vy,vz,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  vgl_vector_3d<double> a(vx-cx, vy-cy, vz-cz);

  for (unsigned i=0; i < verts.size(); i++) {
    bmsh3d_vertex* v = (bmsh3d_vertex*) verts[i];
    lvcs.global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,vx,vy,vz,bgeo_lvcs::DEG,bgeo_lvcs::METERS);

    vgl_vector_3d<double> b(vx-cx, vy-cy, vz-cz);
    vgl_vector_3d<double> n = cross_product (a, b);

    normal += n;

    a = b;
  }

  return normal;
}
