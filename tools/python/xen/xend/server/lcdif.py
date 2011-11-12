


import re
import string

from xen.util import blkif
from xen.xend import sxp
from xen.xend.XendError import VmError

from xen.xend.server.DevController import DevController


class lcdifController(DevController):
    """lcd device interface controller. Handles all lcd  devices
    for a domain.
    """

    def __init__(self, vm):
        """Create a lcd device controller.
        """
        DevController.__init__(self, vm)


    def getDeviceDetails(self, config):
        """@see DevControllergetDeviceDetails"""

        ssi = sxp.child_value(config, 'ssi')

        back = { 'ssi' : ssi}
        """ device id is determined by O.S. (see blkif.getDeviceDetails)
	"""
        devid = 9999
	ssa_value = 444
        front = { 'virtual-device' : "%i" % devid, 'ssa-register': "%i" % ssa_value}

        return (devid, back, front)


    def configuration(self, devid):
        """@see DevController.configuration"""
	return 


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
