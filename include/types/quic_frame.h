/*
 * include/types/quic_frame.h
 * This file contains QUIC frame definitions.
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

#ifndef _TYPES_QUIC_FRAME_H
#define _TYPES_QUIC_FRAME_H

#include <stdint.h>
#include <stdlib.h>

#include <types/quic_conn.h>

/* QUIC frame types. */
#define QUIC_FT_PADDING              0x00
#define QUIC_FT_PING                 0x01
#define QUIC_FT_ACK                  0x02
#define QUIC_FT_ACK_ECN              0x03
#define QUIC_FT_RESET_STREAM         0x04
#define QUIC_FT_STOP_SENDING         0x05
#define QUIC_FT_CRYPTO               0x06
#define QUIC_FT_NEW_TOKEN            0x07

#define QUIC_FT_STREAM_8             0x08
#define QUIC_FT_STREAM_9             0x09
#define QUIC_FT_STREAM_A             0x0a
#define QUIC_FT_STREAM_B             0x0b
#define QUIC_FT_STREAM_C             0x0c
#define QUIC_FT_STREAM_D             0x0d
#define QUIC_FT_STREAM_E             0x0e
#define QUIC_FT_STREAM_F             0x0f

#define QUIC_FT_MAX_DATA             0x10
#define QUIC_FT_MAX_STREAM_DATA      0x11
#define QUIC_FT_MAX_STREAMS_BIDI     0x12
#define QUIC_FT_MAX_STREAMS_UNI      0x13
#define QUIC_FT_DATA_BLOCKED         0x14
#define QUIC_FT_STREAM_DATA_BLOCKED  0x15
#define QUIC_FT_STREAMS_BLOCKED_BIDI 0x16
#define QUIC_FT_STREAMS_BLOCKED_UNI  0x17
#define QUIC_FT_NEW_CONNECTION_ID    0x18
#define QUIC_FT_RETIRE_CONNECTION_ID 0x19
#define QUIC_FT_PATH_CHALLENGE       0x1a
#define QUIC_FT_PATH_RESPONSE        0x1b
#define QUIC_FT_CONNECTION_CLOSE     0x1c
#define QUIC_FT_CONNECTION_CLOSE_APP 0x1d

#define QUIC_STREAM_FRAME_FIN_BIT    0x01
#define QUIC_STREAM_FRAME_LEN_BIT    0x02
#define QUIC_STREAM_FRAME_OFF_BIT    0x04

#define QUIC_PATH_CHALLENGE_LEN         8

struct quic_padding {
	size_t len;
};

struct quic_ack_range {
	uint64_t gap;
	uint64_t ack_range;
};

struct quic_ack {
	uint64_t largest_ack;
	uint64_t ack_delay;
	size_t ack_range_num;
	uint64_t first_ack_range;
};

struct quic_crypto {
	uint64_t offset;
	uint64_t len;
	const unsigned char *data;
};

struct quic_reset_stream {
	uint64_t id;
	uint64_t app_error_code;
	uint64_t final_size;
};

struct quic_stop_sending_frame {
	uint64_t id;
	uint64_t app_error_code;
};

struct quic_new_token {
	size_t len;
	const unsigned char *data;
};

struct quic_stream {
	uint64_t id;
	uint64_t offset;
	uint64_t len;
	const unsigned char *data;
};

struct quic_max_data {
	uint64_t max_data;
};

struct quic_max_stream_data {
	uint64_t id;
	uint64_t max_stream_data;
};

struct quic_max_streams {
	uint64_t max_streams;
};

struct quic_data_blocked {
	uint64_t limit;
};

struct quic_stream_data_blocked {
	uint64_t id;
	uint64_t limit;
};

struct quic_streams_blocked {
	uint64_t limit;
};

struct quic_new_connection_id {
	uint64_t seq_num;
	uint64_t retire_prior_to;
	struct quic_cid cid;
	unsigned char stateless_reset_token[QUIC_STATELESS_RESET_TOKEN_LEN];
};

struct quic_retire_connection_id {
	uint64_t seq_num;
};

struct quic_path_challenge {
	unsigned char data[QUIC_PATH_CHALLENGE_LEN];
};

struct quic_path_challenge_response {
	unsigned char data[QUIC_PATH_CHALLENGE_LEN];
};

struct quic_connection_close {
	uint64_t error_code;
	uint64_t frame_type;
	size_t reason_phrase_len;
	unsigned char *reason_phrase;
};

struct quic_frame {
	unsigned char type;
	union {
		struct quic_padding padding;
		struct quic_ack ack;
		struct quic_crypto crypto;
		struct quic_reset_stream reset_stream;
		struct quic_stop_sending_frame stop_sending_frame;
		struct quic_new_token new_token;
		struct quic_stream stream;
		struct quic_max_data max_data;
		struct quic_max_stream_data max_stream_data;
		struct quic_max_streams max_streams_bidi;
		struct quic_max_streams max_streams_uni;
		struct quic_data_blocked data_blocked;
		struct quic_stream_data_blocked stream_data_blocked;
		struct quic_streams_blocked streams_blocked_bidi;
		struct quic_streams_blocked streams_blocked_uni;
		struct quic_new_connection_id new_connection_id;
		struct quic_retire_connection_id retire_connection_id;
		struct quic_path_challenge path_challenge;
		struct quic_path_challenge_response path_challenge_response;
		struct quic_connection_close connection_close;
		struct quic_connection_close connection_close_app;
	};
};

extern int (*quic_build_frame_funcs[])(unsigned char **buf, const unsigned char *end,
                                       struct quic_frame *frame);

extern int (*quic_parse_frame_funcs[])(struct quic_frame *frame,
                                       const unsigned char **buf, const unsigned char *end);
#endif /* _TYPES_QUIC_FRAME_H */
