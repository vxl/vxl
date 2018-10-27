#include "bocl_command_queue_mgr.h"
#include <vcl_where_root_dir.h>
#include <vul/vul_timer.h>

bool bocl_command_queue_mgr::init_kernel()
{
  memLength_ = 1024*1024*50;
  memHalf_ = memLength_/2;

  //set up kernels
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bbas/bocl/tests/";
  src_paths.push_back(source_dir + "test_command_queue.cl");
  kernel_a_.create_kernel(  &this->context(),
                            &this->devices()[0],
                            src_paths,
                            "test_command_queue",     //kernel name
                            "",                       //options
                            "test command queue a");  //kernel identifier (for error checking)
  kernel_b_.create_kernel(  &this->context(),
                            &this->devices()[0],
                            src_paths,
                            "test_command_queue",     //kernel name
                            "",                       //options
                            "test command queue b");  //kernel identifier (for error checking)
  for (auto & kernel : kernels_) {
    kernel.create_kernel(&this->context(),
                              &this->devices()[0],
                              src_paths,
                              "test_command_queue",   //kernel name
                              "",                     //options
                              "test command queue");  //kernel identifier (for error checking)
  }


  // set up both cmd queue
  int status;
  queue_a_ = clCreateCommandQueue(this->context(),
                                  this->devices()[0],
                                  CL_QUEUE_PROFILING_ENABLE,
                                  &status);
  if (!check_val(status,CL_SUCCESS,"Failed in command queue a creation" + error_to_string(status)))
    return false;

  queue_b_ = clCreateCommandQueue(this->context(),
                                  this->devices()[0],
                                  CL_QUEUE_PROFILING_ENABLE,
                                  &status);
  if (!check_val(status,CL_SUCCESS,"Failed in command queue b creation" + error_to_string(status)))
    return false;

  for (auto & queue : queues_) {
    queue = clCreateCommandQueue(this->context(),
                                      this->devices()[0],
                                      CL_QUEUE_PROFILING_ENABLE,
                                      &status);
    if (!check_val(status,CL_SUCCESS,"Failed in command queue creation" + error_to_string(status)))
      return false;
  }

  //set up pinned memory
  auto* in = new float[memLength_];
  auto* out = new float[memLength_];
  for (int i=0; i<memLength_; i++)
    in[i] = (float) i;

  pinned_in_ = new bocl_mem(this->context(), in, memLength_*sizeof(float), "pinned in buffer");
  pinned_in_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR);
  pinned_out_ = new bocl_mem(this->context(), out, memLength_*sizeof(float), "pinned out buffer");
  pinned_out_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR);

  //map standard pointers to pinned memory
  auto* pinned_in = (float*) clEnqueueMapBuffer(queue_a_, pinned_in_->buffer(), CL_TRUE,
                                            CL_MAP_WRITE, 0, memLength_*sizeof(float), 0,
                                            nullptr, nullptr, nullptr);
  auto* pinned_out = (float*) clEnqueueMapBuffer(queue_a_, pinned_out_->buffer(), CL_TRUE,
                                            CL_MAP_READ, 0, memLength_*sizeof(float), 0,
                                            nullptr, nullptr, nullptr);
  pinned_in_->set_cpu_buffer(pinned_in);
  pinned_out_->set_cpu_buffer(pinned_out);

  return true;
}

// NEED TO FIGURE OUT HOW TO differentiate between SCENE/BLOCK/DATA arguments and Camera/Image arguments
// the scene level stuff needs to live on the processor, other
bool bocl_command_queue_mgr::test_async_command_queue()
{
  //create start boclmem
  int* start = new int[1]; start[0] = 0;
  bocl_mem* offset = new bocl_mem(this->context(), start, sizeof(int), "offset zero buffer");
  offset->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  std::size_t lThreads[] = {8, 8};
  std::size_t gThreads[] = {1024, 1024};

  //3. EXECUTE once because opencl overhead tacks on a few milliseconds
  pinned_in_->write_to_buffer( queue_a_ );
  kernel_a_.set_arg( pinned_in_ );
  kernel_a_.set_arg( pinned_out_);
  kernel_a_.set_arg( offset );
  kernel_a_.execute( queue_a_, 2, lThreads, gThreads);
  pinned_out_->read_to_buffer( queue_a_ );
  clFinish(queue_a_);

  bool good = true;
  auto* pout = (float*) pinned_out_->cpu_buffer();
  for (int i=0; i<100; i++) {
    if (i*i != pout[i]) {
      good = false;
      break;
    }
  }
  std::cout<<"kernel calculated squres: "<<good<<std::endl;
  //////////////////////////////////////////////////////////////////////////////
  //4. execute 50 trials (warm up GPU)
  //////////////////////////////////////////////////////////////////////////////
  std::cout<<"--------------------------------------\n"
          <<"EXECUTING SERIAL KERNEL/WRITE\n"
          <<"--------------------------------------"<<std::endl;
  pinned_in_->write_to_buffer( queue_a_ );
  vul_timer t;
  int numTrials = 50;
  for (int i=0; i<numTrials; i++)
  {
    //execute kernel and TIME
    kernel_a_.execute( queue_a_, 2, lThreads, gThreads);
    pinned_in_->write_to_buffer( queue_a_ );
    clFinish(queue_a_);
  }
  std::cout<<"One Queue WALL CLOCK TIME: "<<t.all()/numTrials<<" ms\n"
          <<"Test kernel time: "<<kernel_a_.exec_time()<<" ms\n"
          <<"Test write buffer time: "<<pinned_in_->exec_time()<<" ms"<<std::endl;
  pinned_out_->read_to_buffer( queue_a_ );
  clFinish(queue_a_);

  //store result to verify
  auto* control = new float[memLength_];
  auto* out = (float*) pinned_out_->cpu_buffer();
  for (int i=0; i<memLength_; i++)
    control[i] = out[i];

  //////////////////////////////////////////////////////////////////////////////
  //do 100 trials overlapping
  //////////////////////////////////////////////////////////////////////////////
  std::cout<<"--------------------------------------\n"
          <<"EXECUTING OVERLAPPING KERNEL/WRITE\n"
          <<"--------------------------------------"<<std::endl;
  //create start boclmem
  int incr = memLength_/NUM_QUEUES;
  for (int i=0; i<NUM_QUEUES; i++)
  {
    int* off = new int[1]; off[0] = i*incr;
    offsets_[i] = new bocl_mem(this->context(), off, sizeof(int), "offset buffer, i");
    offsets_[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  }

  lThreads[0] = 8; lThreads[1] = 8;
  gThreads[0] = 1024; gThreads[1] = 1024/NUM_QUEUES;
  t.mark();
  for (int i=0; i<numTrials; i++)
  {
    for (int k=0; k<NUM_QUEUES; k++)
    {
      int next = (k+1) % NUM_QUEUES;

      //launch kernel computation queue A (on buffer A)
      kernels_[k].set_arg( pinned_in_ );
      kernels_[k].set_arg( pinned_out_ );
      kernels_[k].set_arg( offsets_[k] );
      kernels_[k].execute( queues_[k], 2, lThreads, gThreads);

      // non blocking write of buffer B (on Queue B)
      int off = next * incr * sizeof(float);
      auto* buff = (float*) pinned_in_->cpu_buffer();
      clEnqueueWriteBuffer( queues_[next], pinned_in_->buffer(),
                            CL_FALSE, off, incr*sizeof(float),
                            (void*) &buff[memHalf_], 0, nullptr, nullptr);
      kernels_[k].clear_args();
    }
  }
  for (auto & queue : queues_) clFinish(queue);
  std::cout<<"WALL CLOCK TIME: "<<t.all()/numTrials<<" ms\n"
          <<"Test kernel time: "<<kernel_a_.exec_time()<<" ms\n"
          <<"Test write buffer time: "<<pinned_in_->exec_time()<<" ms"<<std::endl;

  //VERIFY OUTPUT
  out = (float*) pinned_out_->cpu_buffer();
  for (int i=0; i<memLength_; i++) {
    if (control[i] != out[i]) {
      std::cout<<"CONTROL: "<<control[i]<<" != OUT: "<<out[i]<<std::endl;
      break;
    }
  }

  //delete buffer_a_;
  //delete buffer_b_;

  return true;
}
