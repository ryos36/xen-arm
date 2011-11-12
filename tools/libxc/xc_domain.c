/******************************************************************************
 * xc_domain.c
 * 
 * API for manipulating and obtaining information on domains.
 * 
 * Copyright (c) 2003, K A Fraser.
 */

/*
 * Some codes in this file was added, modified and deleted 
 * for supporting ARM processor by Samsung Electronics.
 */

#include "xc_private.h"
#include <xen/memory.h>

int xc_domain_create(int xc_handle,
                     //uint32_t ssidref,
                     xen_domain_handle_t handle,
                     uint32_t *pdomid)
{
    int err;
    DECLARE_DOM0_OP;

    op.cmd = DOM0_CREATEDOMAIN;
    op.u.createdomain.domain = (domid_t)*pdomid;
    //op.u.createdomain.ssidref = ssidref;
    memcpy(op.u.createdomain.handle, handle, sizeof(xen_domain_handle_t));
    if ( (err = do_dom0_op(xc_handle, &op)) != 0 )
        return err;

    *pdomid = (uint16_t)op.u.createdomain.domain;
    return 0;
}    


int xc_domain_pause(int xc_handle, 
                    uint32_t domid)
{
    DECLARE_DOM0_OP;
    op.cmd = DOM0_PAUSEDOMAIN;
    op.u.pausedomain.domain = (domid_t)domid;
    return do_dom0_op(xc_handle, &op);
}    


int xc_domain_unpause(int xc_handle,
                      uint32_t domid)
{
    DECLARE_DOM0_OP;
    op.cmd = DOM0_UNPAUSEDOMAIN;
    op.u.unpausedomain.domain = (domid_t)domid;
    return do_dom0_op(xc_handle, &op);
}    


int xc_domain_destroy(int xc_handle,
                      uint32_t domid)
{
    DECLARE_DOM0_OP;
    op.cmd = DOM0_DESTROYDOMAIN;
    op.u.destroydomain.domain = (domid_t)domid;
    return do_dom0_op(xc_handle, &op);
}

int xc_vcpu_setaffinity(int xc_handle,
                        uint32_t domid, 
                        int vcpu,
                        cpumap_t cpumap)
{
    DECLARE_DOM0_OP;
    op.cmd = DOM0_SETVCPUAFFINITY;
    op.u.setvcpuaffinity.domain  = (domid_t)domid;
    op.u.setvcpuaffinity.vcpu    = vcpu;
    op.u.setvcpuaffinity.cpumap  = cpumap;
    return do_dom0_op(xc_handle, &op);
}


int xc_domain_getinfo(int xc_handle,
                      uint32_t first_domid,
                      unsigned int max_doms,
                      xc_dominfo_t *info)
{
    unsigned int nr_doms;
    uint32_t next_domid = first_domid;
    DECLARE_DOM0_OP;
    int rc = 0; 
   
    memset(info, 0, max_doms*sizeof(xc_dominfo_t));
    //printf("%s, max_doms : %d\n",__FUNCTION__,max_doms);

    for ( nr_doms = 0; nr_doms < max_doms; nr_doms++ )
    {
        op.cmd = DOM0_GETDOMAININFO;
        op.u.getdomaininfo.domain = (domid_t)next_domid;
	//printf("next domain %d\n",next_domid);
        if ( (rc = do_dom0_op(xc_handle, &op)) < 0 )
            break;
        info->domid      = (uint16_t)op.u.getdomaininfo.domain;

        info->dying    = !!(op.u.getdomaininfo.flags & DOMFLAGS_DYING);
        info->shutdown = !!(op.u.getdomaininfo.flags & DOMFLAGS_SHUTDOWN);
        info->paused   = !!(op.u.getdomaininfo.flags & DOMFLAGS_PAUSED);
        info->blocked  = !!(op.u.getdomaininfo.flags & DOMFLAGS_BLOCKED);
        info->running  = !!(op.u.getdomaininfo.flags & DOMFLAGS_RUNNING);

        info->shutdown_reason = 
            (op.u.getdomaininfo.flags>>DOMFLAGS_SHUTDOWNSHIFT) & 
            DOMFLAGS_SHUTDOWNMASK;

        if ( info->shutdown && (info->shutdown_reason == SHUTDOWN_crash) )
        {
            info->shutdown = 0;
            info->crashed  = 1;
        }

        info->ssidref  = op.u.getdomaininfo.ssidref;
        info->nr_pages = op.u.getdomaininfo.tot_pages;
        info->max_memkb = op.u.getdomaininfo.max_pages << (PAGE_SHIFT - 10);
        info->shared_info_frame = op.u.getdomaininfo.shared_info_frame;
        info->cpu_time = op.u.getdomaininfo.cpu_time;
        info->nr_online_vcpus = op.u.getdomaininfo.nr_online_vcpus;
        info->max_vcpu_id = op.u.getdomaininfo.max_vcpu_id;

        memcpy(info->handle, op.u.getdomaininfo.handle,
               sizeof(xen_domain_handle_t));

        next_domid = (uint16_t)op.u.getdomaininfo.domain + 1;
        info++;
    }

    if( !nr_doms ) return rc; 
    return nr_doms;
}

int xc_domain_getinfolist(int xc_handle,
                          uint32_t first_domain,
                          unsigned int max_domains,
                          xc_domaininfo_t *info)
{
    int ret = 0;
    DECLARE_DOM0_OP;

    if ( mlock(info, max_domains*sizeof(xc_domaininfo_t)) != 0 )
        return -1;
    
    op.cmd = DOM0_GETDOMAININFOLIST;
    op.u.getdomaininfolist.first_domain = first_domain;
    op.u.getdomaininfolist.max_domains  = max_domains;
    op.u.getdomaininfolist.buffer       = info;

    if ( xc_dom0_op(xc_handle, &op) < 0 )
        ret = -1;
    else
        ret = op.u.getdomaininfolist.num_domains;
    
    if ( munlock(info, max_domains*sizeof(xc_domaininfo_t)) != 0 )
        ret = -1;
    
    return ret;
}

int xc_vcpu_getcontext(int xc_handle,
                               uint32_t domid,
                               uint32_t vcpu,
                               vcpu_guest_context_t *ctxt)
{
    int rc;
    DECLARE_DOM0_OP;

    op.cmd = DOM0_GETVCPUCONTEXT;
    op.u.getvcpucontext.domain = (domid_t)domid;
    op.u.getvcpucontext.vcpu   = (uint16_t)vcpu;
    op.u.getvcpucontext.ctxt   = ctxt;

    if ( (rc = mlock(ctxt, sizeof(*ctxt))) != 0 )
        return rc;

    rc = do_dom0_op(xc_handle, &op);

    safe_munlock(ctxt, sizeof(*ctxt));

    return rc;
}


int xc_shadow_control(int xc_handle,
                      uint32_t domid, 
                      unsigned int sop,
                      unsigned long *dirty_bitmap,
                      unsigned long pages,
                      xc_shadow_control_stats_t *stats )
{
    int rc;
    DECLARE_DOM0_OP;
    op.cmd = DOM0_SHADOW_CONTROL;
    op.u.shadow_control.domain = (domid_t)domid;
    op.u.shadow_control.op     = sop;
    op.u.shadow_control.dirty_bitmap = dirty_bitmap;
    op.u.shadow_control.pages  = pages;

    rc = do_dom0_op(xc_handle, &op);

    if ( stats )
        memcpy(stats, &op.u.shadow_control.stats,
               sizeof(xc_shadow_control_stats_t));

    return (rc == 0) ? op.u.shadow_control.pages : rc;
}

int xc_domain_setcpuweight(int xc_handle,
                           uint32_t domid,
                           float weight)
{
    int sched_id;
    int ret;
    
    /* Figure out which scheduler is currently used: */
    if ( (ret = xc_sched_id(xc_handle, &sched_id)) != 0 )
        return ret;
    
    switch ( sched_id )
    {
        case SCHED_BVT:
        {
            uint32_t mcuadv;
            int warpback;
            int32_t warpvalue;
            long long warpl;
            long long warpu;

            /* Preserve all the scheduling parameters apart 
               of MCU advance. */
            if ( (ret = xc_bvtsched_domain_get(
                xc_handle, domid, &mcuadv, 
                &warpback, &warpvalue, &warpl, &warpu)) != 0 )
                return ret;
            
            /* The MCU advance is inverse of the weight.
               Default value of the weight is 1, default mcuadv 10.
               The scaling factor is therefore 10. */
            if ( weight > 0 )
                mcuadv = 10 / weight;
            
            ret = xc_bvtsched_domain_set(xc_handle, domid, mcuadv, 
                                         warpback, warpvalue, warpl, warpu);
            break;
        }
    }

    return ret;
}

int xc_domain_setmaxmem(int xc_handle,
                        uint32_t domid, 
                        unsigned int max_memkb)
{
    DECLARE_DOM0_OP;
    op.cmd = DOM0_SETDOMAINMAXMEM;
    op.u.setdomainmaxmem.domain = (domid_t)domid;
    op.u.setdomainmaxmem.max_memkb = max_memkb;
    return do_dom0_op(xc_handle, &op);
}

int xc_domain_memory_increase_reservation(int xc_handle,
                                          uint32_t domid, 
                                          unsigned long nr_extents,
                                          unsigned int extent_order,
                                          unsigned int address_bits,
                                          unsigned long *extent_start)
{
    int err;
    struct xen_memory_reservation reservation = {
        .extent_start = extent_start, /* may be NULL */
        .nr_extents   = nr_extents,
        .extent_order = extent_order,  
        .address_bits = address_bits,
        .domid        = domid
    };

    err = xc_memory_op(xc_handle, XENMEM_increase_reservation, &reservation);
    if ( err == nr_extents )
        return 0;

    if ( err > 0 )
    {
        fprintf(stderr, "Failed allocation for dom %d: "
                "%ld pages order %d addr_bits %d\n",
                domid, nr_extents, extent_order, address_bits);
        errno = ENOMEM;
        err = -1;
    }

    return err;
}

int xc_domain_memory_decrease_reservation(int xc_handle,
                                          uint32_t domid, 
                                          unsigned long nr_extents,
                                          unsigned int extent_order,
                                          unsigned long *extent_start)
{
    int err;
    struct xen_memory_reservation reservation = {
        .extent_start = extent_start, 
        .nr_extents   = nr_extents,
        .extent_order = extent_order,  
        .address_bits = 0,
        .domid        = domid
    };

    if ( extent_start == NULL )
    {
        fprintf(stderr,"decrease_reservation extent_start is NULL!\n");
        errno = EINVAL;
        return -1;
    }

    err = xc_memory_op(xc_handle, XENMEM_decrease_reservation, &reservation);
    if ( err == nr_extents )
        return 0;

    if ( err > 0 )
    {
        fprintf(stderr,"Failed deallocation for dom %d: %ld pages order %d\n",
                domid, nr_extents, extent_order);
        errno = EBUSY;
        err = -1;
    }

    return err;
}

int xc_domain_memory_populate_physmap(int xc_handle,
                                          uint32_t domid,
                                          unsigned long nr_extents,
                                          unsigned int extent_order,
                                          unsigned int address_bits,
                                          unsigned long *extent_start)
{
    int err;
    struct xen_memory_reservation reservation = {
        .extent_start = extent_start,
        .nr_extents   = nr_extents,
        .extent_order = extent_order,
        .address_bits = address_bits,
        .domid        = domid
    };

    err = xc_memory_op(xc_handle, XENMEM_populate_physmap, &reservation);
    if ( err == nr_extents )
        return 0;

    if ( err > 0 )
    {
        fprintf(stderr,"Failed deallocation for dom %d: %ld pages order %d\n",
                domid, nr_extents, extent_order);
        errno = EBUSY;
        err = -1;
    }

    return err;
}

int xc_domain_translate_gpfn_list(int xc_handle,
                                  uint32_t domid,
                                  unsigned long nr_gpfns,
                                  unsigned long *gpfn_list,
                                  unsigned long *mfn_list)
{
    struct xen_translate_gpfn_list op = {
        .domid        = domid,
        .nr_gpfns     = nr_gpfns,
        .gpfn_list    = gpfn_list,
        .mfn_list     = mfn_list
    };

    return xc_memory_op(xc_handle, XENMEM_translate_gpfn_list, &op);
}

int xc_domain_max_vcpus(int xc_handle, uint32_t domid, unsigned int max)
{
    DECLARE_DOM0_OP;
    op.cmd = DOM0_MAX_VCPUS;
    op.u.max_vcpus.domain = (domid_t)domid;
    op.u.max_vcpus.max    = max;
    return do_dom0_op(xc_handle, &op);
}

int xc_domain_sethandle(int xc_handle, uint32_t domid, 
                        xen_domain_handle_t handle)
{
    DECLARE_DOM0_OP;
    op.cmd = DOM0_SETDOMAINHANDLE;
    op.u.setdomainhandle.domain = (domid_t)domid;
    memcpy(op.u.setdomainhandle.handle, handle, sizeof(xen_domain_handle_t));
    return do_dom0_op(xc_handle, &op);
}

int xc_vcpu_getinfo(int xc_handle,
                    uint32_t domid,
                    uint32_t vcpu,
                    xc_vcpuinfo_t *info)
{
    int rc;
    DECLARE_DOM0_OP;
    op.cmd = DOM0_GETVCPUINFO;
    op.u.getvcpuinfo.domain = (domid_t)domid;
    op.u.getvcpuinfo.vcpu   = (uint16_t)vcpu;

    rc = do_dom0_op(xc_handle, &op);

    memcpy(info, &op.u.getvcpuinfo, sizeof(*info));

    return rc;
}

int xc_domain_ioport_permission(int xc_handle,
                                uint32_t domid,
                                uint32_t first_port,
                                uint32_t nr_ports,
                                uint32_t allow_access)
{
    DECLARE_DOM0_OP;

    op.cmd = DOM0_IOPORT_PERMISSION;
    op.u.ioport_permission.domain = (domid_t)domid;
    op.u.ioport_permission.first_port = first_port;
    op.u.ioport_permission.nr_ports = nr_ports;
    op.u.ioport_permission.allow_access = allow_access;

    return do_dom0_op(xc_handle, &op);
}

int xc_vcpu_setcontext(int xc_handle,
                       uint32_t domid,
                       uint32_t vcpu,
                       vcpu_guest_context_t *ctxt)
{
    dom0_op_t op;
    int rc;

    op.cmd = DOM0_SETVCPUCONTEXT;
    op.u.setvcpucontext.domain = domid;
    op.u.setvcpucontext.vcpu = vcpu;
    op.u.setvcpucontext.ctxt = ctxt;

    if ( (rc = mlock(ctxt, sizeof(*ctxt))) != 0 )
        return rc;

    rc = do_dom0_op(xc_handle, &op);

    safe_munlock(ctxt, sizeof(*ctxt));

    return rc;

}

int xc_domain_irq_permission(int xc_handle,
                             uint32_t domid,
                             uint8_t pirq,
                             uint8_t allow_access)
{
    dom0_op_t op;

    op.cmd = DOM0_IRQ_PERMISSION;
    op.u.irq_permission.domain = domid;
    op.u.irq_permission.pirq = pirq;
    op.u.irq_permission.allow_access = allow_access;

    return do_dom0_op(xc_handle, &op);
}

int xc_domain_iomem_permission(int xc_handle,
                               uint32_t domid,
                               unsigned long first_mfn,
                               unsigned long nr_mfns,
                               uint8_t allow_access)
{
    dom0_op_t op;

    op.cmd = DOM0_IOMEM_PERMISSION;
    op.u.iomem_permission.domain = domid;
    op.u.iomem_permission.first_mfn = first_mfn;
	op.u.iomem_permission.nr_mfns = nr_mfns;
    op.u.iomem_permission.allow_access = allow_access;

    return do_dom0_op(xc_handle, &op);
}

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
