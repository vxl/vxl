#ifndef vil_to_mil_h_
#define vil_to_mil_h_

#include <vil/vil_image.h>
#include <mil/mil_convert_vil.h>

inline mil_image* vil_to_mil(vil_image const& im)
{
  if (im.planes() == 1) {
    int cmpts = im.components();
    int bpp = cmpts * im.bits_per_component();
    int width = im.width();
    int height = im.height();

    char* buf = new char[im.get_size_bytes()];
    im.get_section(buf, 0, 0, width, height);
  
    mil_image* imo;
    if (im.component_format() == VIL_COMPONENT_FORMAT_UNSIGNED_INT) {
      if (bpp == 8) {
        mil_image_2d_of<vil_byte>* imt = new mil_image_2d_of<vil_byte>(width, height);
        imt->setGrey((vil_byte*)buf, width, height, width);
        imo = imt;
      }
#if 0
      else if (bpp == 8*sizeof(int)) {
        mil_image_2d_of<unsigned int>* imt = new mil_image_2d_of<unsigned int>(width, height);
        imt->setGrey((unsigned int*)buf, width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(short)) {
        mil_image_2d_of<unsigned short>* imt = new mil_image_2d_of<unsigned short>(width, height);
        imt->setGrey((unsigned short*)buf, width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(long)) {
        mil_image_2d_of<unsigned long>* imt = new mil_image_2d_of<unsigned long>(width, height);
        imt->setGrey((unsigned long*)buf, width, height, width);
        imo = imt;
      }
#endif
    }
#if 0
    else if (im.component_format() == VIL_COMPONENT_FORMAT_SIGNED_INT) {
      if (bpp == 8*sizeof(int)) {
        mil_image_2d_of<int>* imt = new mil_image_2d_of<int>(width, height);
        imt->setGrey((int*)buf, width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(short)) {
        mil_image_2d_of<short>* imt = new mil_image_2d_of<short>(width, height);
        imt->setGrey((short*)buf, width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(long)) {
        mil_image_2d_of<long>* imt = new mil_image_2d_of<long>(width, height);
        imt->setGrey((long*)buf, width, height, width);
        imo = imt;
      }
      else if (bpp == 8) {
        mil_image_2d_of<signed char>* imt = new mil_image_2d_of<signed char>(width, height);
        imt->setGrey((signed char*)buf, width, height, width);
        imo = imt;
      }
    }
#endif
    else if (im.component_format() == VIL_COMPONENT_FORMAT_IEEE_FLOAT) {
      if (bpp == 8*sizeof(float)) {
        mil_image_2d_of<float>* imt = new mil_image_2d_of<float>(width, height);
        imt->setGrey((float*)buf, width, height, width);
        imo = imt;
      }
#if 0
      else if (bpp == 8*sizeof(double)) {
        mil_image_2d_of<double>* imt = new mil_image_2d_of<double>(width, height);
        imt->setGrey((double*)buf, width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(long double)) {
        mil_image_2d_of<long double>* imt = new mil_image_2d_of<long double>(width, height);
        imt->setGrey((long double*)buf, width, height, width);
        imo = imt;
      }
#endif
    }
    delete[] buf;
    return imo;
  }
  else if (im.planes() == 3) {
    int cmpts = im.components();
    int bpp = cmpts * im.bits_per_component();
    int width = im.width();
    int height = im.height();

    int sz = im.get_size_bytes();
    char* buf = new char[sz];
    im.get_section(buf, 0, 0, width, height);
  
    mil_image* imo;
    if (im.component_format() == VIL_COMPONENT_FORMAT_UNSIGNED_INT) {
      if (bpp == 8) {
        mil_image_2d_of<vil_byte>* imt = new mil_image_2d_of<vil_byte>(width, height, 3);
        imt->setRGB((vil_byte*)buf, (vil_byte*)(buf+(sz/3)), (vil_byte*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
#if 0
      else if (bpp == 8*sizeof(int)) {
        mil_image_2d_of<unsigned int>* imt = new mil_image_2d_of<unsigned int>(width, height, 3);
        imt->setRGB((unsigned int*)buf, (unsigned int*)(buf+(sz/3)), (unsigned int*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(short)) {
        mil_image_2d_of<unsigned short>* imt = new mil_image_2d_of<unsigned short>(width, height, 3);
        imt->setRGB((unsigned short*)buf, (unsigned short*)(buf+(sz/3)), (unsigned short*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(long)) {
        mil_image_2d_of<unsigned long>* imt = new mil_image_2d_of<unsigned long>(width, height, 3);
        imt->setRGB((unsigned long*)buf, (unsigned long*)(buf+(sz/3)), (unsigned long*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
#endif
    }
#if 0
    else if (im.component_format() == VIL_COMPONENT_FORMAT_SIGNED_INT) {
      if (bpp == 8*sizeof(int)) {
        mil_image_2d_of<int>* imt = new mil_image_2d_of<int>(width, height, 3);
        imt->setRGB((int*)buf, (int*)(buf+(sz/3)), (int*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(short)) {
        mil_image_2d_of<short>* imt = new mil_image_2d_of<short>(width, height, 3);
        imt->setRGB((short*)buf, (short*)(buf+(sz/3)), (short*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(long)) {
        mil_image_2d_of<long>* imt = new mil_image_2d_of<long>(width, height, 3);
        imt->setRGB((long*)buf, (long*)(buf+(sz/3)), (long*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
      else if (bpp == 8) {
        mil_image_2d_of<signed char>* imt = new mil_image_2d_of<signed char>(width, height, 3);
        imt->setRGB((signed char*)buf, (signed char*)(buf+(sz/3)), (signed char*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
    }
#endif
    else if (im.component_format() == VIL_COMPONENT_FORMAT_IEEE_FLOAT) {
      if (bpp == 8*sizeof(float)) {
        mil_image_2d_of<float>* imt = new mil_image_2d_of<float>(width, height, 3);
        imt->setRGB((float*)buf, (float*)(buf+(sz/3)), (float*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
#if 0
      else if (bpp == 8*sizeof(double)) {
        mil_image_2d_of<double>* imt = new mil_image_2d_of<double>(width, height, 3);
        imt->setRGB((double*)buf, (double*)(buf+(sz/3)), (double*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
      else if (bpp == 8*sizeof(long double)) {
        mil_image_2d_of<long double>* imt = new mil_image_2d_of<long double>(width, height, 3);
        imt->setRGB((long double*)buf, (long double*)(buf+(sz/3)), (long double*)(buf+2*(sz/3)), width, height, width);
        imo = imt;
      }
#endif
    }
  
    delete[] buf;
    return imo;
  }
  else assert(!"Can only convert 1-plane and 3-plane vil_images");
  return 0;
}

#endif // vil_to_mil_h_
