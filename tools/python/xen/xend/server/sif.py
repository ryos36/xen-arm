import re
import string

from xen.xend import sxp
from xen.xend.XendError import VmError
from xen.xend.XendLogging import log
from xen.xend.server.DevController import DevController

class SifController(DevController):
    """Virtual USB serial"""

    def __init__(self, vm):
        DevController.__init__(self, vm)
        self.hotplug = False

    def getDeviceDetails(self,config):

        back = {}
        front = {}
        devid = 977
        front = { 'virtual-device' : "%i" % devid }
        log.info('sback :%s',back)
        log.info('sfront:%s',front)
        return (devid, back, front)

    def waitForDevice(self, devid):
	log.debug("Waiting for %s.", devid)

    def destroyDevice(self, devid):
        try:
            DevController.destroyDevice(self, int(devid))
        except ValueError:
            devid_end = type(devid) is str and devid.split('/')[-1] or None

            for i in self.deviceIDs():
                d = self.readBackend(i, 'dev')
                if d == devid or (devid_end and d == devid_end):
                    DevController.destroyDevice(self, i)
                    return
            raise VmError("Device %s not connected" % devid)

