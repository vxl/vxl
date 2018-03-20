import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)

#############################################################################
# PROVIDES higher level python functions to make boxm2_batch
# code more readable/refactored
#############################################################################


# does the opencl prep work on an input scene
def load_multi_cache(scene, numdevices=1):
    ###############################################################
    # Create multi cache, opencl manager, device, and gpu cache
    ###############################################################
    #print("Create Main Cache");
    batch.init_process("boxm2CreateMultiCacheProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_int(1, numdevices)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mcache = dbvalue(id, type)

    return mcache


###############################################
# Model building stuff
###############################################
# Generic update - will use GPU if device/openclcache are passed in
def update_multi(scene, mcache, cam, img,  ident="", update_alpha=True, var=-1.0, update_app=True, tnear=100000.0, tfar=0.000001):
    # If no device is passed in, do cpu update
    print("boxm2_batch multi GPU update")
    batch.init_process("boxm2MultiUpdateProcess")
    batch.set_input_from_db(0, mcache)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cam)
    batch.set_input_from_db(3, img)
    batch.set_input_string(4, ident)
    batch.set_input_bool(5, update_alpha)
    batch.set_input_float(6, var)
    batch.set_input_bool(7, update_app)
    batch.set_input_float(8, tnear)
    batch.set_input_float(9, tfar)
    return batch.run_process()
#####################################################################
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
#####################################################################


def render_multi(scene, mcache, cam, ni=1280, nj=720, ident_string="", tnear=100000.0, tfar=100000.0):
    batch.init_process("boxm2MultiRenderProcess")
    batch.set_input_from_db(0, mcache)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, cam)
    batch.set_input_unsigned(3, ni)
    batch.set_input_unsigned(4, nj)
    batch.set_input_string(5, ident_string)
    batch.set_input_float(6, tnear)
    batch.set_input_float(7, tfar)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    exp_image = dbvalue(id, type)
    return exp_image


######################################################################
# cache methods
#####################################################################
# generic write cache to disk
def write_mcache(mcache, do_clear=0):
    batch.init_process("boxm2WriteMultiCacheProcess")
    batch.set_input_from_db(0, mcache)
    batch.set_input_bool(1, do_clear)
    batch.run_process()
