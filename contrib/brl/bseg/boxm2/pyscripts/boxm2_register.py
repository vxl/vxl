#############################################################################
# Register Script to ensure boxm2_batch processes are only registered once
#
# to use our boxm2 python binding, be sure to add: 
#   <vxl_build_root>/lib/:<vxl_src_root>/contrib/brl/bseg/boxm2/pyscripts/ 
# to your PYTHONPATH environment variable.  
#############################################################################
import boxm2_batch;
boxm2_batch.register_processes();
boxm2_batch.register_datatypes();

#class used for python/c++ pointers in database
class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string
