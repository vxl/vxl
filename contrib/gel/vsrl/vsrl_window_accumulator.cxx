// This is gel/vsrl/vsrl_window_accumulator.cxx
#include "vsrl_window_accumulator.h"
#include <vcl_cassert.h>
#include <vsrl/vsrl_parameters.h>

// constructor 

vsrl_window_accumulator::vsrl_window_accumulator()
{
  // initial values
  in_mat_=0;
  out_mat_=0;

  win_width_= (vsrl_parameters::instance()->correlation_window_width)/2; // probably 10
  win_height_= (vsrl_parameters::instance()->correlation_window_height)/2; // probably 10

  current_row_=0;
  current_col_=0;
}

// destructor
vsrl_window_accumulator::~vsrl_window_accumulator()
{
}

void vsrl_window_accumulator::set_in_matrix(vnl_matrix<double> *in_mat)
{
  // set the in matrix
  in_mat_=in_mat;
}


void vsrl_window_accumulator::set_out_matrix(vnl_matrix<double> *out_mat)
{
  // set the out matrix
  out_mat_=out_mat;
}


void vsrl_window_accumulator::set_window_width(int width)
{
  // the width of the accumulation window
  win_width_=width/2;
}

void vsrl_window_accumulator::set_window_height(int height)
{
  // the height of the accumulation window
  win_height_=height/2;
}


void vsrl_window_accumulator::execute()
{
  // run the accumulation 

  current_row_= 0-1;

  // the column sums 

  vnl_vector<double> vec(in_mat_->cols());
  double sum=0;
  double N;

  int num_row = in_mat_->rows()-1;
  int num_col = in_mat_->cols()-1;

  while (num_row > current_row_){

    // get the column sums 
    next_column_sums(vec);

    while (num_col > current_col_){

      // get the sum;
      sum = next_accumulation(vec,sum);

      // save the accumulation 
      N=num_samples(current_row_,current_col_);

      (*out_mat_)(current_row_, current_col_) = sum/N;
    }
  }
}


void vsrl_window_accumulator::initial_column_sums(vnl_vector<double> &vec)
{
  // we want to find the column sums for the first row of the matrix 
  // for the given window

  // set the current row and column to zero

  current_row_=0;
  current_col_=0-1;

  // find out how far to accumulate;

  unsigned int sum_length=win_height_;

  if (sum_length >= in_mat_->rows()){
    sum_length=in_mat_->rows()-1;
  }

  // initialize the vector of column sums 

  vec.fill(0.0);

  // start to accumulate

  for (unsigned int i=0;i<in_mat_->columns();i++)
  {
    double sum=0;
    for (unsigned int j=0;j<=sum_length;j++)
      sum+=(*in_mat_)(j,i);
    vec(i)=sum;
  }
}

void vsrl_window_accumulator::next_column_sums(vnl_vector<double> &vec)
{
  // using the column sums from the previous calculation, update the vec
  // by going to the next region

  if (current_row_ < 0){
    // initialize the column vectors
    initial_column_sums(vec);
    return;
  }

  current_col_=0-1;
  current_row_++;

  unsigned int add_row = current_row_ + win_height_;
  int delete_row= current_row_ - win_height_ -1;

  bool add_row_flag= add_row<in_mat_->rows();
  bool delete_row_flag= delete_row >=0;

  // update the vector sums 

  for (unsigned int i=0;i<in_mat_->cols();i++)
  {
    if (add_row_flag)
      vec(i)=vec(i) + (*in_mat_)(add_row,i);
    if (delete_row_flag)
      vec(i)=vec(i) - (*in_mat_)(delete_row,i);
  }
}

double vsrl_window_accumulator::initial_accumulation(vnl_vector<double> &vec)
{
  // perform an initial accumulation using the column sums

  current_col_=0;

  unsigned int sum_width=win_width_+1;
  if (sum_width > in_mat_->cols()){
    sum_width = in_mat_->cols();
  }

  double sum=0;
  for (unsigned int i=0;i< sum_width;i++){
    sum += vec(i);
  }

  return sum;
}

double vsrl_window_accumulator::next_accumulation(vnl_vector<double> &vec, double old_sum)
{
  if (current_col_ < 0){
    // initialize the accumulation
    return initial_accumulation(vec);
  }

  // update the accumulation using the old sum and the column sums 
  current_col_++;

  unsigned int add_col=current_col_ + win_width_;
  int delete_col= current_col_ - win_width_ -1;

  if (add_col<in_mat_->cols()){
    old_sum=old_sum + vec(add_col);
  }
  if (delete_col >=0){
    old_sum=old_sum - vec(delete_col);
  }

  return old_sum;
}


double vsrl_window_accumulator::num_samples(int r, int c)
{
  assert(r>=0 && c>=0);

  // we want to find the number of samples in the region r,c

  int l1 = ((unsigned int)c > win_width_) ? win_width_ : c;
  int l2 = in_mat_->cols() -1 -c;
  if (l2 > int(win_width_)) l2 = win_width_;

  double w = l1+l2+1;

  l1 = ((unsigned int)r > win_height_) ? win_height_ : r;
  l2 = in_mat_->rows() -1 -r;
  if (l2 > int(win_height_)) l2 = win_height_;

  double h = l1+l2+1;

  return w*h;
}

