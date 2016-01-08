Each of the subfolders contains scripts for different experiments:

reconstruction error: Finds the reconstruction error using PCA kernels of a scene. The error is computed for a fixed fraction of training samples and varying number of dimensions.

training error: Finds the reconstruction error using PCA kernels of a scene. The error is computed for a fixed dimension and varying number of training samples

Note: *.sh calls the python scripts with appropriate inputs. You only need to adjust *.sh for your system
