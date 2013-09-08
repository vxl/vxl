#include "boxm2_cams_and_box_to_scene.h"
#include "boxm2_point_util.h"
//:
// \file

#include <boxm2/boxm2_block_metadata.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vsph/vsph_camera_bounds.h>
#include <vil/vil_image_view.h>

//~ //: takes in a list of cameras and a bounding box, creates
//  an update scene and a render scene
void boxm2_util_cams_and_box_to_scene (vcl_vector<CamType>& cams,
                                       vgl_box_3d<double>   bbox,
                                       boxm2_scene&         uscene,
                                       boxm2_scene&         rscene, int nblks)
{
    //----------------------------------------------------------------------------
    //Build the scenes,
    //----------------------------------------------------------------------------
    // get the inputs
    double zplane  = (bbox.max_z()-bbox.min_z()) / 2.0;

    //run planar bounding box
    vgl_box_2d<double> b2box(bbox.min_x(),bbox.max_x(),bbox.min_y(),bbox.max_y());


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
    double res = 2.0*(cc-zc).length()*cone_half_angle;
    vcl_cout<<"Resres :"<<res<<vcl_endl;

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
    vgl_vector_3d<unsigned> numBlocks(nblks, nblks, 1);

    vcl_cout<<"totSubBlocks "<<totSubBlocks<<vcl_endl;
    //number of subblocks per block
    vgl_vector_3d<unsigned> numSubBlocks( (unsigned) vcl_ceil( (float)totSubBlocks.x()/(float)numBlocks.x() ),
                                          (unsigned) vcl_ceil( (float)totSubBlocks.y()/(float)numBlocks.y() ),
                                          (unsigned) vcl_ceil( (float)totSubBlocks.z()/(float)numBlocks.z() ) );
    vcl_cout<<" Num Sub blocks "<<numSubBlocks<<vcl_endl;
    vgl_vector_3d<double>   blockDim( subBlockDim.x() * numSubBlocks.x(),
                                      subBlockDim.y() * numSubBlocks.y(),
                                      subBlockDim.z() * numSubBlocks.z() );

    //create an image with this res, and count each pixel
    unsigned ni = numSubBlocks.x()*numBlocks.x()*nblks;//(unsigned) (b2box.width()/res);
    unsigned nj = numSubBlocks.y()*numBlocks.y()*nblks;;
    vcl_cout<<"Created Box size: "<<ni<<','<<nj<<vcl_endl;
    vil_image_view<vxl_byte> cntimg(ni, nj); cntimg.fill(0);
    for (unsigned int i=0; i<cams.size(); ++i)
    {
        //project the four corners to the ground plane
        cam = cams[i];
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
    //vil_save(cntimg, "countImage.png");

    //---------------------------------------------------------------------------
    // Set up scene dimensions
    //---------------------------------------------------------------------------

    vcl_cout<<"Selecting Blocks for Rendering"<<vcl_endl;
    //create blocks in each direction
    double thresh = .10;
    for (unsigned int i=0; i<numBlocks.x(); ++i) {
        for (unsigned int j=0; j<numBlocks.y(); ++j) {
            for (unsigned int k=0; k<numBlocks.z(); ++k){
                //setup update scene with loose criteria
                //if ( boxm2_util_has_observation(i,j,numSubBlocks*nblks, cntimg) )
                {
                    //get block map
                    boxm2_block_id id(i,j,k);
                    vcl_map<boxm2_block_id, boxm2_block_metadata> blks = uscene.blocks();
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
                    uscene.set_blocks(blks);
                }

                //setup render scene with strict criteria
                if ( boxm2_util_has_percent_views(i,j,thresh,numSubBlocks*nblks,cntimg,cams.size()))
                {
                    vcl_cout.flush();
                    boxm2_block_id id(i,j,k);
                    vcl_map<boxm2_block_id, boxm2_block_metadata> blks = rscene.blocks();
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
                    rscene.set_blocks(blks);
                }
            }
        }
    }
}


//-----------------------------
//helper methods
//-----------------------------

//does this block ijk have an observation?
bool boxm2_util_has_observation(int i, int j, vgl_vector_3d<unsigned> pixPerBlock, vil_image_view<vxl_byte>& cntimg)
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
bool boxm2_util_has_percent_views(int i,
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
    vcl_cout << "Block (" <<i<< ',' << j << ") mean views,percent: " << mean << ',' << mean/num_views << vcl_endl;
    return mean/num_views > percent;
}
