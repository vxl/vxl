# import the batch module and dbvalue from init 
# set the global variable, batch, on init before importing this file
import init
dbvalue = init.dbvalue;
batch = init.batch

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
def add_event_object(site, name, lon, lat, elev, geom_path):
    batch.init_process("betrAddEventObjectProcess")
    batch.set_input_from_db(0, site)
    batch.set_input_string(1, name)
    batch.set_input_float(2, lon)
    batch.set_input_float(3, lat)
    batch.set_input_float(4, elev)
    batch.set_input_string(5, geom_path)
    status = batch.run_process()
    return status
