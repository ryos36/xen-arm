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
#============================================================================

"""Domain shutdown.
"""
import string
import sys
import time

from xen.xend.XendClient import server
from xen.xend import sxp
from xen.xm.opts import *

DOM0_ID = '0'

gopts = Opts(use="""[options] [DOM]

Shutdown one or more domains gracefully.
""")

gopts.opt('help', short='h',
         fn=set_true, default=0,
         use="Print this help.")

gopts.opt('all', short='a',
         fn=set_true, default=0,
         use="Shutdown all domains.")

gopts.opt('wait', short='w',
         fn=set_true, default=0,
         use='Wait for shutdown to complete.')

gopts.opt('halt', short='H',
          fn=set_true, default=0,
          use='Shutdown without reboot.')

gopts.opt('reboot', short='R',
          fn=set_true, default=0,
          use='Shutdown and reboot.')

def shutdown(opts, doms, mode, wait):
    if doms == None: doms = server.xend.domains(0)
    dom0_name = sxp.child_value(server.xend.domain(0), 'name')
    for x in [dom0_name, DOM0_ID]:
        if x in doms:
            if opts.vals.all:
                doms.remove(x)
            else:
                opts.err("Can't specify Domain-0")
    for d in doms:
        server.xend.domain.shutdown(d, mode)
    if wait:
        while doms:
            alive = server.xend.domains(0)
            dead = []
            for d in doms:
                if d in alive: continue
                dead.append(d)
            for d in dead:
                opts.info("Domain %s terminated" % d)
                doms.remove(d)
            time.sleep(1)
        opts.info("All domains terminated")

def shutdown_mode(opts):
    if opts.vals.halt and opts.vals.reboot:
        opts.err("Can't specify halt and reboot")

    if opts.vals.halt:
        return 'halt'
    elif opts.vals.reboot:
        return 'reboot'
    else:
        return 'poweroff'

def main_all(opts, args):
    mode = shutdown_mode(opts)  
    shutdown(opts, None, mode, opts.vals.wait)

def main_dom(opts, args):
    if len(args) < 1: opts.err('Missing domain')
    dom = args[0]
    mode = shutdown_mode(opts)  
    shutdown(opts, [ dom ], mode, opts.vals.wait)
    
def main(argv):
    opts = gopts
    args = opts.parse(argv)
    if opts.vals.help:
        opts.usage()
        return
    if opts.vals.all:
        main_all(opts, args)
    else:
        main_dom(opts, args)
        
