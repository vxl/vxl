// Main to run opencl implementation of onlineupdate
#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/ocl/boxm_online_update_manager.h>
#include <boxm/ocl/boxm_update_ocl_scene_manager.h>

#include <boxm/ocl/boxm_refine_manager.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_init_scene.h>
#include <boxm/boxm_scene_parser.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_block.h>
#include <boct/boct_tree.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#include <vcl_vector.h>

#if 0 // commented out
void save_tree_and_data_array(boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > >* scene,
                              vcl_string treefile, vcl_string treedatafile)
{
  scene->load_block(0,0,0);
  boxm_block<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > > * block=scene->get_active_block();
  boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > * tree=block->get_tree();

  vcl_vector<vnl_vector_fixed<int, 4> > cell_array;
  vcl_vector<vnl_vector_fixed<float, 16> > data_array;
  vnl_vector_fixed<int, 4> root_cell(0);
  root_cell[0]=-1;                                     // no parent
  root_cell[1]=-1;                                     // no children at the moment
  root_cell[2]=-1;
  root_cell[3]=-1;
  // no data at the moment
  cell_array.push_back(root_cell);

  boxm_ocl_convert<boxm_sample<BOXM_APM_MOG_GREY> >::copy_to_arrays(tree->root(),cell_array,data_array,0);

  vcl_size_t cells_size    =cell_array.size();
  vcl_size_t cell_data_size=data_array.size();

  int *cells = NULL;
  float * cell_data = NULL;

  cells=(cl_int *)boxm_ocl_utils::alloc_aligned(cells_size,sizeof(cl_int4),16);
  cell_data=(cl_float *)boxm_ocl_utils::alloc_aligned(cell_data_size,sizeof(cl_float16),16);
  if (cells== NULL||cell_data == NULL )
  {
      vcl_cout << "Failed to allocate host memory. (tree input)\n";
      return ;
  }

  for (unsigned i = 0, j = 0; i<cell_array.size()*4; i+=4, j++)
      for (unsigned k = 0; k<4; ++k)
          cells[i+k]=cell_array[j][k];

  // appearance model is 8 and alpha is 1
  for (unsigned i = 0, j = 0; i<data_array.size()*16; i+=16, j++)
      for (unsigned k = 0; k<16; ++k)
          cell_data[i+k]=data_array[j][k];

  boxm_ocl_utils::writetree(treefile,cells,cells_size);
  boxm_ocl_utils::writetreedata(treedatafile,cell_data,cell_data_size);
}


int main(int argc,  char** argv)
{
  vcl_cout<<"UPDATE"<<vcl_endl;
  vul_arg<vcl_string> cam_file("-cam", "camera filename", "");
  vul_arg<vcl_string> cam_dir("-camdir", "camera directory", "");
  vul_arg<vcl_string> img_dir("-imgdir", "Image directory", "");

  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> img("-img", "image filename", "");
  vul_arg<vcl_string> workdir("-workdir", "work directory", "");

  vul_arg_parse(argc, argv);

  bool flag=true;
  if (vul_file::is_directory(cam_dir().c_str()))
  {
    vcl_string camglob=cam_dir()+"/*.txt";
    vcl_string imgglob=img_dir()+"/*.jpg";

    vul_file_iterator file_it(camglob.c_str());
    vul_file_iterator img_file_it(imgglob.c_str());

    int count=0;
    while (file_it && img_file_it)
    {
    //if (count2)
    {
      vcl_cout<<"Cam "<<file_it()<<" Image "<<img_file_it()<<vcl_endl;

      vcl_ifstream ifs(file_it());
      vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
      if (!ifs.is_open()) {
        vcl_cerr << "Failed to open file " << file_it() << vcl_endl;
        return -1;
      }

      else{
        ifs >> *pcam;

        boxm_online_update_manager* updt_mgr= boxm_online_update_manager::instance();
        int bundle_dim=8;
        updt_mgr->set_bundle_ni(bundle_dim);
        updt_mgr->set_bundle_nj(bundle_dim);

        vul_file::is_directory(workdir().c_str());

        vcl_string treefile =workdir()+"/treefile.bin";
        vcl_string treedatafile =workdir()+"/treedatafile.bin";

        vil_image_view_base_sptr loaded_image = vil_load(img_file_it());//img().c_str() );
        vil_image_view<float> floatimg(loaded_image->ni(), loaded_image->nj(), 1);

        // load the image
        if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
        {
          vil_convert_stretch_range_limited(*img_byte ,floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
        }
        else
        {
          return -1;
        }

        // load the scene;
        boxm_scene_parser parser;
        boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
        scene_ptr->load_scene(scene_file(), parser);

        if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY) {
          typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
          boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
          scene->load_scene(parser);
          scene_ptr = scene;
          if (flag)
          {
            boxm_init_scene<BOXM_APM_MOG_GREY>(*scene);
            save_tree_and_data_array(scene,treefile,treedatafile);
            flag=false;
          }
          updt_mgr->init_update(treefile,treedatafile,scene->origin(),scene->block_dim(), pcam, floatimg,scene->max_level()-1);
        }
        else
        {
          return -1;
        }

        if (!updt_mgr->setup_norm_data(true, 0.5f, 0.25f))
          return -1;

        updt_mgr->process_block(5);
        boxm_refine_manager<boxm_sample<BOXM_APM_MOG_GREY> > * refine_mgr
          =boxm_refine_manager<boxm_sample<BOXM_APM_MOG_GREY> >::instance();
        refine_mgr->init(treefile,treedatafile,2,0.45,scene_ptr->max_level(),scene_ptr->get_world_bbox().width());

        refine_mgr->run_tree();

        cl_int * cells_refined = refine_mgr->get_tree();
        cl_float * cell_data_refined = refine_mgr->get_data();

        boxm_ocl_utils::writetree(treefile,cells_refined,refine_mgr->get_tree_size());
        boxm_ocl_utils::writetreedata(treedatafile,cell_data_refined,refine_mgr->get_data_size());

        refine_mgr->clean_refine();
      }
    }
    ++file_it ; ++img_file_it;
    ++file_it ; ++img_file_it;
    count++;
    }
  }

  return 0;
}
#endif // 0

int main(int argc,  char** argv)
{
  // handle arguments
  vcl_cout<<"OCL SCENE UPDATE"<<vcl_endl;
  vul_arg<vcl_string> cam_dir("-camdir", "camera directory", "");
  vul_arg<vcl_string> img_dir("-imgdir", "Image directory", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg_parse(argc, argv);
  if (!vul_file::is_directory(cam_dir().c_str()))
    return -1;

  //initialize the ocl scene and update manager
  boxm_ocl_scene ocl_scene(scene_file());
  boxm_update_ocl_scene_manager* updt_mgr = boxm_update_ocl_scene_manager::instance();
  int bundle_dim=8;
  updt_mgr->set_bundle_ni(bundle_dim);
  updt_mgr->set_bundle_nj(bundle_dim);

  //get all of the cam and image files, and sort them!
  vcl_string camglob=cam_dir()+"/*.txt";
  vcl_string imgglob=img_dir()+"/*.jpg";
  vul_file_iterator file_it(camglob.c_str());
  vul_file_iterator img_file_it(imgglob.c_str());

  vcl_vector<vcl_string> cam_files;
  vcl_vector<vcl_string> img_files;
  while (file_it && img_file_it) {
    vcl_string camName(file_it());
    vcl_string imgName(img_file_it());
    cam_files.push_back(camName);
    img_files.push_back(imgName);
    ++file_it; ++img_file_it;
  }
  vcl_sort(cam_files.begin(), cam_files.end());
  vcl_sort(img_files.begin(), img_files.end());
  if (cam_files.size() != img_files.size()) {
    vcl_cerr<<"Image files and cam files not one to one\n";
    return -1;
  }

  //loop through the images/cams and build the scene
  for (unsigned int i=0; i<cam_files.size(); ++i)
  {
    vcl_cout<<"Cam "<<cam_files[i]<<" Image "<<img_files[i]<<vcl_endl;

    //build the camera from file
    vcl_ifstream ifs(cam_files[i].c_str());
    vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
    if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << file_it() << vcl_endl;
      return -1;
    }
    ifs >> *pcam;

    //load image from file
    vil_image_view_base_sptr loaded_image = vil_load(img_files[i].c_str());
    vil_image_view<float> floatimg(loaded_image->ni(), loaded_image->nj(), 1);
    if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
      vil_convert_stretch_range_limited(*img_byte ,floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
    else
      return -1;
    if (i==0)
    {
      updt_mgr->init_update(&ocl_scene,pcam, floatimg);
      if (!updt_mgr->setup_norm_data(true, 0.5f, 0.25f))
        return -1;
      updt_mgr->setup_online_processing();
    }
    else
    {
      updt_mgr->set_input_image(floatimg);
      updt_mgr->write_image_buffer();
      updt_mgr->set_persp_camera(pcam);
      updt_mgr->write_persp_camera_buffers();
    }
    updt_mgr->online_processing();
  }
  updt_mgr->finish_online_processing();
  updt_mgr->clean_update();

  ocl_scene.save();
  return 0;
}
