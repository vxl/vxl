// This is vxl/vnl/io/tests/golden_test_vbl_io.cxx

#include <vsl/vsl_binary_io.h>

#include <vbl/io/vbl_io_array_1d.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <vbl/io/vbl_io_array_3d.h>
#include <vbl/io/vbl_io_user_info.h>

#include <vbl/io/tests/vbl_io_test_classes.cxx>


#include <vbl/io/vbl_io_smart_ptr.h>


//#include <vbl/io/vbl_io_sparse_array.h>
#include <vbl/io/vbl_io_bounding_box.h>


#include <vcl/vcl_cassert.h>
#include <vcl/vcl_string.h>
#include <vcl/vcl_fstream.h>

void golden_test_vbl_io(bool save_file=false);

// This nasty macro stuff is to allow the program to be compiled as a stand
// alone program or as part of a larger test program.
#ifndef TESTMAIN
  #include <vbl/vbl_test.h>
  int main( int argc, char* argv[] )
  {
    vbl_test_start("golden_test_all_vbl_io");
    bool save_file=false;

    if (argc==2)
    {
      vcl_string conf = argv[1];
      vcl_string ref="create";
      if (conf==ref)
      {
        save_file =true;
      }
    }
    golden_test_vbl_io(save_file);
    return vbl_test_summary();
  }
#else
  #include <vbl/vbl_test.h>
#endif;


void golden_test_vbl_io(bool save_file)
{
  //------------------------------------------------------------------------
  // Create objects:
  // If the "create" flag was used on the command line, then the program saves
  // an example of each class. Otherwise it just fills them with values
  // for comparison to the values read in.
  //------------------------------------------------------------------------
  
  
  vcl_cout << "***********************" << vcl_endl;
  vcl_cout << "Testing a golden data file for cross platform consistency"
    << vcl_endl;
  vcl_cout << "***********************" << vcl_endl;
  
  
  
  
  
  
  // vbl_bounding_box
  vbl_bounding_box<double,2> b_box_in, b_box_out;
  
  b_box_out.update(1.2, 3.4); // Bounding box now has only one point and therefore no size
  b_box_out.update(5.6, 7.8); // Socond point now defines a bounding box
  
  // 1d Array
  int n = 50;
  vbl_array_1d<float> a1_out, a1_in;
  
  a1_out.reserve(n+10);
  for (int i=0; i<n; i++)
  {
    a1_out.push_back(i*i);
  }
  
  
  // 2d Array
  const int array_rows = 8;
  const int array_cols = 6;
  vbl_array_2d<int> a2_out(array_rows, array_cols), a2_in;
  
  for (int i=0; i<array_rows; i++)
  {
    for (int j=0; j< array_cols; j++)
      a2_out(i,j) = i*j*j;
  }
  
  
  // 3d Array
  const int array_row1 = 8;
  const int array_row2 = 7;
  const int array_row3 = 9;
  vbl_array_3d<int> a3_out(array_row1, array_row2, array_row3), a3_in;
  for (int i=0; i<array_row1; i++)
    for (int j=0; j< array_row2; j++)
      for (unsigned k=0; k<array_row3; k++)   
        a3_out(i,j,k) = i*j*j*k;
      
      
  // Sparse Array
/*  vbl_sparse_array<double> sa_out, sa_in;
  unsigned key1=3,key2=4,key3=5;
  double data1=1.2, data2=3.4, data3=5.6;
  
  //create a sparse array - more than 5 elements so only first 5 are written out
  sa_out[key1]=data1;
  sa_out[key2]=data2;
  sa_out[key3]=data3;
  for (unsigned k=60; k<70; k++)
    sa_out[k]=data1;
*/
  
/* This won't work on systems without a user called cbj!
  // User Info
  vcl_string name="cjb";
  vbl_user_info ui_out(name), ui_in("");
  ui_out.init("cjb");
*/

  
  // Smart Pointer
  n = 50;
  vbl_smart_ptr<impl > sp1_out(new impl(n));
  vbl_smart_ptr<impl> sp2_out(sp1_out);
  vbl_smart_ptr<impl > sp1_in, sp2_in;
  
  
  // Save if option set
  if (save_file)
  {
    vcl_cout << "Was save invoked ?" << vcl_endl;
    vsl_b_ofstream bfs_out("golden_test_vbl_io.bvl");
    if (!bfs_out)
    {
      vcl_cerr<<"Problems opening file for output"<<vcl_endl;
      exit(1);
    }
    vsl_b_write(bfs_out, b_box_out);
    vsl_b_write(bfs_out, a1_out);
    vsl_b_write(bfs_out, a2_out);	
    vsl_b_write(bfs_out, a3_out);
//    vsl_b_write(bfs_out, sa_out);
//    vsl_b_write(bfs_out, ui_out);
    vsl_b_write(bfs_out, sp1_out);
    vsl_b_write(bfs_out, sp2_out);
    
    bfs_out.close();
  }
  
  // Read in file to each class in turn
  
  vcl_cout << "Did we get this far ?" << vcl_endl;
  
  vsl_b_ifstream bfs_in("golden_test_vbl_io.bvl");
  
  
  
  TEST ("Opened golden_test_vbl_io.bvl for reading ", ! bfs_in, false);
  
  vsl_b_read(bfs_in, b_box_in);
  vsl_b_read(bfs_in, a1_in);	
  vsl_b_read(bfs_in, a2_in);	
  vsl_b_read(bfs_in, a3_in);
//  vsl_b_read(bfs_in, sa_in);
//  vsl_b_read(bfs_in, ui_in);
  vsl_b_read(bfs_in, sp1_in);
  vsl_b_read(bfs_in, sp2_in);
  
  bfs_in.close();
  
  
  // Test that each object created is the same as read in from the file.
  
  // Test bounding box
  TEST ("b_box_out.initialized_ == b_box_in.initialized_", b_box_out.initialized_ == b_box_in.initialized_, true);
  TEST ("b_box_out.min_[0] == b_box_in.min_[0]", b_box_out.min_[0] == b_box_in.min_[0], true);
  TEST ("b_box_out.min_[1] == b_box_in.min_[1]", b_box_out.min_[1] == b_box_in.min_[1], true);
  TEST ("b_box_out.max_[0] == b_box_in.max_[0]", b_box_out.max_[0] == b_box_in.max_[0], true);
  TEST ("b_box_out.max_[1] == b_box_in.max_[1]", b_box_out.max_[1] == b_box_in.max_[1], true);
  
  
  //Test 1d array
  bool test_result1 = true;
  if (a1_out.size() != a1_in.size())
    test_result1 = false;
  else if (a1_out.capacity() != a1_in.capacity())
    test_result1 = false;
  else
  {
    unsigned array_size = a1_out.size();
    for (unsigned i=0; i<array_size; i++)
    {
      if (a1_out[i] != a1_in[i])
        test_result1 = false;
    }
  }
  TEST ("a1_out == a1_in", test_result1, true);
  
  
  // Test 2d array
  bool test_result2 = true;
  if (a2_out.rows() != a2_in.rows())
    test_result2 = false;
  else if (a2_out.cols() != a2_in.cols())
    test_result2 = false;
  else
  {
    unsigned array_rows = a2_out.rows();
    unsigned array_cols = a2_out.cols();
    for (unsigned i=0; i<array_rows; i++)
    {
      for (unsigned j=0; j<array_cols; j++)
        if (a2_out(i,j) != a2_in(i,j))
          test_result2 = false;
    }
  }
  TEST ("a2_out == a2_in", test_result2, true);
  
  
  
  // Test 3d array
  bool test_result3 = true;
  if (a3_out.get_row1_count() != a3_in.get_row1_count())
    test_result3 = false;
  else if (a3_out.get_row2_count() != a3_in.get_row2_count())
    test_result3 = false;
  else if (a3_out.get_row3_count() != a3_in.get_row3_count())
    test_result3 = false;
  else
  {
    unsigned array_row1 = a3_out.get_row1_count();
    unsigned array_row2 = a3_out.get_row2_count();
    unsigned array_row3 = a3_out.get_row3_count();
    for (unsigned i=0; i<array_row1; i++)
      for (unsigned j=0; j<array_row2; j++)
        for (unsigned k=0; k<array_row3; k++)
          if (a3_out(i,j,k) != a3_in(i,j,k))
            test_result3 = false;
  }
  TEST ("a3_out == a3_in", test_result3, true);
  
  
/*  // Test Sparse Array
  bool test_result4 = true;
  //same number of non zero elements?
  if(sa_out.count_nonempty() != sa_in.count_nonempty())
    test_result4=false;
  else {
    //check every key/data pair, require same order too.
    vbl_sparse_array<double>::const_iterator s = sa_in.begin();
    //N.B. relies on sensible == operator for <T> 
    for(vbl_sparse_array<double>::const_iterator r = sa_out.begin(); r != sa_out.end(); ++r){
      if(((*s).first != (*r).first) || ((*s).second != (*r).second)) test_result4=false;
      s++;
    }
  }
  TEST ("sa_out == sa_in",test_result4, true);
*/

/*
  //Test User Info
  TEST ("ui_out == ui_in",ui_out.uid==ui_in.uid && ui_out.gid==ui_in.gid && ui_out.name==ui_in.name &&
    ui_out.home_directory==ui_in.home_directory && ui_out.full_name==ui_in.full_name &&
    ui_out.shell==ui_in.shell && ui_out.passwd==ui_in.passwd, true);
*/
  
  
  // Test Smart Pointer
  TEST ("sp1_in == sp2_in", sp1_in == sp2_in, true);
  TEST ("sp1_in->get_references() == 2", sp1_in->get_references() ==2, true);
  
  
  
  
  
  return;
}
