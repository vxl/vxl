// This is brl/bseg/bmdl/pro/bmdl_classify_process.h
#ifndef bmdl_classify_process_h_
#define bmdl_classify_process_h_
//:
// \file
// \brief A class for classifying pixels using 2 lidar images.
//        -  Input:
//             - First return "vil_image_view_base_sptr"
//             - Last return  "vil_image_view_base_sptr"
//
//        -  Output:
//             - label image  "vil_image_view_base_sptr"
//             - height image "vil_image_view_base_sptr"
//
//        -  Params:
//
//
// \author  Gamze D. Tunali
// \date    Oct. 14, 2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <bprb/bprb_process.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

class bmdl_classify_process : public bprb_process
{
 public:

   bmdl_classify_process();

  //: Copy Constructor (no local data)
  bmdl_classify_process(const bmdl_classify_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){}

  ~bmdl_classify_process(){}

  //: Clone the process
  virtual bmdl_classify_process* clone() const
    { return new bmdl_classify_process(*this); }

  vcl_string name(){return "bmdlClassifyProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

 private:

   bool classify(vil_image_view_base_sptr lidar_first,
                 vil_image_view_base_sptr lidar_last,
                 vil_image_view_base_sptr ground,
                 vil_image_view_base_sptr& label_img,
                 vil_image_view_base_sptr& height_img);

  template <class T>
  bool classify(const vil_image_view<T>& lidar_first,
                const vil_image_view<T>& lidar_last,
                const vil_image_view<T>& ground,
                vil_image_view<unsigned int>& label_img,
                vil_image_view<T>& height_img);
};

#endif // bmdl_classify_process_h_
