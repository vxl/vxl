#ifndef bbgm_loader_h_
#define bbgm_loader_h_

//:
// \file
// \brief register binary loaders for bbgm_image_base
//
class bbgm_loader
{
 public:
  static void register_loaders();
 private:
  static bool registered_;//prevent multiple registration
};


#endif

