/*
 * include/haproxy/h3.h
 * This file containts types for H3
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

#ifndef _HAPROXY_H3_T_H
#define _HAPROXY_H3_T_H
#ifdef USE_QUIC
#ifndef USE_OPENSSL
#error "Must define USE_OPENSSL"
#endif

#include <haproxy/buf-t.h>

/* H3 unidirectional stream types (does not exist for bidirectional streams) */
#define H3_UNI_STRM_TP_CONTROL_STREAM 0x00
#define H3_UNI_STRM_TP_PUSH_STREAM    0x01
#define H3_UNI_STRM_TP_QPACK_ENCODER  0x02
#define H3_UNI_STRM_TP_QPACK_DECODER  0x03
/* Must be the last one */
#define H3_UNI_STRM_TP_MAX            H3_UNI_STRM_TP_QPACK_DECODER

/* Errors. */
enum h3_err {
	H3_NO_ERROR                = 0x100,
	H3_GENERAL_PROTOCOL_ERROR  = 0x101,
	H3_INTERNAL_ERROR          = 0x102,
	H3_STREAM_CREATION_ERROR   = 0x103,
	H3_CLOSED_CRITICAL_STREAM  = 0x104,
	H3_FRAME_UNEXPECTED        = 0x105,
	H3_FRAME_ERROR             = 0x106,
	H3_EXCESSIVE_LOAD          = 0x107,
	H3_ID_ERROR                = 0x108,
	H3_SETTINGS_ERROR          = 0x109,
	H3_MISSING_SETTINGS        = 0x10a,
	H3_REQUEST_REJECTED        = 0x10b,
	H3_REQUEST_CANCELLED       = 0x10c,
	H3_REQUEST_INCOMPLETE      = 0x10d,
	H3_MESSAGE_ERROR           = 0x10e,
	H3_CONNECT_ERROR           = 0x10f,
	H3_VERSION_FALLBACK        = 0x110,

	QPACK_DECOMPRESSION_FAILED = 0x200,
	QPACK_ENCODER_STREAM_ERROR = 0x201,
	QPACK_DECODER_STREAM_ERROR = 0x202,
};

/* Frame types. */
enum h3_ft       {
	H3_FT_DATA         = 0x00,
	H3_FT_HEADERS      = 0x01,
	H3_FT_CANCEL_PUSH  = 0x02,
	H3_FT_SETTINGS     = 0x03,
	H3_FT_PUSH_PROMISE = 0x04,
	H3_FT_GOAWAY       = 0x05,
	H3_FT_MAX_PUSH_ID  = 0x06,
};

extern const struct qcc_app_ops h3_ops;

#endif /* USE_QUIC */
#endif /* _HAPROXY_H3_T_H */
