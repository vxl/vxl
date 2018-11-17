#include "boxm2_convert_nvm.h"
#include "boxm2_point_util.h"
//:
// \file
#include <cassert>
#include <utility>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vidl/vidl_image_list_istream.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vsph/vsph_camera_bounds.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vpgl/algo/vpgl_ortho_procrustes.h>

//: Main boxm2_convert_nvm function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_convert_nvm(std::string nvm_file,
                            std::string img_dir,
                            std::map<std::string, vpgl_perspective_camera<double>* >& cams,std::vector<vgl_point_3d<double> >  & pts,
                            vgl_box_3d<double>& bbox,
                            double& resolution,bool axis_align)
{
    boxm2_convert_nvm b2s(std::move(nvm_file), std::move(img_dir),axis_align);
    cams        = b2s.get_cams();
    bbox        = b2s.get_bbox();
    resolution  = b2s.get_resolution();

    pts = b2s.get_points();

    std::cout<<"# of pts "<<pts.size()<<std::endl;


}

// reads bundler file and populates list of cameras, and a scene bounding box
boxm2_convert_nvm::boxm2_convert_nvm(const std::string& nvm_file, const std::string& img_dir,bool axis_align)
{
    img_dir_ = img_dir;
    nvm_file_ = nvm_file;

    // verify image dir
    if (!vul_file::is_directory(img_dir.c_str()))
    {
        std::cout<<"boxm2_convert_nvm::Image directory does not exist"<<std::endl;
        return;
    }
    vidl_image_list_istream imgstream(img_dir+"/*");
    if (!imgstream.is_open())
    {
        std::cout<<"boxm2_convert_nvm::Invalid image stream"<<std::endl;
        return;
    }

    // get image size
    unsigned ni=imgstream.width();
    unsigned nj=imgstream.height();

    // central point of the image
    vgl_point_2d<double> ppoint((double)ni/2,(double)nj/2);
    std::cout<<"principal point for cams: "<<ppoint<<std::endl;

    // open the bundler file
    std::ifstream bfile( nvm_file.c_str() );
    if (!bfile)
    {
        std::cout<<"boxm2_convert_nvm::Error Opening Bundler output file"<<std::endl;
        return;
    }
    this->read_cameras(bfile, ppoint);
    this->read_points(bfile, ppoint);
    std::cout<<"NVM file out projection error:"<<std::endl;
    boxm2_point_util::calc_projection_error(cams_, bad_cams_, corrs_, view_error_map_, view_count_map_);

    //--------------------------------------------------------------------------
    // make sure the scene is axis aligned
    ////--------------------------------------------------------------------------
    if(axis_align)
    {
        if (!boxm2_point_util::axis_align_scene(corrs_,cams_))
            return;
        pts_3d_.clear() ;
        for (auto & corr : corrs_)
        {
            pts_3d_.push_back(corr->world_pt());
        }

    }
    //------------------------------------------------------------------------
    // Filter out the cams with very high error
    //------------------------------------------------------------------------
    boxm2_point_util::report_error(view_error_map_, view_count_map_, bad_cams_, 7.5f);
    std::cout<<"Num bad cams: "<<bad_cams_.size()<<std::endl;

    //------------------------------------------------------------------------
    // Save camera and corresponding image file
    //------------------------------------------------------------------------
    for (unsigned i = 0; i < cams_.size(); ++i) {
        if ( !bad_cams_.count(i) ) {
            std::string path = img_dir + "/" + names_[i]; // was: +vul_file::strip_extension(names_[i])+".png";
            // was: imgstream.seek_frame(i); std::string path = imgstream.current_path();
            auto* cam = new CamType(cams_[i]);
            final_cams_[path] = cam;
#ifdef DEBUG
            std::cout<<"Final cam: "<<path<<std::endl;
#endif
        }
    }//end camera write

    //------------------------------------------------------------------------
    // Save calc bounding box
    //------------------------------------------------------------------------

    vgl_box_3d<double> bounding_box;
    pts_3d_.clear() ;
    for (auto & corr : corrs_)
    {
        bounding_box.add(corr->world_pt());
        pts_3d_.push_back(corr->world_pt());
    }

    // Dimensions of the World
    std::cout<<"Full Point Bounding Box "<<bounding_box<<std::endl;
    vgl_point_3d<double> c = centre(pts_3d_);
    std::cout<<"Center of Gravity "<< c <<std::endl;
    vnl_double_3 sigma = boxm2_point_util::stddev(pts_3d_);
    std::cout<<"Point stddev "<< sigma <<std::endl;

    //--------------------------------------------------------------------------
    // Define dimensions to be used for a boxm scene
    // Note: x-y dimensions are kind of a good approximation
    // the z-dimension however suffers because most points tend to be on the ground and the average miss represents points off the gound
    //--------------------------------------------------------------------------
    double minx=c.x()-3.0f*sigma[0], miny=c.y()-3.0f*sigma[1], minz=c.z()-3.0f*sigma[2];
    double maxx=c.x()+ 3.0f*sigma[0], maxy= c.y()+3.0f*sigma[1], maxz=c.z()+3.0f*sigma[2];
    bbox_ = vgl_box_3d<double>(minx, miny, minz, maxx, maxy,maxz);

    //--------------------------------------------------------------------------
    // Determining the resolution of the cells
    //--------------------------------------------------------------------------
    int good_cam = 0;
    while ( bad_cams_.count(good_cam) > 0 ) good_cam++;
#ifdef DEBUG
    std::cout<<"Determining resolution of cells with cam: "<< good_cam << std::endl;
#endif

    vgl_ray_3d<double> cone_axis;
    double cone_half_angle, solid_angle;
    vsph_camera_bounds::pixel_solid_angle(cams_[good_cam], ni/4, nj/4,cone_axis,cone_half_angle,solid_angle);
    vgl_point_3d<double> cc = cams_[good_cam].camera_center();
    resolution_ = (cc-centre(pts_3d_)).length()*cone_half_angle/4;
#ifdef DEBUG
    std::cout<<"Resolution     "<<resolution_<<std::endl;
#endif
}

//------------------------------------------------------------------------
// reading the cameras from nvm file
//------------------------------------------------------------------------
bool boxm2_convert_nvm::read_cameras(std::ifstream& in, vgl_point_2d<double> ppoint)
{
    int rotation_parameter_num = 4;
    std::string token;
    bool format_r9t = false;
    if (in.peek() == 'N')
    {
        std::getline(in, token); // was: in >> token; //file header
        if (std::strstr(token.c_str(), "R9T"))
        {
            rotation_parameter_num = 9;  //rotation as 3x3 matrix
            format_r9t = true;
        }
    }

    // read # of cameras
    int ncam = 0;
    in >> ncam;
    if (ncam <= 1) {
        std::cout<<"Found fewer than 1 camera in NVM file (" << ncam<<')' <<std::endl;
        return false;
    }
    std::cout<<"Found "<<ncam<<" cameras in nvm file"<<std::endl;

    //read the camera parameters
    cams_.resize(ncam); // allocate the camera data
    names_.resize(ncam); // allocate token data
    for (int i = 0; i < ncam; ++i)
    {
        double f, q[9], c[3], d[2];
        in >> token >> f ;
        vpgl_calibration_matrix<double> K(f,ppoint);

        for (int j = 0; j < rotation_parameter_num; ++j) in >> q[j];
        in >> c[0] >> c[1] >> c[2] >> d[0] >> d[1];

        //if not fixed
        if (!format_r9t)
        {
            vnl_quaternion<double> quaternion(q[1],q[2],q[3],q[0]);
            vgl_rotation_3d<double> rot(quaternion);
            vgl_vector_3d<double> t(c[0],c[1],c[2]);
            vgl_point_3d<double> cc(c[0],c[1],c[2]);

            vpgl_perspective_camera<double> cam(K,cc,rot);
            cams_[i] = cam;
        }
        else
        {
            vnl_matrix_fixed<double,3,3> r;
            r(0,0) = q[0];  r(0,1) = q[1];  r(0,2) = q[2];
            r(1,0) = q[3];  r(1,1) = q[4];  r(1,2) = q[5];
            r(2,0) = q[6];  r(2,1) = q[7];  r(2,2) = q[8];

            vgl_rotation_3d<double> rot(r);
            vgl_vector_3d<double> t(c[0],c[1],c[2]);

            vpgl_perspective_camera<double> cam(K,rot,t);
            cams_[i] = cam;
        }

        //scrub name
        std::size_t found = 0;
        while ( (found=token.find('\\')) != std::string::npos )
            token.replace(found, 1, "/");
#ifdef DEBUG
        std::cout<<"Scrubbed filename: "<<token<<std::endl;
#endif
        names_[i] = vul_file::strip_directory(token);
    }
    return true;
}

//------------------------------------------------------------------------
// Read points into vector of bwm_video_corr_sptrs
//------------------------------------------------------------------------
bool boxm2_convert_nvm::read_points(std::ifstream& in, vgl_point_2d<double> ppoint)
{
    int npoint;
    in >> npoint;
    if (npoint <= 0) {
        std::cout<<"Found 0 points in nvm file, exiting"<<std::endl;
        return false;
    }
    std::cout<<"Found "<<npoint<<" points in nvm file."<<std::endl;

    //read image projections and 3D points.
    for (int i = 0; i < npoint; ++i)
    {
        float pt[3]; int cc[3], npj;
        in  >> pt[0] >> pt[1] >> pt[2]
        >> cc[0] >> cc[1] >> cc[2] >> npj;

        //create new bwm video corr
        bwm_video_corr_sptr corr = new bwm_video_corr();
        corr->set_world_pt(vgl_point_3d<double>(pt[0],pt[1],pt[2]));
        vgl_homg_point_3d<double> homg_world_pt(corr->world_pt());

        for (int j = 0; j < npj; ++j)
        {
            int cidx, fidx;   //camera index (view number), fidx?
            double imx, imy;  //image x and y
            in >> cidx >> fidx >> imx >> imy;
            imx += ppoint.x();
            imy += ppoint.y();

            //track correlations
            corr->add(cidx, vgl_point_2d<double>(imx,imy));
        }
        corrs_.push_back(corr);
    }
    return true;
}
