
import re
import string

from xen.xend import sxp
from xen.xend.XendError import VmError
from xen.xend.XendLogging import log
from xen.xend.server.DevController import DevController

ssa_default = 0

class KppifController(DevController):
    """Key pad device interface controller. Handles a keypad devices
    for a domain.
    """

    def __init__(self, vm):
        """Create a keypad device controller.
        """
        DevController.__init__(self, vm)


    def getDeviceDetails(self,config):
        """@see DevController.getDeviceDetails"""

        back = { 'ssa' : "%i" % ssa_default }
        devid = 1
        front = { 'virtual-device' : "%i" % devid }
        log.info('kpp back :%s',back)
	log.info('kpp front:%s',front)
        return (devid, back, front)


    def configuration(self, devid):
        """@see DevController.configuration"""

        result = DevController.configuration(self, devid)
        result.append(['ssa','0'])
        return result


    def destroyDevice(self, devid):
        """@see DevController.destroyDevice"""

        # If we are given a device name, then look up the device ID from it,
        # and destroy that ID instead.  If what we are given is an integer,
        # then assume it's a device ID and pass it straight through to our
        # superclass's method.

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

    def waitForDevice(self, devid):
        log.debug("Waiting for %s.", devid)
	#key pad panel don't have hot-plug script so just return nothig