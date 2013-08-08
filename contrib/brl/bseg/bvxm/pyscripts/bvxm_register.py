#############################################################################
# Register Script to ensure bvxm_batch processes are only registered once
#
# to use our bvxm python binding, be sure to add:
#   <vxl_build_root>/lib/:<vxl_src_root>/contrib/brl/bseg/bvxm/pyscripts/
# to your PYTHONPATH environment variable.
#############################################################################
import bvxm_batch;
bvxm_batch.not_verbose();
bvxm_batch.register_processes();
bvxm_batch.register_datatypes();

#class used for python/c++ pointers in database
class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

def remove_data(id):
  bvxm_batch.remove_data(id)

def get_output_float(id):
  fval = bvxm_batch.get_output_float(id)
  return fval

def get_output_unsigned(id):
  uval = bvxm_batch.get_output_unsigned(id)
  return uval
