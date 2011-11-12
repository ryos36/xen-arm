/*
 * Copyright (C) 2006 Hollis Blanchard <hollisb@us.ibm.com>, IBM Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef __ARM_GUEST_ACCESS_H__
#define __ARM_GUEST_ACCESS_H__

#include <asm/uaccess.h>

/* Is the guest handle a NULL reference? */
#define guest_handle_is_null(hnd)        ((hnd).p == NULL)

/* Offset the given guest handle into the array it refers to. */
#define guest_handle_add_offset(hnd, nr) ((hnd).p += (nr))

/* Cast a guest handle to the specified type of handle. */
#define guest_handle_cast(hnd, type) ({         \
    type *_x = (hnd).p;                         \
    (XEN_GUEST_HANDLE(type)) { _x };                \
})

/*
 * Pre-validate a guest handle.
 * Allows use of faster __copy_* functions.
 */
#define guest_handle_okay(hnd, nr)                      \
    array_access_ok((hnd).p, (nr), sizeof(*(hnd).p))
 
    
/*
 * Copy an array of objects to guest context via a guest handle.
 * Optionally specify an offset into the guest array.
 */
#define copy_to_guest_offset(hnd, idx, ptr, nr) \
    __copy_to_guest_offset(hnd, idx, ptr, nr)

  
/*
 * Copy an array of objects from guest context via a guest handle.
 * Optionally specify an offset into the guest array.
 */
#define copy_from_guest_offset(ptr, hnd, idx, nr) \
    __copy_from_guest_offset(ptr, hnd, idx, nr)
    
    
/* Copy sub-field of a structure to guest context via a guest handle. */
#define copy_field_to_guest(hnd, ptr, field) \
    __copy_field_to_guest(hnd, ptr, field)

/* Copy sub-field of a structure from guest context via a guest handle. */
#define copy_field_from_guest(ptr, hnd, field) \
    __copy_field_from_guest(ptr, hnd, field)
    
#define __copy_to_guest_offset(hnd, off, ptr, nr) ({    \
    const typeof(ptr) _x = (hnd).p;                     \
    const typeof(ptr) _y = (ptr);                       \
    __copy_to_user(_x+(off), _y, sizeof(*_x)*(nr));     \
})

#define __copy_from_guest_offset(ptr, hnd, off, nr) ({  \
    const typeof(ptr) _x = (hnd).p;                     \
    const typeof(ptr) _y = (ptr);                       \
    __copy_from_user(_y, _x+(off), sizeof(*_x)*(nr));   \
})

#define __copy_field_to_guest(hnd, ptr, field) ({       \
    const typeof(&(ptr)->field) _x = &(hnd).p->field;   \
    const typeof(&(ptr)->field) _y = &(ptr)->field;     \
    __copy_to_user(_x, _y, sizeof(*_x));                \
})

#define __copy_field_from_guest(ptr, hnd, field) ({     \
    const typeof(&(ptr)->field) _x = &(hnd).p->field;   \
    const typeof(&(ptr)->field) _y = &(ptr)->field;     \
    __copy_from_user(_y, _x, sizeof(*_x));              \
})

#endif
