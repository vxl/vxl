#include <testlib/testlib_test.h>

#include <vcl_cstdio.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>

#include <bil/algo/bil_detect_blur_scale.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>

bool generate_random_circle1(vil_image_view<unsigned char> &im,int seed)
{
    vnl_random rnd(seed);
    int x0 = rnd.lrand32(0, im.ni()-1);
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    int y0 = rnd.lrand32(0, im.nj()-1);
#endif
    int radius=rnd.lrand32(0, (int)vcl_min(im.nj(),im.ni())/2);

      int imniunsigned = static_cast<int>(im.ni());
      assert(imniunsigned>=0);


    for (int i=0;i<imniunsigned;i++)
    {
      float discriminant= (float)(radius*radius-(i-x0)*(i-x0));
      if (discriminant>0)
      {
        int j1=x0+(int)vcl_sqrt(discriminant);
        int j2=x0-(int)vcl_sqrt(discriminant);

        if (j1>=0 && j1< static_cast<int>(im.nj()) )
            im(i,j1)=0;

        if (j2>=0 && j2<static_cast<int>(im.nj()))
            im(i,j2)=0;
      }
    }
    return true;
}

static void test_bil_detect_blur_scale()
{
    vcl_cout << "Contour Euclidean Distance Transform Algorithms\n";

    unsigned r=50,c=60;
    vil_image_view <unsigned char> image(c,r,1);

    image.fill(255);

    for(unsigned i=0;i<r;i++)
        for(unsigned j=c/2+1;j<c;j++)
            image(j,i)=0;

    for(unsigned i=0;i<r;i++)
            image(c/2,i)=127;
    for(unsigned i=0;i<r;i++)
            image(c/2-1,i)=190;
    for(unsigned i=0;i<r;i++)
            image(c/2+1,i)=63;

    //vil_image_resource_sptr img=vil_load_image_resource("f:/visdt/imgs/gray00000.png");

    vil_image_resource_sptr img=vil_new_image_resource_of_view(image);

    float est_sigma=0.0;
    bil_detect_blur_scale(img,5,est_sigma);

    TEST("Sigma for blur kernel of the image passed ",1.5,est_sigma);
}

TESTMAIN(test_bil_detect_blur_scale);

