#============================================================================
# This library is free software; you can redistribute it and/or
# modify it under the terms of version 2.1 of the GNU Lesser General Public
# License as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#============================================================================
# Copyright (C) 2004, 2005 Mike Wray <mike.wray@hp.com>
# Copyright (C) 2005 XenSource Ltd
#============================================================================

#============================================================================
# Some codes in this file was modified
# for supporting ARM processor by Samsung Electronics.
#============================================================================

from threading import Event

from xen.xend import sxp
from xen.xend.XendError import VmError
from xen.xend.XendLogging import log

from xen.xend.xenstore.xstransact import xstransact, complete
from xen.xend.xenstore.xswatch import xswatch

DEVICE_CREATE_TIMEOUT = 10
HOTPLUG_STATUS_NODE = "hotplug-status"
HOTPLUG_ERROR_NODE  = "hotplug-error"
HOTPLUG_STATUS_ERROR = "error"
HOTPLUG_STATUS_BUSY  = "busy"

Connected = 1
Error     = 2
Missing   = 3
Timeout   = 4
Busy      = 5

xenbusState = {
    'Unknown'      : 0,
    'Initialising' : 1,
    'InitWait'     : 2,
    'Initialised'  : 3,
    'Connected'    : 4,
    'Closing'      : 5,
    'Closed'       : 6,
    }

xenbusState.update(dict(zip(xenbusState.values(), xenbusState.keys())))


class DevController:
    """Abstract base class for a device controller.  Device controllers create
    appropriate entries in the store to trigger the creation, reconfiguration,
    and destruction of devices in guest domains.  Each subclass of
    DevController is responsible for a particular device-class, and
    understands the details of configuration specific to that device-class.

    DevController itself provides the functionality common to all device
    creation tasks, as well as providing an interface to XendDomainInfo for
    triggering those events themselves.
    """

    # Set when registered.
    deviceClass = None


    ## public:

    def __init__(self, vm):
        self.vm = vm


    def createDevice(self, config):
        """Trigger the creation of a device with the given configuration.

        @return The ID for the newly created device.
        """
        (devid, back, front) = self.getDeviceDetails(config)
        if devid is None:
            return 0

        (backpath, frontpath) = self.addStoreEntries(config, devid, back,
                                                     front)

        import xen.xend.XendDomain
        count = 0
        while True:
            t = xstransact()
            try:
                if devid in self.deviceIDs(t):
                    if 'dev' in back:
                        dev_str = '%s (%d, %s)' % (back['dev'], devid,
                                                   self.deviceClass)
                    else:
                        dev_str = '%s (%s)' % (devid, self.deviceClass)
                    
                    raise VmError("Device %s is already connected." % dev_str)

                if count == 0:
                    log.debug('DevController: writing %s to %s.', str(front),
                              frontpath)
                    log.debug('DevController: writing %s to %s.', str(back),
                              backpath)
                elif count % 50 == 0:
                    log.debug(
                      'DevController: still waiting to write device entries.')

                t.remove(frontpath)
                t.remove(backpath)

                t.mkdir(backpath)
                t.set_permissions(backpath,
                                  {'dom': xen.xend.XendDomain.PRIV_DOMAIN },
                                  {'dom'  : self.vm.getDomid(),
                                   'read' : True })

                t.write2(frontpath, front)
                t.write2(backpath,  back)

                if t.commit():
                    return devid

                count += 1
            except:
                t.abort()
                raise


    def waitForDevices(self):
        log.debug("Waiting for devices %s.", self.deviceClass)
        
        return map(self.waitForDevice, self.deviceIDs())


    def waitForDevice(self, devid):
        log.debug("Waiting for %s.", devid)
        
        status = self.waitForBackend(devid)

        if status == Timeout:
            self.destroyDevice(devid)
            raise VmError("Device %s (%s) could not be connected. "
                          "Hotplug scripts not working." %
                          (devid, self.deviceClass))

        elif status == Error:
            self.destroyDevice(devid)
            raise VmError("Device %s (%s) could not be connected. "
                          "Backend device not found." %
                          (devid, self.deviceClass))

        elif status == Missing:
            # Don't try to destroy the device; it's already gone away.
            raise VmError("Device %s (%s) could not be connected. "
                          "Device not found." % (devid, self.deviceClass))

        elif status == Busy:
            err = None
            frontpath = self.frontendPath(devid)
            backpath = xstransact.Read(frontpath, "backend")
            if backpath:
                err = xstransact.Read(backpath, HOTPLUG_ERROR_NODE)
            if not err:
                err = "Busy."
                
            self.destroyDevice(devid)
            raise VmError("Device %s (%s) could not be connected.\n%s" %
                          (devid, self.deviceClass, err))



    def reconfigureDevice(self, devid, config):
        """Reconfigure the specified device.

        The implementation here just raises VmError.  This may be overridden
        by those subclasses that can reconfigure their devices.
        """
        raise VmError('%s devices may not be reconfigured' % self.deviceClass)


    def destroyDevice(self, devid):
        """Destroy the specified device.

        @param devid The device ID, or something device-specific from which
        the device ID can be determined (such as a guest-side device name).

        The implementation here simply deletes the appropriate paths from the
        store.  This may be overridden by subclasses who need to perform other
        tasks on destruction.  Further, the implementation here can only
        accept integer device IDs, or values that can be converted to
        integers.  Subclasses may accept other values and convert them to
        integers before passing them here.
        """

        devid = int(devid)
        
        frontpath = self.frontendPath(devid)
        backpath = xstransact.Read(frontpath, "backend")

        if backpath:
            xstransact.Write(backpath, 'state', str(xenbusState['Closing']))
        else:
            raise VmError("Device %s not connected" % devid)
           

    def configurations(self):
        return map(self.configuration, self.deviceIDs())


    def configuration(self, devid):
        """@return an s-expression giving the current configuration of the
        specified device.  This would be suitable for giving to {@link
        #createDevice} in order to recreate that device."""

        backdomid = xstransact.Read(self.frontendPath(devid), "backend-id")
        if backdomid is None:
            raise VmError("Device %s not connected" % devid)
        
        return [self.deviceClass, ['backend', int(backdomid)]]


    def sxprs(self):
        """@return an s-expression describing all the devices of this
        controller's device-class.
        """
        return xstransact.ListRecursive(self.frontendRoot())


    def sxpr(self, devid):
        """@return an s-expression describing the specified device.
        """
        return [self.deviceClass, ['dom', self.vm.getDomid(),
                                   'id', devid]]


    ## protected:

    def getDeviceDetails(self, config):
        """Compute the details for creation of a device corresponding to the
        given configuration.  These details consist of a tuple of (devID,
        backDetails, frontDetails), where devID is the ID for the new device,
        and backDetails and frontDetails are the device configuration
        specifics for the backend and frontend respectively.

        backDetails and frontDetails should be dictionaries, the keys and
        values of which will be used as paths in the store.  There is no need
        for these dictionaries to include the references from frontend to
        backend, nor vice versa, as these will be handled by DevController.

        Abstract; must be implemented by every subclass.

        @return (devID, backDetails, frontDetails), as specified above.
        """

        raise NotImplementedError()


    def getDomid(self):
        """Stub to {@link XendDomainInfo.getDomid}, for use by our
        subclasses.
        """
        return self.vm.getDomid()


    def allocateDeviceID(self):
        """Allocate a device ID, allocating them consecutively on a
        per-domain, per-device-class basis, and using the store to record the
        next available ID.

        This method is available to our subclasses, though it is not
        compulsory to use it; subclasses may prefer to allocate IDs based upon
        the device configuration instead.
        """
        path = self.frontendMiscPath()
        return complete(path, self._allocateDeviceID)


    def _allocateDeviceID(self, t):
        result = t.read("nextDeviceID")
        if result:
            result = int(result)
        else:
            result = 0
        t.write("nextDeviceID", str(result + 1))
        return result


    def readBackend(self, devid, *args):
        frontpath = self.frontendPath(devid)
        backpath = xstransact.Read(frontpath, "backend")
        if backpath:
            return xstransact.Read(backpath, *args)
        else:
            raise VmError("Device %s not connected" % devid)


    def deviceIDs(self, transaction = None):
        """@return The IDs of each of the devices currently configured for
        this instance's deviceClass.
        """
        fe = self.frontendRoot()
        if transaction:
            return map(lambda x: int(x.split('/')[-1]), transaction.list(fe))
        else:
            return map(int, xstransact.List(fe))


## private:

    def addStoreEntries(self, config, devid, backDetails, frontDetails):
        """Add to backDetails and frontDetails the entries to be written in
        the store to trigger creation of a device.  The backend domain ID is
        taken from the given config, paths for frontend and backend are
        computed, and these are added to the backDetails and frontDetails
        dictionaries for writing to the store, including references from
        frontend to backend and vice versa.

        @return A pair of (backpath, frontpath).  backDetails and frontDetails
        will have been updated appropriately, also.

        @param config The configuration of the device, as given to
        {@link #createDevice}.
        @param devid        As returned by {@link #getDeviceDetails}.
        @param backDetails  As returned by {@link #getDeviceDetails}.
        @param frontDetails As returned by {@link #getDeviceDetails}.
        """

        import xen.xend.XendDomain
        xd = xen.xend.XendDomain.instance()

        backdom_name = sxp.child_value(config, 'backend')
        if backdom_name:
            backdom = xd.domain_lookup_by_name_or_id_nr(backdom_name)
        else:
            backdom = xd.privilegedDomain()

        if not backdom:
            raise VmError("Cannot configure device for unknown backend %s" %
                          backdom_name)

        frontpath = self.frontendPath(devid)
        backpath  = self.backendPath(backdom, devid)
        
        frontDetails.update({
            'backend' : backpath,
            'backend-id' : "%i" % backdom.getDomid(),
            'state' : str(xenbusState['Initialising'])
            })


        backDetails.update({
            'domain' : self.vm.getName(),
            'frontend' : frontpath,
            'frontend-id' : "%i" % self.vm.getDomid(),
            'state' : str(xenbusState['Initialising'])
            })

        return (backpath, frontpath)


    def waitForBackend(self, devid):
        
        # FIXME!!! We haven't udev daemon...
        #result = { 'status': Connected }
        #return result['status']

        frontpath = self.frontendPath(devid)
        backpath = xstransact.Read(frontpath, "backend")

        if backpath:
            statusPath = backpath + '/' + HOTPLUG_STATUS_NODE
            ev = Event()
            result = { 'status': Timeout }
            
            xswatch(statusPath, hotplugStatusCallback, ev, result)

            ev.wait(DEVICE_CREATE_TIMEOUT)
            return result['status']
        else:
            return Missing


    def backendPath(self, backdom, devid):
        """@param backdom [XendDomainInfo] The backend domain info."""

        return "%s/backend/%s/%s/%d" % (backdom.getDomainPath(),
                                        self.deviceClass,
                                        self.vm.getDomid(), devid)


    def frontendPath(self, devid):
        return "%s/%d" % (self.frontendRoot(), devid)


    def frontendRoot(self):
        return "%s/device/%s" % (self.vm.getDomainPath(), self.deviceClass)


    def frontendMiscPath(self):
        return "%s/device-misc/%s" % (self.vm.getDomainPath(),
                                      self.deviceClass)


def hotplugStatusCallback(statusPath, ev, result):
    log.debug("hotplugStatusCallback %s.", statusPath)

    status = xstransact.Read(statusPath)

    if status is not None:
        if status == HOTPLUG_STATUS_ERROR:
            result['status'] = Error
        elif status == HOTPLUG_STATUS_BUSY:
            result['status'] = Busy
        else:
            result['status'] = Connected
    else:
        return 1

    log.debug("hotplugStatusCallback %d.", result['status'])

    ev.set()
    return 0
