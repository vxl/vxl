#ifndef vsrl_window_accumulator_h
#define vsrl_window_accumulator_h

// this program will allow for efficient accumulation for a 
// rectangular window that goes accross an array

#include <vil/vil_memory_image_of.h>
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
 
  
  vnl_matrix<double> *_in_mat; // the current matrix
  vnl_matrix<double> *_out_mat; // the new matrix

  int _win_width; // the window width 
  int _win_height; // the window height
  
  int _current_row; // the current row that is being processed
  int _current_col; // the current column the is being processed

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
  
#endif
