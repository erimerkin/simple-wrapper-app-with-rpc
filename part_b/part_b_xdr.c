/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "part_b.h"

bool_t
xdr_operands (XDR *xdrs, operands *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->executable_path, ~0))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->a))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->b))
		 return FALSE;
	return TRUE;
}
