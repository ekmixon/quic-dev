/*
 * include/proto/quic_cc.h
 * This file contains prototypes for QUIC congestion control.
 *
 * Copyright 2019 HAProxy Technologies, Frédéric Lécaille <flecaille@haproxy.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, version 2.1
 * exclusively.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _PROTO_QUIC_CC_H
#define _PROTO_QUIC_CC_H

#include <common/buf.h>
#include <common/chunk.h>

#include <types/quic_cc.h>
#include <types/xprt_quic.h>

void quic_cc_init(struct quic_cc *cc, struct quic_cc_algo *algo, struct quic_conn *qc);
void quic_cc_event(struct quic_cc *cc, struct quic_cc_event *ev);
void quic_cc_state_trace(struct buffer *buf, const struct quic_cc *cc);

static inline const char *quic_cc_state_str(enum quic_cc_algo_state_type state)
{
	switch (state) {
	case QUIC_CC_ST_SS:
		return "ss";
	case QUIC_CC_ST_CA:
		return "ca";
	default:
		return "unknown";
	}
}

/* Return a human readable string from <ev> control congestion event type. */
static inline void quic_cc_event_trace(struct buffer *buf, const struct quic_cc_event *ev)
{
	chunk_appendf(buf, " event type=");
	switch (ev->type) {
	case QUIC_CC_EVT_ACK:
		chunk_appendf(buf, "ack acked=%lu time_sent:%lu", ev->ack.acked, ev->ack.time_sent);
		break;
	case QUIC_CC_EVT_LOSS:
		chunk_appendf(buf, "loss now_us=%lu max_ack_delay_us=%lu lost_bytes=%lu"
		              " time_sent=%lu period=%lu",
		              ev->loss.now_us, ev->loss.max_ack_delay_us, ev->loss.lost_bytes,
		              ev->loss.newest_time_sent, ev->loss.period);
		break;
	case QUIC_CC_EVT_ECN_CE:
		chunk_appendf(buf, "ecn_ce");
		break;
	}
}

#endif /* _PROTO_QUIC_CC_H */
