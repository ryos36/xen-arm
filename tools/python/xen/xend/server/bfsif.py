import re
import string
import time

from xen.xend import sxp
from xen.xend.XendError import VmError
from xen.xend.XendLogging import log
from xen.xend.server.DevController import DevController
from xen.xend.xenstore.xstransact import xstransact

class BfsifController(DevController):
    """Virtual USB backed file storage"""

    def __init__(self, vm):
        DevController.__init__(self, vm)
        self.hotplug = False
        
    def getDeviceDetails(self,config):
	log.debug('vbfs config='+sxp.to_string(config))
	luns = sxp.child_value(config, 'luns')
	if not luns:
	    log.debug('vbfs: luns param not set')
	    raise VmError('luns param not set')
        back = { 'luns' : luns }
#        devid = self.allocateDeviceID()
        devid = 1
        front = { 'virtual-device' : "%i" % devid }
        log.info('bfsback :%s',back)
        log.info('bfsfront:%s',front)
        return (devid, back, front)

    def waitForDevice(self, devid):
        log.debug("Waiting for %s.", devid)

    def destroyDevice(self, devid):
        try:
    	    DevController.destroyDevice(self, int(devid))
    	    time.sleep(5)
    	    t = xstransact()
            frontpath = self.frontendPath(int(devid))
	    backpath = t.Read(frontpath, "backend")
	    if backpath:
		t.Remove(backpath)
	    	log.debug('in try: removed %s' % backpath)
	    t.Remove(frontpath)
	    log.debug('in try: removed %s' % frontpath)
        except ValueError:
            devid_end = type(devid) is str and devid.split('/')[-1] or None
	    for i in self.deviceIDs():
                d = self.readBackend(i, 'dev')
                if d == devid or (devid_end and d == devid_end):
            	    DevController.destroyDevice(self, i)
    		    time.sleep(5)
    		    frontpath = self.frontendPath(int(devid))
    		    backpath = t.Read(frontpath, "backend")
		    if backpath:
			t.Remove(backpath)
	    		log.debug('in err: removed %s' % backpath)
		    t.Remove(frontpath)
		    log.debug('in err: removed %s' % frontpath)
                    return
            raise VmError("Device %s not connected" % devid)

