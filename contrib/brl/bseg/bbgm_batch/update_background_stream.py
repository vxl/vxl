import brl_init
import bbgm_batch as batch
dbvalue = brl_init.register_batch(batch)

stream = dbvalue(0, "")
model = dbvalue(0, "")

batch.init_process("OpenVidl2IstreamProcess")
print batch.set_input_string(0, "F:/CapitolSiteLaptop/short_reg_sequence/*.tiff")
print batch.run_process()
(stream.id, stream.type) = batch.commit_output(0)
batch.init_process("bbgmUpdateDistImageStreamProcess")
print batch.set_input_from_db(1, stream)  # input stream
print batch.set_input_int(2, 3)  # number of mixture components
print batch.set_input_int(3, 300)  # window size
print batch.set_input_float(4, 0.1)  # initial variance
print batch.set_input_float(5, 3.0)  # g_thresh
print batch.set_input_float(6, 0.02)  # minimum standard deviation
print batch.set_input_int(7, 0)  # start frame
print batch.set_input_int(8, -1)  # end frame -1 == do all
print batch.process_init()
print batch.run_process()
(model.id, model.type) = batch.commit_output(0)
print batch.remove_data(stream.id)
batch.init_process("bbgmSaveImageOfProcess")
batch.set_input_string(0, "F:/CapitolSiteLaptop/model_1_30_09.mdl")
batch.set_input_from_db(1, model)
print batch.run_process()
print batch.remove_data(model.id)
batch.print_db()
