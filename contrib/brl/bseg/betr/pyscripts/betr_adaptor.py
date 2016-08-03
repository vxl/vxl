# import the batch module and dbvalue from init 
# set the global variable, batch, on init before importing this file
import brl_init
import betr_batch as batch
dbvalue = brl_init.register_batch(batch)

#############################################################################
# PROVIDES higher level betr python functions to make batch
# code more readable/refactored
#############################################################################


# create a betr site to hold event data such as event objects
def create_betr_site( lon, lat, elev):
    batch.init_process("betrCreateSiteProcess")
    # set site location in wgs84 deg, deg, meters
    batch.set_input_float(0, lon)
    batch.set_input_float(1, lat)
    batch.set_input_float(2, elev)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        site = dbvalue(id, type)
        return site
    return status
# create a betr event trigger to support change detection
def create_betr_event_trigger( lon, lat, elev, name):
    batch.init_process("betrCreateEventTriggerProcess")
    # set site location in wgs84 deg, deg, meters
    batch.set_input_float(0, lon)
    batch.set_input_float(1, lat)
    batch.set_input_float(2, elev)
    batch.set_input_string(3, name)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        event_trigger = dbvalue(id, type)
        return event_trigger
    return status
# add a geo object to the site, not associated with an event trigger
def add_site_object(site, name, lon, lat, elev, geom_path):
    batch.init_process("betrAddEventObjectProcess")
    batch.set_input_from_db(0, site)
    batch.set_input_string(1, name)
    batch.set_input_float(2, lon)
    batch.set_input_float(3, lat)
    batch.set_input_float(4, elev)
    batch.set_input_string(5, geom_path)
    status = batch.run_process()
    return status

# add an event trigger to the site
def add_event_trigger(site, event_trigger):
    batch.init_process("betrAddEventTriggerProcess")
    batch.set_input_from_db(0, site)
    batch.set_input_from_db(1, event_trigger)
    status = batch.run_process()
    return status

# add an event trigger object to the event trigger
def add_event_trigger_object(event_trigger, name, lon, lat, elev, geom_path, is_reference):
    batch.init_process("betrAddEventTriggerObjectProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_string(1, name)
    batch.set_input_float(2, lon)
    batch.set_input_float(3, lat)
    batch.set_input_float(4, elev)
    batch.set_input_string(5, geom_path)
    batch.set_input_bool(6, is_reference)
    status = batch.run_process()
    return status

# set image and camera data on an event trigger
def set_event_trigger_data(event_trigger, ref_imgr, ref_camera, event_imgr, event_camera):
    batch.init_process("betrSetEventTriggerDataProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_from_db(1, ref_imgr)
    batch.set_input_from_db(2, ref_camera)
    batch.set_input_from_db(3, event_imgr)
    batch.set_input_from_db(4, event_camera)
    status = batch.run_process()
    return status

# execute change detection
def execute_event_trigger(event_trigger, algorithm_name):
    batch.init_process("betrExecuteEventTriggerProcess")
    batch.set_input_from_db(0, event_trigger)
    batch.set_input_string(1, algorithm_name);
    status = batch.run_process()
    prob_change = None
    if status:
        (pc_id, pc_type) = batch.commit_output(0);
        prob_change = batch.get_output_float(pc_id);
    return (status, prob_change)
