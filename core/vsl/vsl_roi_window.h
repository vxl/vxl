#ifndef vsl_roi_window_h_
#define vsl_roi_window_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vsl_roi_window
// .INCLUDE vsl/vsl_roi_window.h
// .FILE vsl_roi_window.cxx
// \author fsm@robots.ox.ac.uk

// some sort of bounding box.
struct vsl_roi_window {
  int col_end_index;
  int col_start_index;
  int row_end_index;
  int row_start_index;
};
#endif // vsl_roi_window_h_
