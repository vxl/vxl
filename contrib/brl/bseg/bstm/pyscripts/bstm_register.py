############################################################################
# Register Script to ensure bstm_batch processes are only registered once
#
# to use our boxm2 python binding, be sure to add: 
#   <vxl_build_root>/lib/:<vxl_src_root>/contrib/brl/bseg/bstm/pyscripts/ 
# to your PYTHONPATH environment variable.  
#############################################################################
import bstm_batch;
bstm_batch.not_verbose();
bstm_batch.register_processes();
bstm_batch.register_datatypes();

#class used for python/c++ pointers in database
class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

def remove_data(data):
  bstm_batch.remove_data(data.id)

def get_output_float(id):
  fval = bstm_batch.get_output_float(id)
  return fval

def get_output_unsigned(id):
  uval = bstm_batch.get_output_unsigned(id)
  return uval

