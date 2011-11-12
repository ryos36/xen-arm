import re
import string

from xen.xend import sxp
from xen.xend.XendError import VmError
from xen.xend.XendLogging import log
from xen.xend.server.DevController import DevController

class FbifController(DevController):
    """Key pad device interface controller. Handles a keypad devices
    for a domain.
    """

    def __init__(self, vm):
        """Create a keypad device controller.
        """
        DevController.__init__(self, vm)


    def getDeviceDetails(self,config):
        """@see DevController.getDeviceDetails"""

        back = {}
        front = {}
        devid = 1
        log.info('fb back :%s',back)
	log.info('fb front:%s',front)
        return (devid, back, front)

    def waitForDevice(self, devid):
        log.debug("Waiting for %s.", devid)
