/*
 * Copyright (c) 2004,2005 Voltaire Inc.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 * $Id$
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#include <umad.h>
#include "mad.h"

#undef DEBUG
#define DEBUG 	if (ibdebug)	WARN

uint8 *
port_performance_query(void *rcvbuf, ib_portid_t *dest, int port, uint timeout)
{
	ib_rpc_t rpc = {0};
	int lid = dest->lid;

	DEBUG("lid %d port %d", lid, port);

	if (lid == -1) {
		WARN("only lid routes are supported");
		return 0;
	}

	rpc.mgtclass = IB_PERFORMANCE_CLASS;
	rpc.method = IB_MAD_METHOD_GET;
	rpc.attr.id = IB_GSI_PORT_COUNTERS;

	mad_set_field(rcvbuf, 0, IB_PC_PORT_SELECT_F, port);
	rpc.attr.mod = 0;
	rpc.timeout = timeout;
	rpc.datasz = IB_PC_DATA_SZ;
	rpc.dataoffs = IB_PC_DATA_OFFS;

	dest->qp = 1;
	if (!dest->qkey)
		dest->qkey = IB_DEFAULT_QP1_QKEY;

	return madrpc(&rpc, dest, rcvbuf, rcvbuf); 
}

uint8 *
port_performance_reset(void *rcvbuf, ib_portid_t *dest, int port, uint mask, uint timeout)
{
	ib_rpc_t rpc = {0};
	int lid = dest->lid;

	DEBUG("lid %d port %d mask 0x%x", lid, port, mask);

	if (lid == -1) {
		WARN("only lid routed is supported");
		return 0;
	}

	if (!mask)
		mask = ~0;
	
	rpc.mgtclass = IB_PERFORMANCE_CLASS;
	rpc.method = IB_MAD_METHOD_SET;
	rpc.attr.id = IB_GSI_PORT_COUNTERS;

	memset(rcvbuf, 0, IB_MAD_SIZE);
	
	mad_set_field(rcvbuf, 0, IB_PC_PORT_SELECT_F, port);
	mad_set_field(rcvbuf, 0, IB_PC_COUNTER_SELECT_F, mask);
	rpc.attr.mod = 0;
	rpc.timeout = timeout;
	rpc.datasz = IB_PC_DATA_SZ;
	rpc.dataoffs = IB_PC_DATA_OFFS;

	dest->qp = 1;
	if (!dest->qkey)
		dest->qkey = IB_DEFAULT_QP1_QKEY;

	return madrpc(&rpc, dest, rcvbuf, rcvbuf); 
}
