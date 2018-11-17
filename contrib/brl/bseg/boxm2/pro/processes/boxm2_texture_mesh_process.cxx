// This is brl/bseg/boxm2/pro/processes/boxm2_texture_mesh_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting a texture mapped mesh of a scene
//
// \author Vishal Jain
// \date Mar 15, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_random.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>

//vil includes
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_gauss_filter.h>

//vgl
#include <vgl/vgl_distance.h>
#include <vgl/vgl_triangle_scan_iterator.h>
#include <bvgl/bvgl_triangle_interpolation_iterator.h>

//vpgl camera stuff
#include <vpgl/vpgl_perspective_camera.h>
// for loading cameras from directory
#include <bpgl/bpgl_camera_utils.h>

//det and imesh includes
#include <sdet/sdet_image_mesh.h>
#include <sdet/sdet_image_mesh_params.h>
#include <imesh/imesh_fileio.h>
#include <imesh/algo/imesh_render.h>

namespace boxm2_texture_mesh_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;

  //struct for passing 3d triangles (couldn't find a 3d triangle in VGL...)
  struct triangle_3d {
    vgl_point_3d<double> points[3];
    unsigned face_id;
  };

  //aux data structures (norm images, vis images)
  std::vector<vil_image_view<int>* > vis_images_;
  std::vector<vil_image_view<float>* > normx_, normy_, normz_;

  //main helper method - fills out a map of imesh_mesh's, each texture mapped with a separate image
  void boxm2_texture_mesh_from_imgs(const std::string& im_dir,
                                    const std::string& cam_dir,
                                    const std::string& out_dir,
                                    imesh_mesh& in_mesh,
                                    std::map<std::string, imesh_mesh>& meshes);

  //populates a vector of visibility images (by face id int)
  void boxm2_visible_faces( std::vector<vpgl_perspective_camera<double>* >& cameras,
                            imesh_mesh& in_mesh,  unsigned int ni, unsigned int nj);

  //stores a norm image (3d face norm) for each of the texture images
  void compute_norm_images( std::vector<vpgl_perspective_camera<double>* >& cameras,
                            imesh_mesh& in_mesh,unsigned int ni, unsigned int nj);

  //smooths the vector of norm images above
  void smooth_norm_images(double sigma);

  //checks if a triangle in UV space is visible.  us and vs are double buffers of length 3
  bool face_is_visible( vpgl_perspective_camera<double>* cam,
                        vil_image_view<int>* vis_img,
                        triangle_3d& world_tri);

  //matches textures
  void boxm2_match_textures(std::vector<vpgl_perspective_camera<double>* >& cameras,
                            std::vector<std::string>& imfiles,
                            imesh_mesh& in_mesh,
                            std::map<std::string, std::vector<unsigned> >& app_faces,
                            std::map<std::string, vpgl_perspective_camera<double>* >& texture_cams);

  //returns a list of visible triangles given a camera,
  //visibility image, and world coordinate 3d triangle
  std::vector<triangle_3d> get_visible_triangles(vpgl_perspective_camera<double>* cam,
                                                vil_image_view<int>* vis_img,
                                                triangle_3d& world_tri);

  //given revised lists of visible images, norms, etc, return the index of the best view
  int get_best_view(std::vector<vpgl_perspective_camera<double>* >& cams,
                    std::vector<vil_image_view<int>* >& vis_images,
                    std::vector<vil_image_view<float>* >& normx,
                    std::vector<vil_image_view<float>* >& normy,
                    std::vector<vil_image_view<float>* >& normz,
                    triangle_3d& world_tri );

  vgl_vector_3d<double> calc_smooth_norm( vpgl_perspective_camera<double>* cam,
                                          vil_image_view<int>* vis_img,
                                          vil_image_view<float>* normx,
                                          vil_image_view<float>* normy,
                                          vil_image_view<float>* normz,
                                          triangle_3d& world_tri);
}

bool boxm2_texture_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_texture_mesh_process_globals;

  //process takes 2 inputs
  int i=0;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[i++] = "imesh_mesh_sptr";  //depth image
  input_types_[i++] = "vcl_string";       //directory of scene images
  input_types_[i++] = "vcl_string";       //directory of corresponding cams
  input_types_[i++] = "vcl_string";       //output dir of saved mesh

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  //output_types_[0] = "boxm2_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_texture_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_texture_mesh_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  unsigned argIdx = 0;
  imesh_mesh_sptr mesh = pro.get_input<imesh_mesh_sptr>(argIdx++);
  std::string img_dir = pro.get_input<std::string>(argIdx++);
  std::string cam_dir = pro.get_input<std::string>(argIdx++);
  std::string out_dir = pro.get_input<std::string>(argIdx++);

  //create the mesh directory
  if (out_dir != "") {
    if (!vul_file::make_directory_path(out_dir.c_str())) {
      std::cout<<"Couldn't make directory path "<<out_dir<<std::endl;
      return false;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  //Texture map the mesh
  //////////////////////////////////////////////////////////////////////////////
  vul_timer t;
  std::map<std::string, imesh_mesh> meshes;
  boxm2_texture_mesh_from_imgs(img_dir, cam_dir, out_dir, *mesh, meshes);

  ////////////////////////////////////////////////////////////////////////////////
  //// Write out in VRML format
  ////////////////////////////////////////////////////////////////////////////////
  //output file and stream
  std::string vrfile = out_dir + "/vrmesh.wrl";
  std::ofstream os(vrfile.c_str());

  //write each submesh into one file
  std::map<std::string, imesh_mesh>::iterator subMesh;
  for (subMesh = meshes.begin(); subMesh != meshes.end(); ++subMesh)
  {
    imesh_mesh& sMesh = subMesh->second;
    std::cout<<"Writing sub mesh: "<<sMesh.tex_source()<<" has "<<sMesh.num_faces()<<" faces"<<std::endl;
    imesh_write_vrml(os, sMesh);
  }
  os.close();
  std::cout<<"Texture Mapping Mesh Time: "<<t.all()<<"ms"<<std::endl;

  return true;
}


//: Given a directory of images, dir cams, an input mesh, this function creates a map of textured meshes
// (imesh doesn't ostensibly handle meshes from multiple textures)
void boxm2_texture_mesh_process_globals::boxm2_texture_mesh_from_imgs(const std::string& im_dir,
                                                                      const std::string& cam_dir,
                                                                      const std::string& out_dir,
                                                                      imesh_mesh& in_mesh,
                                                                      std::map<std::string, imesh_mesh>& meshes)
{
  ////////////////////////////////////////////////////////////////////////////////
  // BEGIN TEXTURE MAPPING
  // Gather cameras and iamges that will contribute to the texture
  ////////////////////////////////////////////////////////////////////////////////
  std::vector<std::string> allims = boxm2_util::images_from_directory(im_dir);

  //create blank texturemap image
  vil_image_view_base_sptr first_im = boxm2_util::prepare_input_image(allims[0]);
  auto* imptr = (vil_image_view<vil_rgba<vxl_byte> >*) first_im.ptr();
  vil_rgba<vxl_byte> mean = boxm2_util::mean_pixel(*imptr);
  vil_image_view<vil_rgba<vxl_byte> > def(4,4);
  def.fill( mean );
  vil_save(def, (out_dir + "/empty.png").c_str());

  //chop paths to make mesh portable
  for (auto & allim : allims) {
    std::string full_path = allim;
    std::string rel_path = vul_file::basename(full_path);
    allim = rel_path;
  }
  std::vector<vpgl_perspective_camera<double>* > allcams = bpgl_camera_utils::cameras_from_directory(cam_dir);
  if (allims.size() != allcams.size()) {
    std::cout<<"Texture images are not 1 to 1 with cameras:: dirs "<<im_dir<<" and "<<cam_dir<<std::endl;
    return;
  }

  //choose a few random images
  std::vector<std::string> imfiles;
  std::vector<vpgl_perspective_camera<double>* > cameras;

  //unsigned int handpicked[] = {0,1,13,25,33,40,51,64,73,82,96,105,109,114,125,133,140,147,153,164,175};
  //int handpicked[] = { 0,1,13,25,33,51,73,96,109,114,133,147,164};
  //int handpicked[] = {0,12,18};
  //int handpicked[] = {0, 1, 26, 33, 56, 96, 114, 133 };
  //int handpicked[] = {0, 8, 16, 23 };
  for (unsigned int i=0; i<allims.size(); ++i) {
      imfiles.push_back(allims[i]);
      cameras.push_back(allcams[i]);
      std::cout<<"added image: "<<imfiles[i]<<std::endl;
  }
#if 0
  vnl_random rand(9667566);
  for (int i=0; i<5; ++i) {
    unsigned filenum = rand.lrand32(1, allims.size()-1);
    imfiles.push_back(allims[filenum]);
    cameras.push_back(allcams[filenum]);
  }
#endif

  ////////////////////////////////////////////////////////////////////////////////
  // make sure mesh has computed vertex normals
  ////////////////////////////////////////////////////////////////////////////////
  in_mesh.compute_face_normals();

  ////////////////////////////////////////////////////////////////////////////////
  // Render Visibility Images
  ////////////////////////////////////////////////////////////////////////////////
  std::cout<<"calculating visibility images (for each textured image)"<<std::endl;
  boxm2_visible_faces(cameras, in_mesh,first_im->ni(),first_im->nj());

  ////////////////////////////////////////////////////////////////////////////////
  // Render norm images (and then smooth them)
  ////////////////////////////////////////////////////////////////////////////////
  std::cout<<"calculating norm images (for each texture image)"<<std::endl;
  compute_norm_images(cameras, in_mesh,first_im->ni(),first_im->nj());
  smooth_norm_images(30.0);

  ////////////////////////////////////////////////////////////////////////////////
  // match each face to best image, store a list for each texture image
  ////////////////////////////////////////////////////////////////////////////////
  std::cout<<"Populating faces for each texture"<<std::endl;
  std::map<std::string, std::vector<unsigned> > app_faces; //image_name to face_list
  std::map<std::string, vpgl_perspective_camera<double>* > texture_cams;
  boxm2_match_textures(cameras, imfiles, in_mesh, app_faces, texture_cams);

  ////////////////////////////////////////////////////////////////////////////////
  // For each image/appearance:
  //   - create a vert list
  //   - create a face list
  //   - create a sub mesh that is textured
  ////////////////////////////////////////////////////////////////////////////////
  auto& in_faces = (imesh_face_array&) in_mesh.faces();
  imesh_vertex_array<3>& in_verts = in_mesh.vertices<3>();
  //for each appearance (texture image), create an imesh_mesh (subMesh);
  std::cout<<"Creating Sub Meshes for each texture"<<std::endl;
  std::map<std::string, std::vector<unsigned> >::iterator apps;
  for (apps = app_faces.begin(); apps != app_faces.end(); ++apps)
  {
    //for each appearance, we're creating a whole new mesh
    // first create the face list
    auto* flist = new imesh_face_array();

    //now create the vertex list
    auto* verts3 = new imesh_vertex_array<3>();

    //get faces list corresponding to this texture
    std::vector<unsigned>& face_list = apps->second;
    for (unsigned int i=0; i<face_list.size(); ++i) {
      //get old face
      unsigned old_fIdx = face_list[i];

      //old face vertices
      unsigned v1 = in_faces[old_fIdx][0];
      unsigned v2 = in_faces[old_fIdx][1];
      unsigned v3 = in_faces[old_fIdx][2];

      //push these vertices onto the new vert list
      verts3->push_back( imesh_vertex<3>(in_verts[v1][0], in_verts[v1][1], in_verts[v1][2]) );
      verts3->push_back( imesh_vertex<3>(in_verts[v2][0], in_verts[v2][1], in_verts[v2][2]) );
      verts3->push_back( imesh_vertex<3>(in_verts[v3][0], in_verts[v3][1], in_verts[v3][2]) );

      imesh_tri tri(3*i, 3*i+1, 3*i+2);
      flist->push_back(tri);
    }

    //create the submesh using the auto ptrs
    std::unique_ptr<imesh_vertex_array_base> v3(verts3);
    std::unique_ptr<imesh_face_array_base> f3(flist);
    imesh_mesh subMesh(std::move(v3), std::move(f3));

    std::cout<<"Setting tex source: "<<apps->first<<std::endl;
    meshes[apps->first] = subMesh;
    meshes[apps->first].set_tex_source(apps->first);
  }

  //////////////////////////////////////////////////////////////////////////////
  //For each mesh, map each vertex
  //////////////////////////////////////////////////////////////////////////////
  std::cout<<"Mapping sub meshes for each texture"<<std::endl;
  std::map<std::string, imesh_mesh>::iterator subMesh;
  auto txCam = texture_cams.begin();
  for (subMesh = meshes.begin(); subMesh != meshes.end(); ++subMesh, ++txCam)
  {
    imesh_mesh& mesh = subMesh->second;
    imesh_vertex_array<3>& verts = mesh.vertices<3>();
    unsigned nverts = mesh.num_verts();

    //texture map the non empty appearances
    if (txCam->first != "empty.png")
    {
      std::vector<vgl_point_2d<double> > tex_coords(nverts, vgl_point_2d<double>(0.0,0.0));
      for (unsigned iv = 0; iv<nverts; ++iv)
      {
        //find camera corresponding to this texture
        vpgl_perspective_camera<double>* closest = txCam->second;
        vgl_point_2d<double> principal_point = closest->get_calibration().principal_point();
        double ni = principal_point.x()*2.0;
        double nj = principal_point.y()*2.0;

        //project the vertex onto the camera, store the texture coordinate
        double x = verts[iv][0];
        double y = verts[iv][1];
        double z = verts[iv][2];
        double u,v;
        closest->project(x, y, z, u, v);

        //flip v about the y axis
        v=nj-v;

        //store the tex_coordinate
        vgl_point_2d<double> uv(u/ni,v/nj);
        tex_coords[iv] = uv;
      }
      mesh.set_tex_coords(tex_coords);
    }
    else {
      //otherwise put in default texture (grey)
      std::vector<vgl_point_2d<double> > tex_coords(nverts, vgl_point_2d<double>(0.0,0.0));
      for (unsigned iv = 0; iv<nverts; ++iv) {
        tex_coords[iv] = vgl_point_2d<double>(.5, .5);
      }
      mesh.set_tex_coords(tex_coords);
    }
  }
}


//image_name to face_list
void boxm2_texture_mesh_process_globals::boxm2_match_textures(std::vector<vpgl_perspective_camera<double>* >& cameras,
                                                              std::vector<std::string>& imfiles,
                                                              imesh_mesh& in_mesh,
                                                              std::map<std::string, std::vector<unsigned> >& app_faces,
                                                              std::map<std::string, vpgl_perspective_camera<double>* >& texture_cams)
{
  //grab faces and vertices from the mesh
  auto& in_faces = (imesh_face_array&) in_mesh.faces();
  unsigned nfaces = in_mesh.num_faces();
  imesh_vertex_array<3>& in_verts = in_mesh.vertices<3>();

  //for each face, determine which view is best
  for (unsigned iface = 0; iface<nfaces; ++iface)
  {
    //make a triangle_3d out of this face
    triangle_3d world_tri;
    world_tri.face_id = iface;
    for (int i=0; i<3; ++i) {
      unsigned vertexId = in_faces[iface][i];
      double x = in_verts[vertexId][0];
      double y = in_verts[vertexId][1];
      double z = in_verts[vertexId][2];
      world_tri.points[i] = vgl_point_3d<double>(x,y,z);
    }

    //create list of cameras from which you can see this face
    std::vector<std::string> visible_imfiles;
    std::vector<vpgl_perspective_camera<double>* > visible_views;
    std::vector<vil_image_view<int>* > vis_images;
    std::vector<vil_image_view<float>* > normx, normy, normz;
    for (unsigned int i=0; i<vis_images_.size(); ++i) {
      if ( face_is_visible( cameras[i], vis_images_[i], world_tri) ) {
        visible_imfiles.push_back(imfiles[i]);
        visible_views.push_back(cameras[i]);
        vis_images.push_back(vis_images_[i]);
        normx.push_back(normx_[i]);
        normy.push_back(normy_[i]);
        normz.push_back(normz_[i]);
      }
    }

    //now compare the normal to each of the visible view cameras
#ifdef DEBUG
    vgl_vector_3d<double>& normal = in_faces.normal(iface);
    std::cout<<"Face "<<iface<<" normal: "<<normal<<std::endl;
#endif

    //find camera with the closest look vector to this normal
    //int closeIdx = boxm2_util::find_nearest_cam(normal, visible_views);
    int closeIdx = get_best_view(visible_views, vis_images, normx, normy, normz, world_tri);
    vpgl_perspective_camera<double>* closest = nullptr;
    std::string im_name = "empty.png";
    if (closeIdx >= 0) {
      closest = visible_views[closeIdx];
      im_name = visible_imfiles[closeIdx];
    }

    //grab appropriate face list (create it if it's not there)
    auto iter = app_faces.find(im_name);
    if ( iter == app_faces.end() ) {
      std::cout<<"boxm2_match_textures:: Adding image "<<im_name<<" to texture list"<<std::endl;
      std::vector<unsigned> faceList;
      faceList.push_back(iface);
      app_faces[im_name] = faceList;

      //keep track of the camera
      texture_cams[im_name] = closest;
    }
    else {
      app_faces[im_name].push_back(iface);
    }
  }

#if 0
  ////////////////////////////////////////////////////////////////////////////////
  // Now there is a set of faces without textures,
  // for each face
  //    for each image
  //      - find visible portions of these faces, record area and angle from dist
  //    find angle < 60 degrees AND largest area patch
  //    cut the face such that the patch is exposed (patch may be some arbitrary polygon)
  //    triangulate the n-gon into n triangles
  //    add the *new* points to the mesh, as well as the new faces
  //    remember to not leave the old face in the mesh...
  ////////////////////////////////////////////////////////////////////////////////
  for (unsigned iface = 0; iface<nfaces; ++iface)
  {
    //whole first chunk figures out which set of triangles this face should be broken into
    double max_area = 0.0;
    double max_img = -1;
    std::vector<triangle_3d> final_triangles;
    for (int imIdx=0; imIdx<vis_images.size(); ++imIdx) {
      //if this angle is too large, just pass over it
      double midAngle = angle(in_faces.normal(iface), -1*cameras[imIdx]->principal_axis()); // return acos(cos_angle(a,b));
      if (midAngle > vnl_math::pi/3) continue;

      //find visible portion of iface on this image, record area (make a 3d tri out of it first)
      triangle_3d world_tri;
      world_tri.face_id = iface;
      for (int i=0; i<3; ++i) {
        unsigned vertexId = in_faces[iface][i];
        double x = in_verts[vertexId][0];
        double y = in_verts[vertexId][1];
        double z = in_verts[vertexId][2];
        world_tri.points[i] = vgl_point_3d<double>(x,y,z);
      }
      std::vector<triangle_3d> vis_tris = get_visible_triangles(cameras[imIdx], vis_images[imIdx], world_tri);

      //get the total area of the triangles
      double totalArea = 0;
      for (int tri_i = 0; tri_i < vis_tris.size(); ++tri_i) {
        totalArea+= vgl_triangle_3d_area(vis_tris[tri_i].points[0],
                                         vis_tris[tri_i].points[1],
                                         vis_tris[tri_i].points[2]);
      }

      //store it if it's the biggest patch so far
      if (totalArea > max_area) {
        max_area = totalArea;
        max_img = imIdx;
        final_triangles = vis_tris;
      }
    }

    //now that you have the visible triangles that will be mapped in your mesh...
    // 1. add the new vertices
    // 2. add the new faces

    // 3. add the new faces to the correct texture list
    std::string im_name =
    app_faces[im_name].push_back(iface);
  }

  //Be wary of this interpolation iterator - didn't match up with the imesh one
  bvgl_triangle_interpolation_iterator(double *verts_x, double *verts_y, T *values, unsigned int v0 = 0, unsigned int v1 = 1, unsigned int v2 = 2);

  // Create a new face list with the faces split by visibility
  imesh_regular_face_array<3>* newFaces = new imesh_regular_face_array<3>();
#endif
}

//: Compares each visible camera with the average normal from each norm image, chooses the one that most closely matches
int boxm2_texture_mesh_process_globals::get_best_view(std::vector<vpgl_perspective_camera<double>* >& cams,
                                                      std::vector<vil_image_view<int>* >& vis_images,
                                                      std::vector<vil_image_view<float>* >& normx,
                                                      std::vector<vil_image_view<float>* >& normy,
                                                      std::vector<vil_image_view<float>* >& normz,
                                                      triangle_3d& world_tri)
{
  if (cams.empty()) {
    return -1;
  }

  //find minimal dot product amongst cams/images
  double minAngle = 10e20;
  int minCam = -1;
  for (unsigned int i=0; i<cams.size(); ++i) {
    //get the smooth norm corresponding to this camera view
    vgl_vector_3d<double> normal = calc_smooth_norm( cams[i], vis_images[i], normx[i], normy[i], normz[i], world_tri);
    //std::cout<<"   face "<<world_tri.face_id<<" image "<<i<<" normal: "<<normal<<std::endl;

    double dotProd = dot_product( normal, -1*cams[i]->principal_axis());
    double ang = std::acos(dotProd);
#ifdef DEBUG
    if ( std::fabs(normal.z()) > .8 ) {
      std::cout<<"Face normal: "<<normal<<"  principal axis: "<<cams[i]->principal_axis()<<'\n'
              <<" and angle: " <<ang * vnl_math::deg_per_rad<<std::endl;
    }
#endif
    if (ang < minAngle && ang < vnl_math::pi/2.0) {
      minAngle = ang;
      minCam = i;
    }
  }

  //return the min cam
  return minCam;
}

vgl_vector_3d<double>
boxm2_texture_mesh_process_globals::calc_smooth_norm( vpgl_perspective_camera<double>* cam,
                                                      vil_image_view<int>* vis_img,
                                                      vil_image_view<float>* normx,
                                                      vil_image_view<float>* normy,
                                                      vil_image_view<float>* normz,
                                                      triangle_3d& world_tri)
{
  //project triangle
  double us[3], vs[3];
  for (int vIdx=0; vIdx<3; ++vIdx) {
    //project these verts into UV
    double x = world_tri.points[vIdx].x();
    double y = world_tri.points[vIdx].y();
    double z = world_tri.points[vIdx].z();
    double u,v;
    cam->project(x, y, z, u, v);
    us[vIdx] = u;
    vs[vIdx] = v;
  }

  //now create a polygon, and find the integer image coordinates (U,V) that this polygon covers
  int ni = vis_img->ni();
  int nj = vis_img->nj();
  double nx=0.0, ny=0.0, nz=0.0; //initialize normal
  int numNormals = 0;

  vgl_triangle_scan_iterator<double> tsi;
  tsi.a.x = us[0];  tsi.a.y = vs[0];
  tsi.b.x = us[1];  tsi.b.y = vs[1];
  tsi.c.x = us[2];  tsi.c.y = vs[2];
  for (tsi.reset(); tsi.next(); ) {
    int y = tsi.scany();
    if (y<0 || y>=nj) continue;
    int min_x = tsi.startx();
    int max_x = tsi.endx();
    if (min_x >= ni || max_x < 0)
      continue;
    if (min_x < 0) min_x = 0;
    if (max_x >= ni) max_x = ni-1;
    for (int x = min_x; x <= max_x; ++x) {
      if ( (*vis_img)(x,y) == int(world_tri.face_id) ) {
        nx += (*normx)(x,y);
        ny += (*normy)(x,y);
        nz += (*normz)(x,y);
        numNormals++;
      }
    }
  }
  return {nx/numNormals, ny/numNormals, nz/numNormals};
}


//:
// returns a list of visible triangles given a camera,
// visibility image, and world coordinate 3d triangle
std::vector<boxm2_texture_mesh_process_globals::triangle_3d>
boxm2_texture_mesh_process_globals::get_visible_triangles(vpgl_perspective_camera<double>* /*cam*/,
                                                          vil_image_view<int>* /*vis_img*/,
                                                          triangle_3d& /*world_tri*/)
{
  std::cerr << "TODO: boxm2_texture_mesh_process_globals::get_visible_triangles is not yet implemented\n";
  return std::vector<triangle_3d>();
}


//:
// given a camera, and an image with ID faces, this method returns true if the entire
// triangle (id face_id) is unoccluded from this point of view
bool boxm2_texture_mesh_process_globals::face_is_visible( vpgl_perspective_camera<double>* cam,
                                                          vil_image_view<int>* vis_img,
                                                          triangle_3d& world_tri)
{
  //project triangle
  double us[3], vs[3];
  for (int vIdx=0; vIdx<3; ++vIdx) {
    //project these verts into UV
    double x = world_tri.points[vIdx].x();
    double y = world_tri.points[vIdx].y();
    double z = world_tri.points[vIdx].z();
    double u,v;
    cam->project(x, y, z, u, v);
    us[vIdx] = u;
    vs[vIdx] = v;
  }

  //now create a polygon, and find the integer image coordinates (U,V) that this polygon covers
  int ni = vis_img->ni();
  int nj = vis_img->nj();
  unsigned int numPixels = 0;
  unsigned int numMatches = 0;

  vgl_triangle_scan_iterator<double> tsi;
  tsi.a.x = us[0];  tsi.a.y = vs[0];
  tsi.b.x = us[1];  tsi.b.y = vs[1];
  tsi.c.x = us[2];  tsi.c.y = vs[2];
  for (tsi.reset(); tsi.next(); ) {
    int y = tsi.scany();
    if (y<0 || y>=nj) continue;
    int min_x = tsi.startx();
    int max_x = tsi.endx();
    if (min_x >= ni || max_x < 0)
      continue;
    if (min_x < 0) min_x = 0;
    if (max_x >= ni) max_x = ni-1;
    for (int x = min_x; x <= max_x; ++x) {
      ++numPixels;
      if ( (*vis_img)(x,y) == int(world_tri.face_id) )
        ++numMatches;
    }
  }

  //if the majority match, it's visible
  if ( (double) numMatches / (double) numPixels > .99)
    return true;
  else
    return false;
}

//: Constructs vector of visibility images - images that identify which triangle is visible at which pixel.
// Function is more or less complete - not much more to it.
void boxm2_texture_mesh_process_globals::boxm2_visible_faces( std::vector<vpgl_perspective_camera<double>* >& cameras,
                                                              imesh_mesh& in_mesh, unsigned int ni=1024, unsigned int nj=768)
{
  auto& in_faces = (imesh_face_array&) in_mesh.faces();

  unsigned nfaces = in_mesh.num_faces();
  imesh_vertex_array<3>& in_verts = in_mesh.vertices<3>();

  // iterate over each camera, creating a visibility image for each
  for (auto pcam : cameras)
  {
    //// get the principal point of the cam for image size
    //std::cout<<(*pcam);
    //vgl_point_2d<double> principal_point = pcam->get_calibration().principal_point();
    //unsigned ni = (unsigned) (principal_point.x()*2.0);
    //unsigned nj = (unsigned) (principal_point.y()*2.0);

    // render the face_id/distance image
    vil_image_view<double> depth_im(ni, nj);
    auto*   face_im = new vil_image_view<int>(ni, nj);
    depth_im.fill(10e100);  //Initial depth is huge,
    face_im->fill(-1); //initial face id is -1
    for (unsigned iface = 0; iface<nfaces; ++iface)
    {
      // get the vertices from the face, project into UVs
      double us[3], vs[3], dists[3];

      for (int vIdx=0; vIdx<3; ++vIdx) {
        unsigned vertIdx = in_faces[iface][vIdx];
        // project these verts into UV
        double x = in_verts[vertIdx][0], y = in_verts[vertIdx][1], z = in_verts[vertIdx][2];
        double u,v;
        pcam->project(x, y, z, u, v);
        us[vIdx] = u;
        vs[vIdx] = v;
        // keep track of distance to each vertex
        dists[vIdx] = vgl_distance(vgl_point_3d<double>(x,y,z), pcam->get_camera_center());
      }

      // render the triangle label onto the label image, using the depth image
      vgl_point_3d<double> v1(us[0], vs[0], dists[0]);
      vgl_point_3d<double> v2(us[1], vs[1], dists[1]);
      vgl_point_3d<double> v3(us[2], vs[2], dists[2]);
      imesh_render_triangle_label<int>(v1, v2, v3, (int) iface, (*face_im), depth_im);
    } //end for iface

    // keep the vis_image just calculated
    vis_images_.push_back(face_im);
  }
}

//: Constructs vector of visibility images - images that identify which triangle is visible at which pixel.
// Function is more or less complete - not much more to it.
void boxm2_texture_mesh_process_globals::compute_norm_images( std::vector<vpgl_perspective_camera<double>* >& cameras,
                                                              imesh_mesh& in_mesh,unsigned int ni, unsigned int nj)
{
  auto& in_faces = (imesh_face_array&) in_mesh.faces();
  unsigned nfaces = in_mesh.num_faces();

  for (unsigned int i=0; i<vis_images_.size(); ++i) {
#if 0
    // get the principal point/ni,nj
    vpgl_perspective_camera<double>* pcam = cameras[i];
    vgl_point_2d<double> principal_point = pcam->get_calibration().principal_point();
    unsigned ni = (unsigned) (principal_point.x()*2.0);
    unsigned nj = (unsigned) (principal_point.y()*2.0);
#endif // 0
    // create a norm x,y, and z image for each visibility image
    vil_image_view<int>* vis = vis_images_[i];
    auto* nx = new vil_image_view<float>(ni, nj);
    auto* ny = new vil_image_view<float>(ni, nj);
    auto* nz = new vil_image_view<float>(ni, nj);
    vgl_vector_3d<double> paxis = cameras[i]->principal_axis();
    nx->fill( (float) paxis.x());
    ny->fill( (float) paxis.y());
    nz->fill( (float) paxis.z());
    for (unsigned int x=0; x<ni; ++x) {
      for (unsigned int y=0; y<nj; ++y) {
        int face_id = (*vis)(x,y);
        if (face_id >= 0 && face_id < (int)nfaces) {
          vgl_vector_3d<double> fnorm = in_faces.normal(face_id);
          (*nx)(x,y) = (float) fnorm.x();
          (*ny)(x,y) = (float) fnorm.y();
          (*nz)(x,y) = (float) fnorm.z();
        }
      }
    }

    //keep the vis_image just calculated
    normx_.push_back(nx);
    normy_.push_back(ny);
    normz_.push_back(nz);
  }

#if 0 // commented out ...
  imesh_vertex_array<3>& in_verts = in_mesh.vertices<3>();
  //iterate over each camera, creating a visibility image for each
  for (unsigned int i=0; i<cameras.size(); ++i)
  {
    //get the principal point of the cam for image size
    vpgl_perspective_camera<double>* pcam = cameras[i];
    vgl_point_2d<double> principal_point = pcam->get_calibration().principal_point();
    unsigned ni = (unsigned) (principal_point.x()*2.0);
    unsigned nj = (unsigned) (principal_point.y()*2.0);

    // render the norm images in each dimension
    vil_image_view<float>* nx = new vil_image_view<float>(ni, nj);
    vil_image_view<float>* ny = new vil_image_view<float>(ni, nj);
    vil_image_view<float>* nz = new vil_image_view<float>(ni, nj);
    vgl_vector_3d<double> paxis = cameras[i]->principal_axis();
    nx->fill( (float) paxis.x());
    ny->fill( (float) paxis.y());
    nz->fill( (float) paxis.z());
    vil_image_view<double> depth_im(ni, nj);
    depth_im.fill(10e100);
    for (unsigned iface = 0; iface<nfaces; ++iface)
    {
      //get face normal - this will be the "ID"
      vgl_vector_3d<double> fnorm = in_faces.normal(iface);

      //get the vertices from the face, project into UVs
      double us[3], vs[3], dists[3];
      for (int vIdx=0; vIdx<3; ++vIdx) {
        unsigned vertIdx = in_faces[iface][vIdx];

        //project these verts into UV
        double x = in_verts[vertIdx][0], y = in_verts[vertIdx][1], z = in_verts[vertIdx][2];
        double u,v;
        pcam->project(x, y, z, u, v);
        us[vIdx] = u;
        vs[vIdx] = v;

        //keep track of distance to each vertex
        dists[vIdx] = vgl_distance(vgl_point_3d<double>(x,y,z), pcam->get_camera_center());
      }

      //render the triangle label onto the label image, using the depth image
      vgl_point_3d<double> v1(us[0], vs[0], dists[0]);
      vgl_point_3d<double> v2(us[1], vs[1], dists[1]);
      vgl_point_3d<double> v3(us[2], vs[2], dists[2]);
      imesh_render_triangle_label<float>(v1, v2, v3, (float) fnorm.x(), (*nx), depth_im);
      imesh_render_triangle_label<float>(v1, v2, v3, (float) fnorm.y(), (*ny), depth_im);
      imesh_render_triangle_label<float>(v1, v2, v3, (float) fnorm.z(), (*nz), depth_im);
    } //end for iface

    //keep the vis_image just calculated
    normx_.push_back(nx);
    normy_.push_back(ny);
    normz_.push_back(nz);
  }
#endif // 0
}

void boxm2_texture_mesh_process_globals::smooth_norm_images(double sigma)
{
  vil_gauss_filter_5tap_params params(sigma); //set SIGMA
  for (unsigned int i=0; i<normx_.size(); ++i) {
    vil_image_view<float>* currX = normx_[i];
    auto* newX = new vil_image_view<float>(currX->ni(), currX->nj());
    vil_gauss_filter_5tap(*currX, *newX, params);
    //store the filtered image
    delete currX;
    normx_[i] = newX;

    //repeat for y
    vil_image_view<float>* currY = normy_[i];
    auto* newY = new vil_image_view<float>(currY->ni(), currY->nj());
    vil_gauss_filter_5tap(*currY, *newY, params);
    delete currY;
    normy_[i] = newY;

    //z
    vil_image_view<float>* currZ = normz_[i];
    auto* newZ = new vil_image_view<float>(currZ->ni(), currZ->nj());
    vil_gauss_filter_5tap(*currZ, *newZ, params);
    delete currZ;
    normz_[i] = newZ;
  }
}
