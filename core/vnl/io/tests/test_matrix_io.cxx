#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include <vnl/io/vnl_io_matrix.h>

void test_matrix_double_io()
{
    vcl_cout << "***********************" << vcl_endl;
    vcl_cout << "Testing vnl_matrix<double> io" << vcl_endl;
    vcl_cout << "***********************" << vcl_endl;  
    //// test constructors, accessors
    const int m = 10;
    const int n = 6;
    vnl_matrix<double> m_out(m, n), m_in;

    for (int i=0; i<m; i++)
    {
      for (int j=0; j<n; j++)
      {
          m_out(i,j) = (double)(i*j+i);
    }
    }
    
    
    
    vsl_b_ofstream bfs_out("vnl_matrix_test_double_io.bvl.tmp");
    TEST ("Created vnl_matrix_test_double_io.bvl.tmp for writing",
             (!bfs_out), false);
    vsl_b_write(bfs_out, m_out);
    bfs_out.close();        
        
    vsl_b_ifstream bfs_in("vnl_matrix_test_double_io.bvl.tmp");
    TEST ("Opened vnl_matrix_test_double_io.bvl.tmp for reading", (!bfs_in),
             false);
    vsl_b_read(bfs_in, m_in);
    bfs_in.close();     
        
    
    
    TEST ("m_out == m_in", m_out == m_in, true);

  vsl_print_summary(vcl_cout, m_out);
  vcl_cout << vcl_endl;

  
}




   
void test_matrix_prime()
{
  test_matrix_double_io();
}


TESTMAIN(test_matrix_prime);
