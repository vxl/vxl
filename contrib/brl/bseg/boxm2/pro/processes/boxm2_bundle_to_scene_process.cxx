// This is brl/bseg/boxm2/pro/processes/boxm2_bundle_to_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene from a bundler file
//
// \author Andy Miller
// \date Sep 16, 2011
#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/util/boxm2_bundle_to_scene.h>
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vil/vil_save.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_polygon_scan_iterator.h>

namespace boxm2_bundle_to_scene_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 2;

  //does this block ijk have an observation?
  bool has_observation(int i, int j, vgl_vector_3d<unsigned> pixPerBlock, vil_image_view<vxl_byte>& cntimg)
  {
    int startI = i*pixPerBlock.x();
    int startJ = j*pixPerBlock.y();
    int endI   = startI + pixPerBlock.x();
    int endJ   = startJ + pixPerBlock.y();
    for (int ii=startI; ii<endI; ++ii) {
      for (int jj=startJ; jj<endJ; ++jj) {
        if (cntimg(ii,jj) > 0)
          return true;
      }
    }
    return false;
  }

  //does this block ijk have a high enough percent of counts
  bool has_percent_views(int i,
                         int j,
                         double percent,
                         vgl_vector_3d<unsigned> pixPerBlock,
                         vil_image_view<vxl_byte>& cntimg,
                         int num_views)
  {
    int startI = i*pixPerBlock.x();
    int startJ = j*pixPerBlock.y();
    int endI   = startI + pixPerBlock.x();
    int endJ   = startJ + pixPerBlock.y();
    double mean = 0.0;
    for (int ii=startI; ii<endI; ++ii) {
      for (int jj=startJ; jj<endJ; ++jj) {
        mean += (double) cntimg(ii,jj);
      }
    }
    mean = mean / (pixPerBlock.x() * pixPerBlock.y());
    vcl_cout<<"Mean views: "<<mean<<vcl_endl
            <<"Mean percent: "<<mean / num_views<<vcl_endl;
    return mean/num_views > percent;
  }
}

bool boxm2_bundle_to_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_bundle_to_scene_process_globals;

  //process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[1] = "vcl_string";           //appearnce model
  input_types_[2] = "vcl_string";           //occupancy model
  input_types_[0] = "vcl_string";           //bundler out path
  input_types_[1] = "vcl_string";           //image dir path

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_scene_sptr";  //update scene
  output_types_[1] = "boxm2_scene_sptr";  //render scene
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_bundle_to_scene_process(bprb_func_process& pro)
{
  using namespace boxm2_bundle_to_scene_process_globals;
  typedef vpgl_perspective_camera<double> CamType;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vcl_vector<vcl_string> appearance(2,"");
  appearance[0] = "boxm2_mog3_grey";
  appearance[1] = "boxm2_num_obs";
  //appearance[0]          = pro.get_input<vcl_string>(i++); //Appearance Model String
  //appearance[1]          = pro.get_input<vcl_string>(i++); //Occupancy Model String
  vcl_string bundler_out = pro.get_input<vcl_string>(i++); //bundler out
  vcl_string img_dir     = pro.get_input<vcl_string>(i++); //bundler out

  //----------------------------------------------------------------------------
  //run bundle to scene
  //----------------------------------------------------------------------------
  vcl_map<vcl_string, CamType*> cams;
  vgl_box_3d<double>            bbox;
  double                        resolution;
  boxm2_util_bundle_to_scene(bundler_out, img_dir, cams, bbox, resolution);
  vcl_cout << "Bounding Box containing points which are [-3,3]sigma about x and y and [-1,5]-z_sigma about the scene center: " <<bbox<<vcl_endl;

  // get the inputs
  float  zplane  = (bbox.max_z()-bbox.min_z()) / 2.0;

  //populate vector of cameras
  //: returns a list of cameras from specified directory
  vcl_vector<vpgl_perspective_camera<double>* > cs;
  vcl_map<vcl_string, vpgl_perspective_camera<double>* >::iterator iter;
  for (iter=cams.begin(); iter!=cams.end(); ++iter)
    cs.push_back(iter->second);

  //run planar bounding box
  vgl_box_2d<double> b2box;
  bool good = vpgl_camera_bounds::planar_bouding_box(cs,b2box,zplane);
  if (good)
    vcl_cout<<"Bounding box found: "<<b2box<<vcl_endl;
  else
    vcl_cout<<"Bounding box not found."<<vcl_endl;

  //---------------------------------------------------------------------------
  //create zplane count map
  //---------------------------------------------------------------------------
  //determine the resolution of a pixel on the z plane
  vpgl_perspective_camera<double> cam = *cs[0];
  vgl_point_2d<double> pp = (cam.get_calibration()).principal_point();
  vgl_ray_3d<double> cone_axis;
  double cone_half_angle, solid_angle;
  vpgl_camera_bounds::pixel_solid_angle(cam, pp.x()/4, pp.y()/4, cone_axis, cone_half_angle, solid_angle);
  vgl_point_3d<double> cc = cam.camera_center();
  vgl_point_3d<double> zc( bbox.centroid().x(), bbox.centroid().y(), zplane);
  double res = 2*(cc-zc).length()*cone_half_angle;
  vcl_cout<<"Resres :"<<res<<vcl_endl;

  //create an image with this res, and count each pixel
  unsigned ni = (unsigned) (b2box.width()/res);
  unsigned nj = (unsigned) (b2box.height()/res);
  vil_image_view<vxl_byte> cntimg(ni, nj); cntimg.fill(0);
  vcl_cout<<"Created Box size: "<<ni<<','<<nj<<vcl_endl;
  for (unsigned int i=0; i<cs.size(); ++i)
  {
    //project the four corners to the ground plane
    cam = *cs[i];
    vgl_ray_3d<double> ul = cam.backproject(0.0, 0.0);
    vgl_ray_3d<double> ur = cam.backproject(2*pp.x(), 0.0);
    vgl_ray_3d<double> bl = cam.backproject(0.0, 2*pp.y());
    vgl_ray_3d<double> br = cam.backproject(2*pp.x(), 2*pp.y());

    //define z plane
    vgl_plane_3d<double> zp( vgl_point_3d<double>( 1.0,  1.0, zplane),
                             vgl_point_3d<double>( 1.0, -1.0, zplane),
                             vgl_point_3d<double>(-1.0,  1.0, zplane) );

    //intersect each ray with z plane
    vgl_point_3d<double> ulp, urp, blp, brp;
    bool good =    vgl_intersection(ul, zp, ulp);
    good = good && vgl_intersection(ur, zp, urp);
    good = good && vgl_intersection(bl, zp, blp);
    good = good && vgl_intersection(br, zp, brp);

    //convert the four corners into image coordinates
    typedef vgl_polygon<double>::point_t        Point_type;
    typedef vgl_polygon<double>                 Polygon_type;
    typedef vgl_polygon_scan_iterator<double>   Polygon_scan;
    Polygon_type poly;
    poly.new_sheet();
    poly.push_back( Point_type( (ulp.x()-b2box.min_x())/res, (ulp.y()-b2box.min_y())/res ) );
    poly.push_back( Point_type( (urp.x()-b2box.min_x())/res, (urp.y()-b2box.min_y())/res ) );
    poly.push_back( Point_type( (blp.x()-b2box.min_x())/res, (blp.y()-b2box.min_y())/res ) );
    poly.push_back( Point_type( (brp.x()-b2box.min_x())/res, (brp.y()-b2box.min_y())/res ) );

    // There will be scan lines at y=0, 1 and 2.
    Polygon_scan it(poly, false);
    int y=0;
    for (it.reset(); it.next(); ++y) {
      int y = it.scany();
      for (int x = it.startx(); x <= it.endx(); ++x) {
        int yy = nj-y;
        if (x>=0 && (unsigned)x<ni && yy>=0 && (unsigned)yy<nj) {
          cntimg(x, yy) += (vxl_byte) 1;
        }
#ifdef DEBUG
        else {
          vcl_cout<<"X and Y in scan iterator are out of bounds: "<<x<<','<<y<<vcl_endl;
        }
#endif
      }
    }
  }

  //find max/min/mean/var for count image
  vil_save(cntimg, "countImage.png");

  //---------------------------------------------------------------------------
  // Set up scene dimensions
  //---------------------------------------------------------------------------
  vgl_point_3d<double> scene_origin( b2box.min_x(), b2box.min_y(), bbox.min_z() );

  //number of voxels in this scene (x,y,z)
  vgl_vector_3d<unsigned> numVoxels( (unsigned) (b2box.width()/res),
                                     (unsigned) (b2box.height()/res),
                                     (unsigned) ((bbox.max_z()-bbox.min_z())/res) );

  //number of octrees in this scene (x,y,z)
  vgl_vector_3d<unsigned> totSubBlocks ( (unsigned) (numVoxels.x() / 8),
                                         (unsigned) (numVoxels.y() / 8),
                                         (unsigned) (numVoxels.z() / 8) );
  vgl_vector_3d<double> subBlockDim ( 8.0*res, 8.0*res, 8.0*res );

  //number of blocks in scene (8,8,1)
  vgl_vector_3d<unsigned> numBlocks(8, 8, 1);

  //number of subblocks per block
  vgl_vector_3d<unsigned> numSubBlocks( (unsigned) vcl_ceil( totSubBlocks.x()/numBlocks.x() ),
                                        (unsigned) vcl_ceil( totSubBlocks.y()/numBlocks.y() ),
                                        (unsigned) vcl_ceil( totSubBlocks.z()/numBlocks.z() ) );
  vgl_vector_3d<double>   blockDim( subBlockDim.x() * numSubBlocks.x(),
                                    subBlockDim.y() * numSubBlocks.y(),
                                    subBlockDim.z() * numSubBlocks.z() );
  //create update scene
  vcl_string scene_dir = "model/";
  if (!vul_file::make_directory_path( scene_dir.c_str()))
    return false;
  boxm2_scene_sptr uscene = new boxm2_scene(scene_dir, scene_origin);
  uscene->set_local_origin(scene_origin);
  uscene->set_appearances(appearance);
  for (unsigned int i=0; i<numBlocks.x(); ++i) {
    for (unsigned int j=0; j<numBlocks.y(); ++j) {
      for (unsigned int k=0; k<numBlocks.z(); ++k)
      {
        if ( has_observation(i,j,numSubBlocks*8, cntimg) ) {
          //get block map
          boxm2_block_id id(i,j,k);
          vcl_map<boxm2_block_id, boxm2_block_metadata> blks = uscene->blocks();
          if (blks.find(id)!=blks.end()) {
              vcl_cout<<"block already exists: "<<id<<vcl_endl;
              continue;
          }

          //block origin
          double bx = scene_origin.x() + i*blockDim.x();
          double by = scene_origin.y() + j*blockDim.y();
          double bz = scene_origin.z() + k*blockDim.z();

          ////get the inputs
          unsigned max_num_lvls     = 4;
          float    max_data_size    = 1500.0f;
          float    p_init           = 0.01f;
          boxm2_block_metadata mdata(id,
                                     vgl_point_3d<double>(bx,by,bz),
                                     subBlockDim,
                                     numSubBlocks,
                                     1,max_num_lvls,max_data_size,p_init);
          blks[id]=mdata;
          uscene->set_blocks(blks);
        }
      }
    }
  }

  //---------------------------------------------------------------------------
  // Create render scene
  //   - figure out which blocks are seen by enough images
  //---------------------------------------------------------------------------
  double thresh = .10;
  boxm2_scene_sptr rscene = new boxm2_scene(scene_dir, scene_origin);
  rscene->set_local_origin(scene_origin);
  rscene->set_appearances(appearance);
  for (unsigned int i=0; i<numBlocks.x(); ++i) {
    for (unsigned int j=0; j<numBlocks.y(); ++j) {
      for (unsigned int k=0; k<numBlocks.z(); ++k)
      {
        //needs at least .8 views on average
        if ( has_percent_views(i,j,thresh,numSubBlocks*8,cntimg,cs.size())) {
          //get block map
          boxm2_block_id id(i,j,k);
          vcl_map<boxm2_block_id, boxm2_block_metadata> blks = rscene->blocks();
          if (blks.find(id)!=blks.end()) {
              vcl_cout<<"block already exists: "<<id<<vcl_endl;
              continue;
          }

          //block origin
          double bx = scene_origin.x() + i*blockDim.x();
          double by = scene_origin.y() + j*blockDim.y();
          double bz = scene_origin.z() + k*blockDim.z();

          ////get the inputs
          unsigned max_num_lvls     = 4;
          float    max_data_size    = 1500.0f;
          float    p_init           = 0.01f;
          boxm2_block_metadata mdata(id,
                                     vgl_point_3d<double>(bx,by,bz),
                                     subBlockDim,
                                     numSubBlocks,
                                     1,max_num_lvls,max_data_size,p_init);
          blks[id]=mdata;
          rscene->set_blocks(blks);
        }
      }
    }
  }

  // store scene smart pointer
  i=0;
  pro.set_output_val<boxm2_scene_sptr>(i++, uscene);
  pro.set_output_val<boxm2_scene_sptr>(i++, rscene);
#if 0
  //clean up cameras
  for (int i=0; i<cams.size(); ++i)
    delete cams[i];
#endif
  return true;
}
