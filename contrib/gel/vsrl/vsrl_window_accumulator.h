#ifndef vsrl_window_accumulator_h
#define vsrl_window_accumulator_h

//:
// \file
// this program will allow for efficient accumulation for a
// rectangular window that goes across an array

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

class vsrl_window_accumulator
{
 public:

  // constructors
  vsrl_window_accumulator();

  // destructors

  ~vsrl_window_accumulator();

  // accessors

  void set_in_matrix(vnl_matrix<double> *in_mat); // the initial values
  void set_out_matrix(vnl_matrix<double> *out_mat); // the resulting accumulation

  void set_window_width(int width); // the width of the accumulation window
  void set_window_height(int height); // the height of the accumulation window

  // run the accumulation

  void execute();

 private:

  vnl_matrix<double> *in_mat_; // the current matrix
  vnl_matrix<double> *out_mat_; // the new matrix

  unsigned int win_width_; // the window width
  unsigned int win_height_; // the window height

  int current_row_; // the current row that is being processed
  int current_col_; // the current column the is being processed

  // internal methods

  // methods used to get sums of columns;

  void initial_column_sums(vnl_vector<double> &vec);
  void next_column_sums(vnl_vector<double> &vec);

  // methods to get the accumulation

  double initial_accumulation(vnl_vector<double> &vec);
  double next_accumulation(vnl_vector<double> &vec, double old_sum);

  // method to get the number of samples for a given window

  double num_samples(int r,int c);
};

#endif // vsrl_window_accumulator_h
