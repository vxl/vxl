import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)

# compute offsets based on mutual information


def register_translational(img0, img0_mask, img1, search_radius=0):
  batch.init_process('ihogRegisterTranslationalProcess')
  batch.set_input_from_db(0, img0)
  batch.set_input_from_db(1, img0_mask)
  batch.set_input_from_db(2, img1)
  batch.set_input_int(3, search_radius)
  batch.run_process()
  (id, type) = batch.commit_output(0)
  trans_x = batch.get_output_double(id)
  (id, type) = batch.commit_output(1)
  trans_y = batch.get_output_double(id)
  return (trans_x, trans_y)

# compute cost surface image based on mutual information


def compute_mi_cost_surface(img0, img0_mask, img1, search_radius):
  batch.init_process('ihogComputeMiCostSurfaceProcess')
  batch.set_input_from_db(0, img0)
  batch.set_input_from_db(1, img0_mask)
  batch.set_input_from_db(2, img1)
  batch.set_input_int(3, search_radius)
  batch.run_process()
  (id, type) = batch.commit_output(0)
  cost_image = dbvalue(id, type)
  return cost_image

# compute mutual information, this is actually entropy diff so the lower
# the better


def mutual_information(img0, img1, mask):
  batch.init_process('ihogMutualInformationProcess')
  batch.set_input_from_db(0, img0)
  batch.set_input_from_db(1, img1)
  batch.set_input_from_db(2, mask)
  batch.run_process()
  (id, type) = batch.commit_output(0)
  minfo = batch.get_output_float(id)
  batch.remove_data(id)
  return minfo
