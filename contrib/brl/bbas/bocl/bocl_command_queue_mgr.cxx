#include "bocl_command_queue_mgr.h"
#include <vcl_where_root_dir.h>
#include <vul/vul_timer.h>

bool bocl_command_queue_mgr::init_kernel()
{   
  
  //set up buffers
  float* buffer = new float[1024*1024*50]; 
  buffer_ = new bocl_mem(this->context(), buffer, 1024*1024*50*sizeof(float), "test buffer");
  buffer_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float* wbuffer = new float[1024*1024*50];
  for(int i=0; i<1024*1024*50; i++) wbuffer[i] = (float) i;
  write_buffer_ = new bocl_mem(this->context(), wbuffer, 1024*1024*50*sizeof(float), "test write buffer"); 
  write_buffer_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set up kernel
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/bocl/tests/";
  src_paths.push_back(source_dir + "test_command_queue.cl"); 
  
  //have kernel construct itself using the context and device
  bool created =  test_kernel_.create_kernel(  &this->context(),             
                                               &this->devices()[0], 
                                                src_paths,            
                                                "test_command_queue",   //kernel name
                                                "",                     //options
                                                "test command queue");  //kernel identifier (for error checking)


  // set up cmd queue
  int status;
  command_queue_ = clCreateCommandQueue(this->context(), 
                                        this->devices()[0], 
                                        CL_QUEUE_PROFILING_ENABLE|CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, 
                                        &status);
  if (!check_val(status,CL_SUCCESS,"Failed in command queue [0] creation" + error_to_string(status)))
    return false;
    

  return true;
}

// NEED TO FIGURE OUT HOW TO differentiate between SCENE/BLOCK/DATA arguments and Camera/Image arguments
// the scene level stuff needs to live on the processor, other
bool bocl_command_queue_mgr::test_async_command_queue()
{
  //2. set workgroup size
  vcl_size_t lThreads[] = {8, 8}; 
  vcl_size_t gThreads[] = {1024, 1024}; 

  //3. EXECUTE once because opencl overhead tacks on a few milliseconds
  test_kernel_.set_arg( buffer_ );  
  test_kernel_.execute( command_queue_, lThreads, gThreads); 
  write_buffer_->write_to_buffer_async( command_queue_ );
  clFinish(command_queue_);
  
  //execute kernel and TIME
  test_kernel_.set_arg( buffer_ );  
  vul_timer t;
  test_kernel_.execute( command_queue_, lThreads, gThreads); 
  write_buffer_->write_to_buffer( command_queue_ );
  clFinish(command_queue_);
  vcl_cout<<"WALL CLOCK TIME: "<<t.all()<<" ms"<<vcl_endl;
  vcl_cout<<"Test kernel time: "<<test_kernel_.exec_time()<<" ms"<<vcl_endl;
  vcl_cout<<"Test write buffer time: "<<write_buffer_->exec_time()<<" ms"<<vcl_endl;
  
  //read output, do something, blah blah
  buffer_->read_to_buffer( command_queue_); 
  float* cpu_buff = (float*) buffer_->cpu_buffer(); 
  //for(int i=0; i<110; i++) vcl_cout<<cpu_buff[i]<<vcl_endl;
  
  delete buffer_;
  delete write_buffer_;
  
  return true;
}
