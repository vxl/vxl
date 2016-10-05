##########################################################################
# Created by Gamze D. Tunali
#            LEMS, Brown University
#            Nov 11, 2008
##########################################################################
import brl_init
import bmdl_batch as batch
dbvalue = brl_init.register_batch(batch)
xdim = 2000
ydim = 2000
xover = 50
yover = 50
batch.init_process("bmdlModelingProcess")
batch.set_params_process("./modeling_params.xml")
batch.set_input_string(
    0, "C://test_images//BaghdadLIDAR//dem_1m_a1_baghdad_tile39.tif")
batch.set_input_string(
    1, "C://test_images//BaghdadLIDAR//dem_1m_a2_baghdad_tile39.tif")
batch.set_input_string(2, "")
batch.set_input_string(3, "C://test_images//IMESH//bmdl_kmz")
batch.set_input_unsigned(4, xdim)
batch.set_input_unsigned(5, ydim)
batch.set_input_unsigned(6, xover)
batch.set_input_unsigned(7, yover)
batch.run_process()
