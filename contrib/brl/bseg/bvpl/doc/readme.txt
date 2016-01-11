BVPL is Brown Voxel Processing Library

This library is mainly used to operate "kernels" or operators on Brown's volumetric data.

kernels: contains all kind of kernels e.g differential. kernels are composed of 3d locations and a weight associated to each location. When creating a new kernel, you must write a new kernel factory, which will construct the default kernel. All kernel inherit from bvpl_kernel_factory, where common functions such as rotation are implemented


functor: specify how the weights in the kernels are applied to the data

bvpl_octree: function specific to deal with boxm and boct libraries


################### Additional Documentation ######################

taylor_basis.pdf - background on how the taylor kernels are generated
taylor_kernels.m - matlab file to producen kernel files
taylor2_5_5_5 - second degree taylor kernels with dimension 5x5x5
