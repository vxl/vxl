#ifndef boxm2_multi_refine_h_
#define boxm2_multi_refine_h_
//:
// \file
// \brief This class does the cumulative seg len and cumulative observation on the GPU.

#include <boxm2_multi/boxm2_multi_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//: boxm2_multi_cache - example realization of abstract cache class
class boxm2_multi_refine
{
  public:
    typedef std::map<boxm2_block_id, bocl_mem_sptr> BlockMemMap;
    typedef std::map<boxm2_block_id, int>           BlockIntMap;

    //three separate sub procedures (three separate map reduce tasks)
    static float refine( boxm2_multi_cache& cache, float thresh = .3f );

  private:
    static float refine_trees_per_block(const boxm2_block_id& id,
                                        boxm2_opencl_cache1* ocl_cache,
                                        cl_command_queue& queue,
                                        int numTrees,
                                        BlockMemMap&  sizebuffs,
                                        BlockMemMap&  blockCopies,
                                        bocl_mem_sptr& prob_thresh,
                                        bocl_mem_sptr& lookup,
                                        bocl_mem_sptr& cl_output );

    //: refines trees in block
    static void swap_data_per_block(const boxm2_scene_sptr& scene,
                                    const boxm2_block_id& id,
                                    int numTrees,
                                    boxm2_opencl_cache1* ocl_cache,
                                    cl_command_queue& queue,
                                    BlockMemMap& sizebuffs,
                                    BlockMemMap& blockCopies,
                                    BlockMemMap& newDatas,
                                    BlockIntMap& newDataSizes,
                                    const bocl_mem_sptr& cl_output,
                                    const bocl_mem_sptr& lookup,
                                    const std::string& data_type,
                                    int  apptypesize,
                                    const bocl_mem_sptr& prob_thresh );


    //does in place, zero based cumulative sum on cpu, returns total size
    static int cumsum(int* buff, std::size_t len) {
      //non zero based cumsum
      for (unsigned int i=1; i<len; ++i)
        buff[i] += buff[i-1];
      int newSize = buff[len-1];
      //zero based
      for (int i=len-1; i>0; --i)
        buff[i] = buff[i-1];
      buff[0] = 0;
      return newSize;
    }

    //compile kernels and cache
    static bocl_kernel* get_refine_tree_kernel(const bocl_device_sptr& device, const std::string& opts);
    static bocl_kernel* get_refine_data_kernel(const bocl_device_sptr& device, const std::string& opts);

    //map keeps track of all kernels compiled and cached
    static std::map<std::string, bocl_kernel*> refine_tree_kernels_;
    static std::map<std::string, bocl_kernel*> refine_data_kernels_;

    //help with kernel compilation
    static std::string get_option_string(int datasize);
};

#endif
