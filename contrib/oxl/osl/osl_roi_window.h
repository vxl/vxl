// This is oxl/osl/osl_roi_window.h
#ifndef osl_roi_window_h_
#define osl_roi_window_h_
//:
// \file
// \author fsm

//: some sort of bounding box.
struct osl_roi_window
{
  int col_end_index;
  int col_start_index;
  int row_end_index;
  int row_start_index;
};

#endif // osl_roi_window_h_
