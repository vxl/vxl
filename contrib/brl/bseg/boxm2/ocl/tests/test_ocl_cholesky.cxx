#include <iostream>
#include <vcl_where_root_dir.h>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_random.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/tests/boxm2_ocl_test_utils.h>

void compute_ocl_cholesky(const vnl_matrix<float>&  A, vnl_vector<float>  b, const vnl_vector<float>&  x0, vnl_vector<float>  & x)
{
  //load BOCL stuff
  bocl_manager_child &mgr = bocl_manager_child::instance();
  if (mgr.gpus_.size()==0)   return;
  bocl_device_sptr device = mgr.gpus_[0];

  std::cout<<device->info();

  vnl_matrix<float> Acopy = A;
  //compile pyramid test
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "onl/cholesky_decomposition.cl");
  src_paths.push_back(source_dir + "onl/test_onl_cholesky.cl");

  bocl_kernel cholesky_test;
  cholesky_test.create_kernel(&device->context(),device->device_id(), src_paths, "test_onl_cholesky", "", "test cholesky decomposition");

  bocl_kernel cholesky_solve_test;
  cholesky_solve_test.create_kernel(&device->context(),device->device_id(), src_paths, "test_onl_cholesky_solve", "", "test cholesky decomposition solver");

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  //create all the arguments
  bocl_mem_sptr matbuff = new bocl_mem( device->context(), Acopy.data_block(), 16*sizeof(float), "input matrix");
  matbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  float odata[16];
  bocl_mem_sptr invmatbuff = new bocl_mem( device->context(), odata, 16*sizeof(float), "inverse input matrix");
  invmatbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  int  n = 4;
  bocl_mem_sptr nbuff = new bocl_mem( device->context(), &n, sizeof(int), "dimension of square matrix");
  nbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  //set workspace
  std::size_t lThreads[] = {16};
  std::size_t gThreads[] = {16};
  //set first kernel args
  cholesky_test.set_arg( matbuff.ptr() );
  cholesky_test.set_arg( invmatbuff.ptr() );
  cholesky_test.set_arg( nbuff.ptr() );
  cholesky_test.set_local_arg( lThreads[0]*sizeof(float) );//local tree,
  //execute kernel
  cholesky_test.execute( queue, 1, lThreads, gThreads);
  clFinish( queue );
  invmatbuff->read_to_buffer(queue);

  vnl_matrix<float> AL( A.rows(), A.cols());
  AL.fill(0.0);
  std::cout<<"====== L Matrix  ======"<<std::endl;
  for (unsigned i = 0 ; i < Acopy.rows(); i ++)
    for (unsigned j = 0 ; j <= i ; j ++)
      AL(i,j) = odata[i*Acopy.cols() + j];
  AL = AL * AL. transpose();
  testlib_test_assert_near("ONL CHOLESKY L*L'-A", (AL - A).fro_norm(),0,1e-5);

  // testing solver part
  Acopy = A;
  bocl_mem_sptr Abuff = new bocl_mem( device->context(), Acopy.data_block(), 16*sizeof(float), "input matrix");
  Abuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr bbuff = new bocl_mem( device->context(), b.data_block(), 4*sizeof(float), "b of Ax = b");
  bbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr xbuff = new bocl_mem( device->context(), x.data_block(), 4*sizeof(float), "x of Ax = b");
  xbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  cholesky_solve_test.set_arg( Abuff.ptr() );
  cholesky_solve_test.set_arg( bbuff.ptr() );
  cholesky_solve_test.set_arg( xbuff.ptr() );
  cholesky_solve_test.set_arg( nbuff.ptr() );
  cholesky_solve_test.set_local_arg( lThreads[0]*sizeof(float) );//local tree,
  cholesky_solve_test.set_local_arg( b.size()*sizeof(float) );//local tree,
  cholesky_solve_test.set_local_arg( x.size()*sizeof(float) );//local tree,
  status = cholesky_solve_test.execute( queue, 1, lThreads, gThreads);
  clFinish( queue );
  xbuff->read_to_buffer(queue);
  testlib_test_assert_near("ONL CHOLESKY SOLVE x- x0", (x - x0).two_norm(),0,1e-3);
}

void compute_vnl_cholesky(vnl_matrix<double> & A, const vnl_vector<double>&  b, const vnl_vector<double>&  x0, vnl_vector<double>  & x)
{
  vnl_matrix<double> I(4,4);
  I.set_identity();
  {
    vnl_cholesky chol(A);
    testlib_test_assert_near("vnl_inverse() ~= cholesky.inverse()",
                             (chol.inverse() - vnl_inverse(A)).fro_norm());
  }
  {
    vnl_cholesky chol(A);
    testlib_test_assert_near("(VNL)Ai * A - I", (chol.inverse() * A - I).fro_norm());
    testlib_test_assert_near("(VNL)Ai * A - I", (A * chol.inverse() - I).fro_norm());
  }
  {
    vnl_cholesky chol(A, vnl_cholesky::estimate_condition);
    testlib_test_assert_near("(VNL) Ai * A - I", (chol.inverse() * A - I).fro_norm());
    testlib_test_assert_near("(VNL) Ai * A - I", (A * chol.inverse() - I).fro_norm());
  }
  {
    vnl_cholesky chol(A);
    x=chol.solve(b);
    testlib_test_assert_near("Solve Ax=b",(x-x0).one_norm(),0,1e-4);
  }
}

void test_ocl_cholesky()
{
  vnl_random rng(1000);
  vnl_matrix<double> A(4,4);
  boxm2_ocl_test_utils::fill_random(A.begin(), A.end(), rng);
  A = A * A.transpose();

  vnl_matrix<float> Af(4,4);
  for (unsigned i = 0 ; i < A.rows(); i ++)
    for (unsigned j = 0 ; j < A.cols(); j++)
      Af(i,j) = (float)A(i,j);

  vnl_vector<float> bf(4),x0f(4),xf(4);
  boxm2_ocl_test_utils::fill_random(x0f.begin(), x0f.end(), rng);
  bf=Af*x0f;

  vnl_vector<double> b(4),x0(4),x(4);

  for (unsigned i = 0 ; i < b.size(); i++)
  {
    b[i] = bf[i] ;
    x0[i] = x0f[i] ;
    x[i] = xf[i] ;
  }
  compute_ocl_cholesky(Af,bf,x0f,xf);
  compute_vnl_cholesky(A,b,x0,x);
}

TESTMAIN(test_ocl_cholesky);
