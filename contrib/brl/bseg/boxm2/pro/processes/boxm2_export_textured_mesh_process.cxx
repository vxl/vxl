// This is brl/bseg/boxm2/pro/processes/boxm2_export_textured_mesh_process.cxx
#include <bprb/bprb_func_process.h>
#include <fstream>
#include <iostream>
#include <utility>
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
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>

//vil includes
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>

//vgl
#include <vgl/vgl_distance.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_point_3d.h>
#include <bvgl/bvgl_triangle_interpolation_iterator.h>

//vpgl camera stuff
#include <vpgl/vpgl_perspective_camera.h>

//det and imesh includes
#include <sdet/sdet_image_mesh.h>
#include <sdet/sdet_image_mesh_params.h>
#include <imesh/imesh_fileio.h>
#include <imesh/algo/imesh_render.h>

namespace boxm2_export_textured_mesh_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 0;

  //helper texture map methods
  void boxm2_texture_mesh_from_imgs(std::string im_dir,
                                    std::string cam_dir,
                                    imesh_mesh& in_mesh,
                                    std::map<std::string, imesh_mesh>& meshes);

  //populates a vector of visibility images (by face id int)
  void boxm2_visible_faces( std::vector<vpgl_perspective_camera<double>* >& cameras,
                            std::vector<vil_image_view<int>* >& vis_images,
                            imesh_mesh& in_mesh);


  //checks if a triangle in UV space is visible.  us and vs are double buffers of length 3
  bool face_is_visible( vpgl_perspective_camera<double>* cam,
                        vil_image_view<int>* vis_img,
                        const double* us,
                        const double* vs,
                        int face_id);
}

bool boxm2_export_textured_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_textured_mesh_process_globals;

  //process takes 2 inputs
  int i=0;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[i++] = "vil_image_view_base_sptr";  //depth image
  input_types_[i++] = "vpgl_camera_double_sptr";   //depth image cam
  input_types_[i++] = "boxm2_scene_sptr";          //scene object
  input_types_[i++] = "vcl_string";                //directory of scene images
  input_types_[i++] = "vcl_string";                //directory of corresponding cams
  input_types_[i++] = "vcl_string";                //output dir of saved mesh

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  //output_types_[0] = "boxm2_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_export_textured_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_export_textured_mesh_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned argIdx = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vpgl_camera_double_sptr  cam = pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  boxm2_scene_sptr       scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
  std::string img_dir = pro.get_input<std::string>(argIdx++);
  std::string cam_dir = pro.get_input<std::string>(argIdx++);
  std::string out_dir = pro.get_input<std::string>(argIdx++);

  if (img->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    auto* inim = (vil_image_view<vxl_byte>*) img.ptr();
    auto* fimg = new vil_image_view<float>(img->ni(), img->nj());
    for (unsigned int i=0; i<img->ni(); ++i)
      for (unsigned int j=0; j<img->nj(); ++j)
        (*fimg)(i,j) = (float) (*inim)(i,j) / 255.0f;
    img = vil_image_view_base_sptr(fimg);
  }

  //create the mesh directory
  if (!vul_file::make_directory_path(out_dir.c_str())) {
    std::cout<<"Couldn't make directory path "<<out_dir<<std::endl;
    return false;
  }

  //cast camera and image so they are useful
  auto* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
  auto* depth = (vil_image_view<float>*) img.ptr();

  //determine size of depth image
  unsigned ni = img->ni();
  unsigned nj = img->nj();

  //calculate XYZ image image that corresponds to depth image
  auto* xyz_img = new vil_image_view<vil_rgba<float> >(ni, nj);
  auto* z_img = new vil_image_view<float>(ni,nj);
  for (unsigned int i=0; i<ni; ++i) {
    for (unsigned int j=0; j<nj; ++j) {
      //grab the ray from the camera
      vgl_ray_3d<double> ray = pcam->backproject_ray( (double) i+.5, (double) j+.5);

      //ray o
      float ray_ox = float(ray.origin().x()),
            ray_oy = float(ray.origin().y()),
            ray_oz = float(ray.origin().z());

      //ray dir
      float dray_x = float(ray.direction().x()),
            dray_y = float(ray.direction().y()),
            dray_z = float(ray.direction().z());

      //expected ray depth
      float ray_t = (*depth)(i,j);

      //calculate x,y,z position at expected depth
      (*xyz_img)(i,j) = vil_rgba<float>(ray_ox - ray_t*dray_x,
                                        ray_oy - ray_t*dray_y,
                                        ray_oz - ray_t*dray_z,
                                        0.0f);
      (*z_img)(i,j) = ray_oz - ray_t*dray_z;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // Generate 3d mesh from depth image
  //////////////////////////////////////////////////////////////////////////////
  std::cout<<"Generating image mesh"<<std::endl;

  //create new resource sptr
  vil_image_resource_sptr z_img_res = vil_new_image_resource_of_view(*z_img);

  //initialize some sdet_image_mesh parameters
  sdet_image_mesh_params imp;
  // sigma of the Gaussian for smoothing the image prior to edge detection
  imp.smooth_ = 2.0f;
  // the edge detection threshold
  imp.thresh_ = 2.0f;
  // the shortest edgel chain that is considered for line fitting
  imp.min_fit_length_ = 7;
  // the threshold on rms pixel distance of edgels to the line
  imp.rms_distance_ = 0.1;
  // the width in pixels of the transition of a step edge
  imp.step_half_width_ = 5.0;

  // the mesh processor
  sdet_image_mesh im(imp);
  im.set_image(z_img_res);
  if (!im.compute_mesh()) {
    std::cout<<"mesh could not be computed"<<std::endl;
    return false;
  }
  imesh_mesh& mesh = im.get_mesh();
  std::cout << "Number of vertices " << mesh.num_verts()
           << "  number of faces "<< mesh.num_faces()<< '\n';

  std::string meshFile = out_dir + "/wire.wrl";
  std::ofstream wos(meshFile.c_str());
  imesh_write_vrml(wos, mesh);
  wos.close();

  ////////////////////////////////////////////////////////////////////////////////
  //// normalize mesh world points to fit in the image_bb from above
  ////////////////////////////////////////////////////////////////////////////////
  vul_timer t;
  //grab vertices in the mesh - convert them to scene coordinates (not image)
  imesh_vertex_array<3>& verts = mesh.vertices<3>();
  unsigned nverts = mesh.num_verts();
  for (unsigned iv = 0; iv<nverts; ++iv)
  {
    //get coordinates so you can index into the height map
    auto i = static_cast<unsigned>(verts[iv][0]);
    auto j = static_cast<unsigned>(verts[iv][1]);
    double scene_x=0, scene_y=0,scene_z = 0.0;
    if (i<ni && j<nj) {
      scene_x = (*xyz_img)(i,j).R();
      scene_y = (*xyz_img)(i,j).G();
      scene_z = (*xyz_img)(i,j).B();
    }
    verts[iv][0] = scene_x;
    verts[iv][1] = scene_y;
    verts[iv][2] = scene_z;
  }
  std::cout<<mesh.has_tex_coords()<<std::endl;

  //////////////////////////////////////////////////////////////////////////////
  //Texture map the mesh
  //////////////////////////////////////////////////////////////////////////////
  //boxm2_texture_mesh_topdown(img_dir, cam_dir, mesh);
  std::map<std::string, imesh_mesh> meshes;
  boxm2_texture_mesh_from_imgs(img_dir, cam_dir, mesh, meshes);

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


//Given a directory of images, dir cams, an input mesh, this function creates
//map of textured meshes (imesh doesn't ostensibly handle meshes from multiple textures)
void boxm2_export_textured_mesh_process_globals::boxm2_texture_mesh_from_imgs(std::string im_dir,
                                                                              std::string cam_dir,
                                                                              imesh_mesh& in_mesh,
                                                                              std::map<std::string, imesh_mesh>& meshes)
{
  ////////////////////////////////////////////////////////////////////////////////
  // BEGIN TEXTURE MAPPING
  // Gather cameras and iamges that will contribute to the texture
  ////////////////////////////////////////////////////////////////////////////////
  std::vector<std::string> camfiles = boxm2_util::camfiles_from_directory(std::move(cam_dir));
  std::vector<std::string> imfiles = boxm2_util::images_from_directory(std::move(im_dir));

  std::vector<vpgl_perspective_camera<double>* > cameras;
  for (auto & camfile : camfiles) {
    //build the camera from file
    std::ifstream ifs(camfile.c_str());
    auto* icam = new vpgl_perspective_camera<double>;
    if (!ifs.is_open()) {
      std::cerr << "Failed to open file " << camfile << '\n';
      return;
    }
    ifs >> *icam;
    cameras.push_back(icam);
    std::cout<<icam->principal_axis()<<std::endl;
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Grab input mesh vertices and input mesh faces
  ////////////////////////////////////////////////////////////////////////////////
  in_mesh.compute_vertex_normals_from_faces();
  auto& in_faces = (imesh_regular_face_array<3>&) in_mesh.faces();
  unsigned nfaces = in_mesh.num_faces();
  imesh_vertex_array<3>& in_verts = in_mesh.vertices<3>();

  ////////////////////////////////////////////////////////////////////////////////
  // Render Visibility Images
  ////////////////////////////////////////////////////////////////////////////////
  std::vector<vil_image_view<int>* > vis_images;
  boxm2_visible_faces(cameras, vis_images, in_mesh);

  ////////////////////////////////////////////////////////////////////////////////
  // For each Face:
  //   determine which image is closest in normal and visible, store face index in a map[image_str, vector<int>face]
  ////////////////////////////////////////////////////////////////////////////////
  //texture index
  std::cout<<"Populating faces for each texture"<<std::endl;
  std::map<std::string, std::vector<unsigned> > app_faces; //image_name to face_list
  std::map<std::string, vpgl_perspective_camera<double>* > texture_cams;
  for (unsigned iface = 0; iface<nfaces; ++iface)
  {
    //create list of cameras from which you can see this face
    std::vector<vpgl_perspective_camera<double>* > visible_views;
    for (unsigned int i=0; i<vis_images.size(); ++i) {
      //project triangle
      double us[3], vs[3];
      for (int vIdx=0; vIdx<3; ++vIdx) {
        unsigned vertIdx = in_faces[iface][vIdx];

        //project these verts into UV
        double x = in_verts[vertIdx][0], y = in_verts[vertIdx][1], z = in_verts[vertIdx][2];
        double u,v;
        cameras[i]->project(x, y, z, u, v);
        us[vIdx] = u;
        vs[vIdx] = v;
      }
      if ( face_is_visible( cameras[i], vis_images[i], us, vs, iface) )
        visible_views.push_back(cameras[i]);
    }

    //now compare the normal to each of the visible view cameras
    vgl_vector_3d<double>& normal = in_faces.normal(iface);

    //find camera with the closest look vector to this normal
    int closeIdx = boxm2_util::find_nearest_cam(normal, visible_views);
    vpgl_perspective_camera<double>* closest = nullptr;
    std::string im_name = "empty";
    if (closeIdx >= 0) {
      closest = cameras[closeIdx];
      im_name = imfiles[closeIdx];
    }

    //grab appropriate face list (create it if it's not there)
    auto iter = app_faces.find(im_name);
    if ( iter == app_faces.end() ) {
      std::cout<<"Adding image: "<<im_name<<" to texture list"<<std::endl;
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

  ////////////////////////////////////////////////////////////////////////////////
  // For each image/appearance:
  //   - create a vert list
  //   - create a face list
  //   - create a sub mesh that is textured
  ////////////////////////////////////////////////////////////////////////////////

  //for each appearance (texture image), create an imesh_mesh (subMesh);
  std::cout<<"Creating Sub Meshes for each texture"<<std::endl;
  std::map<std::string, std::vector<unsigned> >::iterator apps;
  for (apps = app_faces.begin(); apps != app_faces.end(); ++apps)
  {
    //for each appearance, we're creating a whole new mesh
    // first create the face list
    auto* flist = new imesh_regular_face_array<3>();

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
    if (txCam->first != "empty")
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
  }
}

bool boxm2_export_textured_mesh_process_globals::face_is_visible( vpgl_perspective_camera<double>*  /*cam*/,
                                                                  vil_image_view<int>* vis_img,
                                                                  const double* us,
                                                                  const double* vs,
                                                                  int face_id)
{
  //now create a polygon, and find the integer image coordinates (U,V) that this polygon covers
  int ni = vis_img->ni();
  int nj = vis_img->nj();

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
      if ( (*vis_img)(x,y) != face_id )
        return false;
    }
  }

  //if it made it this far, it's completely visible
  return true;
}


void boxm2_export_textured_mesh_process_globals::boxm2_visible_faces( std::vector<vpgl_perspective_camera<double>* >& cameras,
                                                                      std::vector<vil_image_view<int >* >& vis_images,
                                                                      imesh_mesh& in_mesh)
{
  auto& in_faces = (imesh_regular_face_array<3>&) in_mesh.faces();
  unsigned nfaces = in_mesh.num_faces();
  imesh_vertex_array<3>& in_verts = in_mesh.vertices<3>();

  //iterate over each camera, creating a visibility image for each
  for (auto pcam : cameras)
  {
    //get the principal point of the cam for image size
    vgl_point_2d<double> principal_point = pcam->get_calibration().principal_point();
    auto ni = (unsigned) (principal_point.x()*2.0);
    auto nj = (unsigned) (principal_point.y()*2.0);

    // render the face_id/distance image
    vil_image_view<double> depth_im(ni, nj);
    auto*   face_im = new vil_image_view<int>(ni, nj);
    depth_im.fill(10e100);  //Initial depth is huge,
    face_im->fill(-1); //initial face id is -1
    for (unsigned iface = 0; iface<nfaces; ++iface)
    {
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
      imesh_render_triangle_label<int>(v1, v2, v3, (int) iface, (*face_im), depth_im);
    } //end for iface

    //keep the vis_image just calculated
    vis_images.push_back(face_im);
    vil_save(depth_im, "/media/VXL/mesh/downtown/dist_im.tif");
  }
}
