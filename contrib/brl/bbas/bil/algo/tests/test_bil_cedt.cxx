#include <testlib/testlib_test.h>

#include <vcl_cstdio.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>

#include <bil/algo/bil_cedt.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>

#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>

void bil_cedt_test(vil_image_view<unsigned char> &im, bool print);
vil_image_view<float> bil_cedt_brute_force(vil_image_view<unsigned char> &im);
bool generate_random_line(vil_image_view<unsigned char> &im, int seed);
bool generate_random_circle(vil_image_view<unsigned char> &im, int seed);


#define DATA(I) (I).top_left_ptr()

MAIN( test_bil_cedt )
{
  START ("Contour Euclidean Distance Transform Algorithms");

  {
  unsigned r=10,c=12;

  vil_image_view <unsigned char> image(r,c,1);

  image.fill(255);


  for (int i=0;i<5;i++)
  generate_random_line(image,i);
  for (int i=0;i<5;i++)
generate_random_circle(image,i);

  vcl_cout << "ORIGINAL IMAGE:\n" << vcl_endl;
  vil_print_all(vcl_cout,image);

  bil_cedt_test(image,true);
  }

  SUMMARY();
}

void
bil_cedt_test(vil_image_view<unsigned char> &im, bool print)
{
   print = false; //:< not used


   bil_cedt cedt(im);
   cedt.compute_cedt();


   vil_image_view<float> cedtim=cedt.cedtimg();
   vil_image_view<float> bruteforcedtim=bil_cedt_brute_force(im);
   vcl_cout << "CEDT:\n";
   vil_print_all(vcl_cout,cedtim);

   vcl_cout << "Brute Force:\n";
   vil_print_all(vcl_cout,bruteforcedtim);


   bool cedt_error=false;

   for (unsigned i=0; i<im.size(); ++i) {
      float dst,cedtdst;
      dst = DATA(bruteforcedtim)[i];
      cedtdst=DATA(cedtim)[i];
      if (vcl_fabs(dst -cedtdst)>0.1) {
         vcl_printf("Error. CEDT: %g BF: %g\n",DATA(cedtim)[i],dst);
         cedt_error = true;
         break;
      }
   }

   TEST("CEDT", cedt_error, false);
}

vil_image_view<float> bil_cedt_brute_force(vil_image_view<unsigned char> &im)
{
    vil_image_view<float> dist(im.ni(),im.nj());
    dist.fill(0.0);

#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
    float infty_=1e6;
#endif

    for (unsigned int arrayj=0;arrayj<dist.nj();arrayj++)
    {
     for (unsigned int arrayi=0;arrayi<dist.ni();arrayi++)
     {
         float dst=9999.0;
         for (unsigned int i=0;i<im.ni();i++)
         {
             for (unsigned int j=0;j< im.nj();j++)
             {
                 if (im(i,j)==0)
                 {
                     float temp=vcl_sqrt((float)((i-arrayi)*(i-arrayi)+(j-arrayj)*(j-arrayj)));
                     if (temp<dst)
                         dst=temp;
                 }
             }
         }
         dist(arrayi,arrayj)=dst;
     }
    }

    return dist;
}


bool generate_random_line(vil_image_view<unsigned char> &im, int seed)
{
    vnl_random rnd(seed);
    int x0 = rnd.lrand32(0, im.ni()-1);
    int y0 = rnd.lrand32(0, im.nj()-1);
    double theta=rnd.drand32(0, vnl_math::pi/2);
    for (unsigned i=0;i<im.ni();i++)
    {
      int j=(int)((i-x0)*vcl_tan(theta)+y0);

      int imnjunsigned = static_cast<int>(im.nj());
      assert(imnjunsigned >= 0);

      if (j>=0 && j<imnjunsigned)
          im(i,j)=0;
    }
    return true;
}

bool generate_random_circle(vil_image_view<unsigned char> &im,int seed)
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
