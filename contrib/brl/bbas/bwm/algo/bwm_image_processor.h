#ifndef bwm_image_proc_h_
#define bwm_image_proc_h_

#include <bgui/bgui_image_tableau.h>

class bwm_image_processor {
public:
  static void hist_plot(bgui_image_tableau_sptr img);

  static void intensity_profile(bgui_image_tableau_sptr img,
                                float start_col, float end_col, 
                                float start_row, float end_row);

  static void range_map(bgui_image_tableau_sptr img);
};

#endif