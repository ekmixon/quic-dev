/*
 * include/haproxy/mux_quic-t.h
 * This file containts types for QUIC mux-demux.
 *
 * Copyright 2021 HAProxy Technologies, Frédéric Lécaille <flecaille@haproxy.com>
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

#ifndef _HAPROXY_MUX_QUIC_T_H
#define _HAPROXY_MUX_QUIC_T_H
#ifdef USE_QUIC
#ifndef USE_OPENSSL
#error "Must define USE_OPENSSL"
#endif

#include <stdint.h>

#include <haproxy/buf-t.h>
#include <haproxy/connection-t.h>
#include <haproxy/dynbuf-t.h>

#include <import/eb64tree.h>

/* Bit shift to get the stream sub ID for internal use which is obtained
 * shifting the stream IDs by this value, knowing that the
 * QCS_ID_TYPE_SHIFT less significant bits identify the stream ID
 * types (client initiated bidirectional, server initiated bidirectional,
 * client initiated unidirectional, server initiated bidirectional).
 * Note that there is no reference to such stream sub IDs in the RFC.
 */
#define QCS_ID_TYPE_MASK         0x3
#define QCS_ID_TYPE_SHIFT          2
/* The less significant bit of a stream ID is set for a server initiated stream */
#define QCS_ID_SRV_INTIATOR_BIT  0x1
/* This bit is set for unidirectional streams */
#define QCS_ID_DIR_BIT           0x2
#define QCS_ID_DIR_BIT_SHIFT       1

#define OUQS_SF_TXBUF_MALLOC 0x00000001
#define OUQS_SF_TXBUF_FULL   0x00000002

extern struct pool_head *pool_head_qcs;

/* Stream types */
enum qcs_type {
	QCS_CLT_BIDI,
	QCS_SRV_BIDI,
	QCS_CLT_UNI,
	QCS_SRV_UNI,
	/* Must be the last one */
	QCS_MAX_TYPES,
};

/* 32 buffers: one for the ring's root, rest for the mbuf itself */
#define QCC_MBUF_CNT 32

/* Stream direction types */
enum qcs_dir {
	QCS_BIDI    = 0,
	QCS_UNI     = 1,
	/* Must be the last one */
	QCS_MAX_DIR = 2,
};

/* QUIC connection state, in qcc->st0 */
enum qc_cs {
	/* Initial state */
	QC_CS_NOERR,
	QC_CS_ERROR,
};

/* QUIC connection descriptor */
struct qcc {
	struct connection *conn; /* mux state */
	enum qc_cs st0; /* connection flags: QC_CF_* */
	unsigned int errcode;
	uint32_t flags;
	/* Stream information, one by direction and by initiator */
	struct {
		uint64_t max_streams; /* maximum number of concurrent streams */
		uint64_t largest_id;  /* Largest ID of the open streams */
		uint64_t nb_streams;  /* Number of open streams */
		struct {
			uint64_t max_data; /* Maximum number of bytes which may be received */
			uint64_t bytes;    /* Number of bytes received */
		} rx;
		struct {
			uint64_t max_data; /* Maximum number of bytes which may be sent */
			uint64_t bytes;    /* Number of bytes sent */
		} tx;
	} strms[QCS_MAX_TYPES];
	struct {
		uint64_t max_data; /* Maximum number of bytes which may be received */
		uint64_t bytes;    /* Number of bytes received */
		uint64_t inmux;    /* Number of bytes received but not already demultiplexed. */
	} rx;
	struct {
		uint64_t max_data; /* Maximum number of bytes which may be sent */
		uint64_t bytes;    /* Number of bytes sent */
	} tx;

	struct eb_root streams_by_id; /* all active streams by their ID */

	/* states for the mux direction */
	struct buffer mbuf[QCC_MBUF_CNT];   /* mux buffers (ring) */

	int timeout;        /* idle timeout duration in ticks */
	int shut_timeout;   /* idle timeout duration in ticks after GOAWAY was sent */
	unsigned int nb_cs;       /* number of attached conn_streams */
	unsigned int stream_cnt;  /* total number of streams seen */
	struct proxy *proxy; /* the proxy this connection was created for */
	struct task *task;  /* timeout management task */
	struct qc_counters *px_counters; /* quic counters attached to proxy */
	struct list send_list; /* list of blocked streams requesting to send */
	struct list fctl_list; /* list of streams blocked by connection's fctl */
	struct list blocked_list; /* list of streams blocked for other reasons (e.g. sfctl, dep) */
	struct buffer_wait buf_wait; /* wait list for buffer allocations */
	struct wait_event wait_event;  /* To be used if we're waiting for I/Os */
	struct mt_list qcs_rxbuf_wlist; /* list of streams waiting for their rxbuf */
	void *ctx; /* Application layer context */
	const struct qcc_app_ops *app_ops;
};

/* QUIC RX states */
enum qcs_rx_st {
	QC_RX_SS_IDLE = 0,   // idle
	QC_RX_SS_RECV,       // receive
	QC_RX_SS_SIZE_KNOWN, // stream size known
	/* Terminal states */
	QC_RX_SS_DATA_RECVD, // all data received
	QC_RX_SS_DATA_READ,  // app. read all data
	QC_RX_SS_RST_RECVD,  // reset received
	QC_RX_SS_RST_READ,   // app. read reset
};

/* QUIC TX states */
enum qcs_tx_st {
	QC_TX_SS_IDLE = 0,
	QC_TX_SS_READY,      // ready
	QC_TX_SS_SEND,       // send
	QC_TX_SS_DATA_SENT,  // all data sent
	/* Terminal states */
	QC_TX_SS_DATA_RECVD, // all data received
	QC_TX_SS_RST_SENT,   // reset sent
	QC_TX_SS_RST_RECVD,  // reset received
};

#define QCS_MBUF_CNT 32

/* QUIC stream descriptor, describing the stream as it appears in the QUIC_CONN, and as
 * it is being processed in the internal HTTP representation (HTX).
 */
struct qcs {
	struct conn_stream *cs;
	struct session *sess;
	struct qcc *qcc;
	struct eb64_node by_id; /* place in qcc's streams_by_id */
	struct eb_root frms;
	uint64_t id; /* stream ID */
	uint32_t flags;      /* QC_SF_* */
	struct {
		enum qcs_rx_st st; /* RX state */
		uint64_t max_data; /* maximum number of bytes which may be received */
		uint64_t offset;   /* the current offset of received data */
		uint64_t bytes;    /* number of bytes received */
		struct buffer buf; /* receive buffer, always valid (buf_empty or real buffer) */
	} rx;
	struct {
		enum qcs_tx_st st; /* TX state */
		uint64_t max_data; /* maximum number of bytes which may be sent */
		uint64_t offset;   /* the current offset of data to send */
		uint64_t bytes;    /* number of bytes sent */
		struct buffer buf; /* transmit buffer, always valid (buf_empty or real buffer) */
		struct buffer rbuf[QCS_MBUF_CNT];
	} tx;
	struct wait_event *subs;  /* recv wait_event the conn_stream associated is waiting on (via qc_subscribe) */
	struct list list; /* To be used when adding in qcc->send_list or qcc->fctl_lsit */
	struct tasklet *shut_tl;  /* deferred shutdown tasklet, to retry to send an RST after we failed to,
				   * in case there's no other subscription to do it */
	uint16_t status;
};

/* QUIC application layer operations */
struct qcc_app_ops {
	int (*init)(struct qcc *qcc);
	int (*attach_ruqs)(struct qcs *qcs, void *ctx);
	int (*decode_qcs)(struct qcs *qcs, void *ctx);
	int (*finalize)(void *ctx);
};

#endif /* USE_QUIC */
#endif /* _HAPROXY_MUX_QUIC_T_H */
