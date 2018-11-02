#include <cstdio>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

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

static void test_bil_cedt()
{
  std::cout << "Contour Euclidean Distance Transform Algorithms\n";

  unsigned r=10,c=12;
  vil_image_view <unsigned char> image(r,c,1);

  image.fill(255);

  for (int i=0;i<5;i++)
    generate_random_line(image,i);
  for (int i=0;i<5;i++)
    generate_random_circle(image,i);

  std::cout << "ORIGINAL IMAGE:\n";
  vil_print_all(std::cout,image);

  bil_cedt_test(image,true);
}

void
bil_cedt_test(vil_image_view<unsigned char> &im, bool /*print*/)
{
  bil_cedt cedt(im);
  cedt.compute_cedt();

  vil_image_view<float> cedtim=cedt.cedtimg();
  vil_image_view<float> bruteforcedtim=bil_cedt_brute_force(im);
  std::cout << "CEDT:\n";
  vil_print_all(std::cout,cedtim);

  std::cout << "Brute Force:\n";
  vil_print_all(std::cout,bruteforcedtim);

  bool cedt_error=false;

  for (unsigned i=0; i<im.size(); ++i) {
    float dst,cedtdst;
    dst = DATA(bruteforcedtim)[i];
    cedtdst=DATA(cedtim)[i];
    if (std::fabs(dst -cedtdst)>0.1) {
      std::printf("Error. CEDT: %g BF: %g\n",DATA(cedtim)[i],dst);
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

  for (unsigned int arrayj=0;arrayj<dist.nj();arrayj++)
  {
    for (unsigned int arrayi=0;arrayi<dist.ni();arrayi++)
    {
      float dst=1e34f; // will become min dist, so initialise with high value
      for (unsigned int i=0;i<im.ni();i++)
      {
        for (unsigned int j=0;j< im.nj();j++)
        {
          if (im(i,j)==0)
          {
            float temp=std::sqrt((float)((i-arrayi)*(i-arrayi)+(j-arrayj)*(j-arrayj)));
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
  for (unsigned i=0; i<im.ni(); ++i)
  {
    int j=(int)((i-x0)*std::tan(theta)+y0);

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
  int radius=rnd.lrand32(0, (int)std::min(im.nj(),im.ni())/2);

  int imniunsigned = static_cast<int>(im.ni());
  assert(imniunsigned>=0);

  for (int i=0;i<imniunsigned;i++)
  {
    auto discriminant= (float)(radius*radius-(i-x0)*(i-x0));
    if (discriminant>0)
    {
      int j1=x0+(int)std::sqrt(discriminant);
      int j2=x0-(int)std::sqrt(discriminant);

      int imnjunsigned = static_cast<int>(im.nj());
      assert(imnjunsigned>=0);
      if (j1>=0 && j1<imnjunsigned)
        im(i,j1)=0;

      if (j2>=0 && j2<imnjunsigned)
        im(i,j2)=0;
    }
  }
  return true;
}

TESTMAIN(test_bil_cedt);
