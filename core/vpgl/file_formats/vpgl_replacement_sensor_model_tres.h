// This is core/vpgl/file_formats/vpgl_replacement_sensor_model_tres.h
#ifndef vpgl_replacement_sensor_model_tres_h_
#define vpgl_replacement_sensor_model_tres_h_
//:
// \file
// \brief: Define nitf2.1 tagged record extenstions for the replacement sensor model (RSM)
// \author J. L. Mundy
// \date October 2023
//
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class vpgl_replacement_sensor_model_tres{
 public:
  static void define_RSMPIA();
  static void define_RSMGIA();
  static void define_RSMIDA();
  static void define_RSMPCA();
  static void define() {
      define_RSMPIA();
      define_RSMGIA();
      define_RSMIDA();
      define_RSMPCA();
  }
  private:
    vpgl_replacement_sensor_model_tres();
    ~vpgl_replacement_sensor_model_tres();
};
        

#endif // vpgl_replacement_sensor_model_tres_h_
