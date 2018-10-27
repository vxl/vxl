#include "bstm_cams_and_box_to_scene.h"
//:
// \file

#include <bstm/bstm_block_metadata.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vsph/vsph_camera_bounds.h>
#include <vil/vil_image_view.h>

//~ //: takes in a list of cameras and a bounding box, creates
//  an update scene and a render scene
void bstm_util_cams_and_box_to_scene (std::vector<CamType>& cams,
                                       vgl_box_3d<double>   bbox,
                                       bstm_scene&         uscene,
                                       unsigned time_steps,
                                       int nblks)
{
    //----------------------------------------------------------------------------
    //Build the scenes,
    //----------------------------------------------------------------------------
    // get the inputs
    double zplane  = (bbox.max_z()-bbox.min_z()) / 2.0;

    //run planar bounding box
    vgl_box_2d<double> b2box(bbox.min_x(),bbox.max_x(),bbox.min_y(),bbox.max_y());

    //time related stuff
    double scene_origin_t = 0;
    double subBlockDim_t = 32; //fixed for now
    unsigned num_blocks_t = (double)(time_steps) / subBlockDim_t;
    unsigned numSubBlocks_t = 1;
    std::cout << "Num t blosk :" << num_blocks_t << std::endl;

    //---------------------------------------------------------------------------
    //create zplane count map
    //---------------------------------------------------------------------------
    //determine the resolution of a pixel on the z plane
    vpgl_perspective_camera<double> cam = cams[0];
    vgl_point_2d<double> pp = (cam.get_calibration()).principal_point();
    vgl_ray_3d<double> cone_axis;
    double cone_half_angle, solid_angle;
    vsph_camera_bounds::pixel_solid_angle(cam, int(pp.x()), int(pp.y()), cone_axis, cone_half_angle, solid_angle);
    vgl_point_3d<double> cc = cam.camera_center();
    vgl_point_3d<double> zc( b2box.centroid().x(), b2box.centroid().y(), zplane);
    double res = 4.0*(cc-zc).length()*cone_half_angle;
    std::cout<<"Resres :"<<res<<std::endl;

    //extend bbox a bit
    //double extSize = b2box.width() * .1;
    //b2box = vgl_box_2d<double>(b2box.min_x()-extSize, b2box.max_x()+extSize, b2box.min_y()-extSize, b2box.max_y()+extSize);
    vgl_point_3d<double> scene_origin( b2box.min_x(), b2box.min_y(), bbox.min_z() );



    //number of voxels in this scene (x,y,z)
    vgl_vector_3d<unsigned> numVoxels( (unsigned) (b2box.width()/res),
                                        (unsigned) (b2box.height()/res),
                                        (unsigned) ((bbox.max_z()-bbox.min_z())/res) );

    //number of octrees in this scene (x,y,z)
    vgl_vector_3d<unsigned> totSubBlocks ( (unsigned) (numVoxels.x() / nblks),
                                            (unsigned) (numVoxels.y() / nblks),
                                            (unsigned) (numVoxels.z() / nblks) );
    vgl_vector_3d<double> subBlockDim ( nblks*res, nblks*res, nblks*res );

    //number of blocks in scene (nblks,nblks,1)
    vgl_vector_3d<unsigned> numBlocks(nblks, nblks, 3);

    std::cout<<"totSubBlocks "<<totSubBlocks<<std::endl;
    //number of subblocks per block
    vgl_vector_3d<unsigned> numSubBlocks( (unsigned) std::ceil( (float)totSubBlocks.x()/(float)numBlocks.x() ),
                                          (unsigned) std::ceil( (float)totSubBlocks.y()/(float)numBlocks.y() ),
                                          (unsigned) std::ceil( (float)totSubBlocks.z()/(float)numBlocks.z() ) );
    std::cout<<" Num Sub blocks "<<numSubBlocks<<std::endl;
    vgl_vector_3d<double>   blockDim( subBlockDim.x() * numSubBlocks.x(),
                                      subBlockDim.y() * numSubBlocks.y(),
                                      subBlockDim.z() * numSubBlocks.z() );

    //create an image with this res, and count each pixel
    unsigned ni = numSubBlocks.x()*numBlocks.x()*nblks;//(unsigned) (b2box.width()/res);
    unsigned nj = numSubBlocks.y()*numBlocks.y()*nblks;;
    std::cout<<"Created Box size: "<<ni<<','<<nj<<std::endl;
    vil_image_view<vxl_byte> cntimg(ni, nj); cntimg.fill(0);
    for (const auto & i : cams)
    {
        //project the four corners to the ground plane
        cam = i;
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
        if (!good) {
            std::cout << "ERROR: lines do not intersect" << __FILE__ << __LINE__ << std::endl;
        }

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
                    std::cout<<"X and Y in scan iterator are out of bounds: "<<x<<','<<y<<std::endl;
                }
#endif
            }
        }
    }

    //find max/min/mean/var for count image
    //vil_save(cntimg, "countImage.png");

    //---------------------------------------------------------------------------
    // Set up scene dimensions
    //---------------------------------------------------------------------------

    //create blocks in each direction
    for (unsigned int i=0; i<numBlocks.x(); ++i) {
        for (unsigned int j=0; j<numBlocks.y(); ++j) {
            for (unsigned int k=0; k<numBlocks.z(); ++k){
              for(unsigned int t = 0; t < num_blocks_t; ++t)
              {
                  //setup update scene with loose criteria

                  //get block map
                  bstm_block_id id(i,j,k,t);
                  std::map<bstm_block_id, bstm_block_metadata> blks = uscene.blocks();
                  if (blks.find(id)!=blks.end()) {
                      std::cout<<"block already exists: "<<id<<std::endl;
                      continue;
                  }

                  //block origin
                  double bx = scene_origin.x() + i*blockDim.x();
                  double by = scene_origin.y() + j*blockDim.y();
                  double bz = scene_origin.z() + k*blockDim.z();
                  double bt = scene_origin_t + t*subBlockDim_t;

                  ////get the inputs
                  unsigned max_num_lvls     = 4;
                  float    max_data_size    = 1500.0f;
                  float    p_init           = 0.01f;
                  bstm_block_metadata mdata(id,
                                             vgl_point_3d<double>(bx,by,bz),
                                             bt,
                                             subBlockDim, subBlockDim_t,
                                             numSubBlocks, numSubBlocks_t,
                                             1,max_num_lvls,max_data_size,p_init);
                  blks[id]=mdata;
                  uscene.set_blocks(blks);
              }
            }
        }
    }
}


//-----------------------------
//helper methods
//-----------------------------

//does this block ijk have an observation?
bool bstm_util_has_observation(int i, int j, vgl_vector_3d<unsigned> pixPerBlock, vil_image_view<vxl_byte>& cntimg)
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
bool bstm_util_has_percent_views(int i,
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
    std::cout << "Block (" <<i<< ',' << j << ") mean views,percent: " << mean << ',' << mean/num_views << std::endl;
    return mean/num_views > percent;
}
