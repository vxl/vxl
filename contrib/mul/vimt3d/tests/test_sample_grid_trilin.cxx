// This is mul/vimt3d/tests/test_sample_grid_trilin.cxx
// It also tests vimt3d_reconstruct_from_grid

#include <vcl_iostream.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vimt3d/vimt3d_sample_grid_trilin.h>
#include <vimt3d/vimt3d_reconstruct_from_grid.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <testlib/testlib_test.h>

void compare_images(const vimt3d_image_3d_of<vxl_int_32> &image1,
                    const vimt3d_image_3d_of<vxl_int_32> &image2)
{
    bool different=true;

    if (image1.world2im()==image2.world2im())
    {
        different=false;
        for (unsigned int i=0;i<image1.image().ni();++i)
        {
            for (unsigned int j=0;j<image1.image().nj();++j)
            {
                for (unsigned int k=0;k<image1.image().nk();++k)
                {
                    double val1=image1.image()(i,j,k);
                    double val2=image2.image()(i,j,k);
                    if (val1!=val2)
                    {
                        different=true;
                    }
                }
            }
        }
    }
    TEST("Reconstructed image is = original image",different,false);
}

static void test_sample_grid_trilin()
{
    vcl_cout << "**************************************\n"
             << " Testing vimt3d_sample_grid_trilin_3d\n"
             << "**************************************\n";

    // set up image with anisotropic voxels
    int nx=7;
    int ny=11;
    int nz=13;
    vimt3d_image_3d_of<vxl_int_32> image(nx,ny,nz);
    double vx=0.1;
    double vy=0.2;
    double vz=0.3;
    vimt3d_transform_3d w2i, i2w;
    w2i.set_zoom_only(1/vx,1/vy,1/vz,0,0,0);
    image.set_world2im(w2i);
    i2w=w2i.inverse();
    image.image().fill(0);

    // calculate running total of intensities
    // IGNORING THE EDGE PIXELS (i.e. i==0 || i==n-1 etc)
    double running_total=0;

    for (int i=0;i<nx;++i)
    {
        for (int j=0;j<ny;++j)
        {
            for (int k=0;k<nz;++k)
            {
                if (i>0&&j>0&&k>0 && i<nx-1&&j<ny-1&&k<nz-1)
                {
                    int val=i+j*nx+k*nx*ny;
                    image.image()(i,j,k)=val;
                    running_total += val;
                }
            }
        }
    }

    // sample image as grid
    vgl_point_3d<double> ip(1,1,1);
    vgl_point_3d<double> p=i2w(ip);

    vgl_vector_3d<double> u(vx,0,0);
    vgl_vector_3d<double> v(0,vy,0);
    vgl_vector_3d<double> w(0,0,vz);

    {
        vnl_vector<double> sample;
        vimt3d_sample_grid_trilin(sample,image,p,u,v,w,nx-1,ny-1,nz-1);
        double diff= vcl_fabs(sample.sum()-running_total);
        TEST("Grid sample == explicitly calculated sample 1",bool(diff<1e-6),true);

        // try writing the grid sample onto a blank image
        vimt3d_image_3d_of<vxl_int_32> recon_image;
        recon_image.deep_copy(image);
        recon_image.image().fill(0);
        vimt3d_reconstruct_from_grid(recon_image,sample,p,u,v,w,nx-1,ny-1,nz-1,false);
        compare_images(image,recon_image);
    }
    // try in a different order
    {
        vnl_vector<double> sample;
        vimt3d_sample_grid_trilin(sample,image,p,v,u,w,ny-1,nx-1,nz-1);
        double diff= vcl_fabs(sample.sum()-running_total);
        TEST("Grid sample == explicitly calculated sample 2",bool(diff<1e-6),true);

        vimt3d_image_3d_of<vxl_int_32> recon_image;
        recon_image.deep_copy(image);
        recon_image.image().fill(0);
        vimt3d_reconstruct_from_grid(recon_image,sample,p,v,u,w,ny-1,nx-1,nz-1,true);
        compare_images(image,recon_image);
    }
    // try in a different order
    {
        vnl_vector<double> sample;
        vimt3d_sample_grid_trilin(sample,image,p,v,w,u,ny-1,nz-1,nx-1);
        double diff= vcl_fabs(sample.sum()-running_total);
        TEST("Grid sample == explicitly calculated sample 3",bool(diff<1e-6),true);

        vimt3d_image_3d_of<vxl_int_32> recon_image;
        recon_image.deep_copy(image);
        recon_image.image().fill(0);
        vimt3d_reconstruct_from_grid(recon_image,sample,p,v,w,u,ny-1,nz-1,nx-1,false);
        compare_images(image,recon_image);
    }
}

TESTMAIN(test_sample_grid_trilin);
