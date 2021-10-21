/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc_message_storage.h"

bool_t
xdr_user_message (XDR *xdrs, user_message *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->result))
		 return FALSE;
	 if (!xdr_opaque (xdrs, objp->message, 256))
		 return FALSE;
	 if (!xdr_opaque (xdrs, objp->sender, 256))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_rpc_retrieve_user_message_1_argument (XDR *xdrs, rpc_retrieve_user_message_1_argument *objp)
{
	 if (!xdr_string (xdrs, &objp->username, ~0))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->message_identifier))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_store_user_message_1_argument (XDR *xdrs, store_user_message_1_argument *objp)
{
	 if (!xdr_string (xdrs, &objp->username, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->username_sender, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->message, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_delete_user_message_1_argument (XDR *xdrs, delete_user_message_1_argument *objp)
{
	 if (!xdr_string (xdrs, &objp->username, ~0))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->message_identifier))
		 return FALSE;
	return TRUE;
}