import bbgm_batch
bbgm_batch.register_processes();
bbgm_batch.register_datatypes();
class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string
stream = dbvalue(0,"");
model = dbvalue(0,"");

bbgm_batch.init_process("OpenVidl2IstreamProcess")
print bbgm_batch.set_input_string(0, "f:CapitolSiteLaptop/short_reg_sequence/*.tiff");
print bbgm_batch.run_process();
(stream.id, stream.type) = bbgm_batch.commit_output(0);
bbgm_batch.init_process("bbgmUpdateDistImageStreamProcess")
print bbgm_batch.set_input_from_db(1, stream); # input stream
print bbgm_batch.set_input_int(2, 3); # number of mixture components
print bbgm_batch.set_input_int(3, 300); # window size
print bbgm_batch.set_input_float(4, 0.1); # initial variance
print bbgm_batch.set_input_float(5, 3.0); # g_thresh
print bbgm_batch.set_input_float(6, 0.02);# minimum standard deviation
print bbgm_batch.set_input_int(7, 0);# start frame
print bbgm_batch.set_input_int(8, -1);# end frame -1 == do all
print bbgm_batch.process_init();
print bbgm_batch.run_process();
(model.id, model.type) = bbgm_batch.commit_output(0);
print bbgm_batch.remove_data(stream.id);
bbgm_batch.init_process("bbgmSaveImageOfProcess");
bbgm_batch.set_input_string(0,"f:/CapitolSiteLaptop/model_1_30_09.mdl");
bbgm_batch.set_input_from_db(1, model);
print bbgm_batch.run_process();
print bbgm_batch.remove_data(model.id);
bbgm_batch.print_db();





