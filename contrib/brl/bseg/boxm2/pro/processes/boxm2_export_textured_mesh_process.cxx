// This is brl/bseg/boxm2/pro/processes/boxm2_export_textured_mesh_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for exporting a texture mapped mesh of a scene
//
// \author Vishal Jain
// \date Mar 15, 2011

#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>

//vil includes
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_math.h>

//vgl
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>

//vpgl camera stuff
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_project.h>

//det and imesh includes
#include <sdet/sdet_image_mesh.h>
#include <sdet/sdet_image_mesh_params.h>
#include <imesh/imesh_fileio.h>

namespace boxm2_export_textured_mesh_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 0;

  //helper texture map methods
  void boxm2_texture_mesh_from_imgs(vcl_string im_dir,
                                    vcl_string cam_dir,
                                    imesh_mesh& in_mesh,
                                    vcl_map<vcl_string, imesh_mesh>& meshes);
  void boxm2_texture_mesh_topdown(vcl_string im_dir,
                                  vcl_string cam_dir,
                                  imesh_mesh& in_mesh);
}

bool boxm2_export_textured_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_export_textured_mesh_process_globals;

  //process takes 2 inputs
  int i=0;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[i++] = "vil_image_view_base_sptr";  //depth image
  input_types_[i++] = "vpgl_camera_double_sptr";   //depth image cam
  input_types_[i++] = "boxm2_scene_sptr";          //scene object
  input_types_[i++] = "vcl_string";                //directory of scene images
  input_types_[i++] = "vcl_string";                //directory of corresponding cams
  input_types_[i++] = "vcl_string";                //output dir of saved mesh

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  //output_types_[0] = "boxm2_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_export_textured_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_export_textured_mesh_process_globals;
  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned argIdx = 0;
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(argIdx++);
  vpgl_camera_double_sptr  cam = pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  boxm2_scene_sptr       scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
  vcl_string img_dir           = pro.get_input<vcl_string>(argIdx++);
  vcl_string cam_dir           = pro.get_input<vcl_string>(argIdx++);
  vcl_string out_dir           = pro.get_input<vcl_string>(argIdx++);

  //create the mesh directory
  if (!vul_file::make_directory_path(out_dir.c_str())) {
    vcl_cout<<"Couldn't make directory path "<<out_dir<<vcl_endl;
    return false;
  }

  //cast camera and image so they are useful
  vpgl_perspective_camera<double>* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
  vil_image_view<float>* depth = (vil_image_view<float>*) img.ptr();

  //determine size of depth image
  unsigned ni = img->ni();
  unsigned nj = img->nj();

  //calculate XYZ image image that corresponds to depth image
  vil_image_view<vil_rgba<float> >* xyz_img = new vil_image_view<vil_rgba<float> >(ni, nj);
  vil_image_view<float>* z_img = new vil_image_view<float>(ni,nj);
  for (unsigned int i=0; i<ni; ++i) {
    for (unsigned int j=0; j<nj; ++j) {
      //grab the ray from the camera
      vgl_ray_3d<double> ray = pcam->backproject_ray( (double) i+.5, (double) j+.5);

      //ray o
      float ray_ox = ray.origin().x();
      float ray_oy = ray.origin().y();
      float ray_oz = ray.origin().z();

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
  vcl_cout<<"Generating image mesh"<<vcl_endl;

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
    vcl_cout<<"mesh could not be computed"<<vcl_endl;
    return 0;
  }
  imesh_mesh& mesh = im.get_mesh();
  vcl_cout << "Number of vertices " << mesh.num_verts()
           << "  number of faces "<< mesh.num_faces()<< '\n';

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
    unsigned i = static_cast<unsigned>(verts[iv][0]);
    unsigned j = static_cast<unsigned>(verts[iv][1]);
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
  vcl_cout<<mesh.has_tex_coords()<<vcl_endl;

  //////////////////////////////////////////////////////////////////////////////
  //Texture map the mesh
  //////////////////////////////////////////////////////////////////////////////
  //boxm2_texture_mesh_topdown(img_dir, cam_dir, mesh);
  vcl_map<vcl_string, imesh_mesh> meshes;
  boxm2_texture_mesh_from_imgs(img_dir, cam_dir, mesh, meshes);

  ////////////////////////////////////////////////////////////////////////////////
  //// Write out in VRML format
  ////////////////////////////////////////////////////////////////////////////////
  //output file and stream
  vcl_string vrfile = out_dir + "/vrmesh.wrl";
  vcl_ofstream os(vrfile.c_str());

  //write each submesh into one file
  vcl_map<vcl_string, imesh_mesh>::iterator subMesh;
  for (subMesh = meshes.begin(); subMesh != meshes.end(); ++subMesh)
  {
    imesh_mesh& sMesh = subMesh->second;
    vcl_cout<<"Writing sub mesh: "<<sMesh.tex_source()<<" has "<<sMesh.num_faces()<<" faces"<<vcl_endl;
    imesh_write_vrml(os, sMesh);
  }
  os.close();
  vcl_cout<<"Texture Mapping Mesh Time: "<<t.all()<<"ms"<<vcl_endl;

  return true;
}


void boxm2_export_textured_mesh_process_globals::boxm2_texture_mesh_from_imgs(vcl_string im_dir,
                                                                              vcl_string cam_dir,
                                                                              imesh_mesh& in_mesh,
                                                                              vcl_map<vcl_string, imesh_mesh>& meshes)
{
  ////////////////////////////////////////////////////////////////////////////////
  // BEGIN TEXTURE MAPPING
  // Gather cameras and iamges that will contribute to the texture
  ////////////////////////////////////////////////////////////////////////////////
  vcl_vector<vcl_string> camfiles = boxm2_util::camfiles_from_directory(cam_dir);
  vcl_vector<vcl_string> imfiles  = boxm2_util::images_from_directory(im_dir);

  vcl_vector<vpgl_perspective_camera<double>* > cameras;
  for (unsigned int i=0; i<camfiles.size(); ++i) {
    //build the camera from file
    vcl_ifstream ifs(camfiles[i].c_str());
    vpgl_perspective_camera<double>* icam = new vpgl_perspective_camera<double>;
    if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << camfiles[i] << '\n';
      return;
    }
    ifs >> *icam;
    cameras.push_back(icam);
  }


  ////////////////////////////////////////////////////////////////////////////////
  // For each Face:
  //   determine which image is closest, store face index in a map[image_str, vector<int>face]
  ////////////////////////////////////////////////////////////////////////////////
  in_mesh.compute_vertex_normals_from_faces();
  imesh_regular_face_array<3>& in_faces = (imesh_regular_face_array<3>&) in_mesh.faces();
  unsigned nfaces = in_mesh.num_faces();

  //texture index
  vcl_cout<<"Populating faces for each texture"<<vcl_endl;
  vcl_map<vcl_string, vcl_vector<unsigned> > app_faces; //image_name to face_list
  vcl_map<vcl_string, vpgl_perspective_camera<double>* > texture_cams;
  for (unsigned iface = 0; iface<nfaces; ++iface)
  {
    vgl_vector_3d<double>& normal = in_faces.normal(iface);

    //find camera with the closest look vector to this normal
    unsigned closeIdx = boxm2_util::find_nearest_cam(normal, cameras);
    vpgl_perspective_camera<double>* closest = cameras[closeIdx];
    vcl_string im_name = imfiles[closeIdx];

    //grab appropriate face list (create it if it's not there)
    vcl_map<vcl_string, vcl_vector<unsigned> >::iterator iter = app_faces.find(im_name);
    if ( iter == app_faces.end() ) {
      vcl_cout<<"Adding image: "<<im_name<<" to texture list"<<vcl_endl;
      vcl_vector<unsigned> faceList;
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
  imesh_vertex_array<3>& in_verts = in_mesh.vertices<3>();

  //for each appearance (texture image), create an imesh_mesh (subMesh);
  vcl_cout<<"Creating Sub Meshes for each texture"<<vcl_endl;
  vcl_map<vcl_string, vcl_vector<unsigned> >::iterator apps;
  for (apps = app_faces.begin(); apps != app_faces.end(); ++apps)
  {
    //for each appearance, we're creating a whole new mesh
    // first create the face list
    imesh_regular_face_array<3>* flist = new imesh_regular_face_array<3>();

    //now create the vertex list
    imesh_vertex_array<3>* verts3 = new imesh_vertex_array<3>();

    //get faces list corresponding to this texture
    vcl_vector<unsigned>& face_list = apps->second;
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
    vcl_auto_ptr<imesh_vertex_array_base> v3(verts3);
    vcl_auto_ptr<imesh_face_array_base> f3(flist);
    imesh_mesh subMesh(v3, f3);

    vcl_cout<<"Setting tex source: "<<apps->first<<vcl_endl;
    meshes[apps->first] = subMesh;
    meshes[apps->first].set_tex_source(apps->first);
  }

  //////////////////////////////////////////////////////////////////////////////
  //For each mesh, map each vertex
  //////////////////////////////////////////////////////////////////////////////
  vcl_cout<<"Mapping sub meshes for each texture"<<vcl_endl;
  vcl_map<vcl_string, imesh_mesh>::iterator subMesh;
  vcl_map<vcl_string, vpgl_perspective_camera<double>* >::iterator txCam = texture_cams.begin();
  for (subMesh = meshes.begin(); subMesh != meshes.end(); ++subMesh, ++txCam)
  {
    imesh_mesh& mesh = subMesh->second;
    imesh_vertex_array<3>& verts = mesh.vertices<3>();
    unsigned nverts = mesh.num_verts();

    vcl_vector<vgl_point_2d<double> > tex_coords(nverts, vgl_point_2d<double>(0.0,0.0));
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

void boxm2_export_textured_mesh_process_globals::boxm2_texture_mesh_topdown(vcl_string im_dir,
                                                                            vcl_string cam_dir,
                                                                            imesh_mesh& in_mesh)
{
  ////////////////////////////////////////////////////////////////////////////////
  // BEGIN TEXTURE MAPPING
  // Gather cameras and iamges that will contribute to the texture
  ////////////////////////////////////////////////////////////////////////////////
  vcl_vector<vcl_string> camfiles = boxm2_util::camfiles_from_directory(cam_dir);
  vcl_vector<vcl_string> imfiles  = boxm2_util::images_from_directory(im_dir);

  vcl_vector<vpgl_perspective_camera<double>* > cameras;
  for (unsigned int i=0; i<camfiles.size(); ++i) {
    //build the camera from file
    vcl_ifstream ifs(camfiles[i].c_str());
    vpgl_perspective_camera<double>* icam = new vpgl_perspective_camera<double>;
    if (!ifs.is_open()) {
        vcl_cerr << "Failed to open file " << camfiles[i] << '\n';
        return;
    }
    ifs >> *icam;
    cameras.push_back(icam);
  }

  //////////////////////////////////////////////////////////////////////////////
  // For each Face:
  //   determine which image is closest, store face index in a map[image_str, vector<int>face]
  ////////////////////////////////////////////////////////////////////////////////
  in_mesh.compute_vertex_normals_from_faces();
  unsigned nfaces = in_mesh.num_faces();

  //grab vertices in the mesh - convert them to scene coordinates (not image)
  imesh_vertex_array<3>& verts = in_mesh.vertices<3>();
  unsigned nverts = in_mesh.num_verts();

  //////////////////////////////////////////////////////////////////////////////
  // for each triangle,
  //   if surface normal is less than X degrees off horizontal
  //      project triangle onto image, store triangle points (see if there are functions for this in imesh)
  //////////////////////////////////////////////////////////////////////////////
  vcl_vector<vcl_string> tex_sources(nfaces, "");  //each face gets a
  vcl_vector<vgl_point_2d<double> > tex_coords(nverts, vgl_point_2d<double>(0.0,0.0));
  for (unsigned iv = 0; iv<nverts; ++iv)
  {
    //find camera with the closest look vector to this normal
    vpgl_perspective_camera<double>* closest = cameras[0];
    vgl_point_2d<double> principal_point = closest->get_calibration().principal_point();
    double ni = principal_point.x()*2.0;
    double nj = principal_point.y()*2.0;

    //project the vertex onto the camera, store the texture coordinate
    double x = verts[iv][0];
    double y = verts[iv][1];
    double z = verts[iv][2];
    double u,v;
    closest->project(x, y, z, u, v);

    //store the tex_coordinate
    vgl_point_2d<double> uv(u/nj,v/ni);
    tex_coords[iv] = uv;
  }
  in_mesh.set_tex_coords(tex_coords);
  in_mesh.set_tex_source(imfiles[0]);
  vcl_cout<<"Mesh has tex_coordinates "<<in_mesh.has_tex_coords()<<vcl_endl;
}
