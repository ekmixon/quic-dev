/*
 * include/proto/quic_conn.h
 * This file contains definitions for QUIC connections.
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

#ifndef _PROTO_QUIC_FRAME_H
#define _PROTO_QUIC_FRAME_H

#include <types/quic_frame.h>

#include <proto/quic_conn.h>

/*
 * Encode <frm> PADDING frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static int inline quic_build_padding_frame(unsigned char **buf, const unsigned char *end,
                                           struct quic_frame *frm)
{
	struct quic_padding *padding = &frm->padding;

	if (end - *buf < padding->len - 1)
		return 0;

	memset(*buf, 0, padding->len - 1);
	*buf += padding->len - 1;

	return 1;
}

/*
 * Parse a PADDING frame from <buf> buffer with <end> as end into <frm> frame.
 */
static int inline quic_parse_padding_frame(struct quic_frame *frm,
                                           const unsigned char **buf, const unsigned char *end)
{
	const unsigned char *beg;
	struct quic_padding *padding = &frm->padding;

	beg = *buf;
	padding->len = 1;
	while (*buf != end && !**buf)
		(*buf)++;
	padding->len += *buf - beg;

	return 1;
}

/*
 * Encode a ACK frame into <buf> buffer.
 * Always succeeds.
 */
static int inline quic_build_ping_frame(unsigned char **buf, const unsigned char *end,
                                        struct quic_frame *frm)
{
	/* No field */
	return 1;
}

/*
 * Parse a PADDING frame from <buf> buffer with <end> as end into <frm> frame.
 */
static int inline quic_parse_ping_frame(struct quic_frame *frm,
                                        const unsigned char **buf, const unsigned char *end)
{
	/* No field */
	return 1;
}

/*
 * Encode a ACK frame.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static int inline quic_build_ack_frame(unsigned char **buf, const unsigned char *end,
                                       struct quic_frame *frm)
{
	struct quic_ack *ack = &frm->ack;

	return quic_enc_int(buf, end, ack->largest_ack) &&
		quic_enc_int(buf, end, ack->ack_delay) &&
		quic_enc_int(buf, end, ack->first_ack_range) &&
		quic_enc_int(buf, end, ack->ack_range_num);
}

/*
 * Parse an ACK frame from <buf> buffer with <end> as end into <frm> frame.
 */
static int inline quic_parse_ack_frame(struct quic_frame *frm,
                                       const unsigned char **buf, const unsigned char *end)
{
	return 1;
}

/*
 * Encode a ACK_ECN frame.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static int inline quic_build_ack_ecn_frame(unsigned char **buf, const unsigned char *end,
                                           struct quic_frame *frm)
{
	struct quic_ack *ack = &frm->ack;

	return quic_enc_int(buf, end, ack->largest_ack) &&
		quic_enc_int(buf, end, ack->ack_delay) &&
		quic_enc_int(buf, end, ack->first_ack_range) &&
		quic_enc_int(buf, end, ack->ack_range_num);
}

/*
 * Parse an ACK_ECN frame from <buf> buffer with <end> as end into <frm> frame.
 */
static int inline quic_parse_ack_ecn_frame(struct quic_frame *frm,
                                           const unsigned char **buf, const unsigned char *end)
{
	return 1;
}

/*
 * Encode a RESET_STREAM frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static int inline quic_build_reset_stream_frame(unsigned char **buf, const unsigned char *end,
                                                struct quic_frame *frm)
{
	struct quic_reset_stream *reset_stream = &frm->reset_stream;

	return quic_enc_int(buf, end, reset_stream->id) &&
		quic_enc_int(buf, end, reset_stream->app_error_code) &&
		quic_enc_int(buf, end, reset_stream->final_size);
}

/*
 * Parse a RESET_STREAM frame from <buf> buffer with <end> as end into <frm> frame.
 */
static int inline quic_parse_reset_stream_frame(struct quic_frame *frm,
                                               const unsigned char **buf, const unsigned char *end)
{
	return 1;
}

/*
 * Encode a STOP_SENDING frame.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static int inline quic_build_stop_sending_frame(unsigned char **buf, const unsigned char *end,
                                                struct quic_frame *frm)
{
	struct quic_stop_sending_frame *stop_sending_frame = &frm->stop_sending_frame;

	return quic_enc_int(buf, end, stop_sending_frame->id) &&
		quic_enc_int(buf, end, stop_sending_frame->app_error_code);
}

/*
 * Encode a CRYPTO frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static int inline quic_build_crypto_frame(unsigned char **buf, const unsigned char *end,
                                          struct quic_frame *frm)
{
	struct quic_crypto *crypto = &frm->crypto;

	if (!quic_enc_int(buf, end, crypto->offset) ||
	    !quic_enc_int(buf, end, crypto->len) || end - *buf < crypto->len)
		return 0;

	memcpy(*buf, crypto->data, crypto->len);
	*buf += crypto->len;

	return 1;
}

/*
 * Encode a NEW_TOKEN frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static int inline quic_build_new_token_frame(unsigned char **buf, const unsigned char *end,
                                             struct quic_frame *frm)
{
	struct quic_new_token *new_token = &frm->new_token;

	if (!quic_enc_int(buf, end, new_token->len) || end - *buf < new_token->len)
		return 0;

	memcpy(*buf, new_token->data, new_token->len);

	return 1;
}

/*
 * Encode a STREAM frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static int inline quic_build_stream_frame(unsigned char **buf, const unsigned char *end,
                                          struct quic_frame *frm)
{
	struct quic_stream *stream = &frm->stream;

	if (!quic_enc_int(buf, end, stream->id) ||
	    ((frm->type & QUIC_STREAM_FRAME_OFF_BIT) && !quic_enc_int(buf, end, stream->offset)) ||
	    ((frm->type & QUIC_STREAM_FRAME_LEN_BIT) &&
	     (!quic_enc_int(buf, end, stream->len) || end - *buf < stream->len)))
		return 0;

	memcpy(*buf, stream->data, stream->len);
	*buf += stream->len;

	return 1;
}

/*
 * Encode a MAX_DATA frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_max_data_frame(unsigned char **buf, const unsigned char *end,
                                            struct quic_frame *frm)
{
	struct quic_max_data *max_data = &frm->max_data;

	return quic_enc_int(buf, end, max_data->max_data);
}

/*
 * Encode a MAX_STREAM_DATA frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_max_stream_data_frame(unsigned char **buf, const unsigned char *end,
                                                   struct quic_frame *frm)
{
	struct quic_max_stream_data *max_stream_data = &frm->max_stream_data;

	return quic_enc_int(buf, end, max_stream_data->id) &&
		quic_enc_int(buf, end, max_stream_data->max_stream_data);
}

/*
 * Encode a MAX_STREAMS frame for bidirectional streams into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_max_streams_bidi_frame(unsigned char **buf, const unsigned char *end,
                                                    struct quic_frame *frm)
{
	struct quic_max_streams *max_streams_bidi = &frm->max_streams_bidi;

	return quic_enc_int(buf, end, max_streams_bidi->max_streams);
}

/*
 * Encode a MAX_STREAMS frame for unidirectional streams into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_max_streams_uni_frame(unsigned char **buf, const unsigned char *end,
                                                   struct quic_frame *frm)
{
	struct quic_max_streams *max_streams_uni = &frm->max_streams_uni;

	return quic_enc_int(buf, end, max_streams_uni->max_streams);
}

/*
 * Encode a DATA_BLOCKED frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_data_blocked_frame(unsigned char **buf, const unsigned char *end,
                                                struct quic_frame *frm)
{
	struct quic_data_blocked *data_blocked = &frm->data_blocked;

	return quic_enc_int(buf, end, data_blocked->limit);
}

/*
 * Encode a STREAM_DATA_BLOCKED into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_stream_data_blocked_frame(unsigned char **buf, const unsigned char *end,
                                                       struct quic_frame *frm)
{
	struct quic_stream_data_blocked *stream_data_blocked = &frm->stream_data_blocked;

	return quic_enc_int(buf, end, stream_data_blocked->id) &&
		quic_enc_int(buf, end, stream_data_blocked->limit);
}

/*
 * Encode a STREAMS_BLOCKED frame for bidirectional streams into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_streams_blocked_bidi_frame(unsigned char **buf, const unsigned char *end,
                                                        struct quic_frame *frm)
{
	struct quic_streams_blocked *streams_blocked_bidi = &frm->streams_blocked_bidi;

	return quic_enc_int(buf, end, streams_blocked_bidi->limit);
}

/*
 * Encode a STREAMS_BLOCKED frame for unidirectional streams into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_streams_blocked_uni_frame(unsigned char **buf, const unsigned char *end,
                                                       struct quic_frame *frm)
{
	struct quic_streams_blocked *streams_blocked_uni = &frm->streams_blocked_uni;

	return quic_enc_int(buf, end, streams_blocked_uni->limit);
}

/*
 * Encode a NEW_CONNECTION_ID frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_new_connection_id_frame(unsigned char **buf, const unsigned char *end,
                                                     struct quic_frame *frm)
{
	struct quic_new_connection_id *new_cid = &frm->new_connection_id;

	if (!quic_enc_int(buf, end, new_cid->seq_num) ||
	    !quic_enc_int(buf, end, new_cid->retire_prior_to) ||
	    end - *buf < sizeof_quic_cid(&new_cid->cid) + sizeof new_cid->stateless_reset_token)
		return 0;

	*(*buf)++ = new_cid->cid.len;

	memcpy(*buf, new_cid->cid.data, new_cid->cid.len);
	*buf += new_cid->cid.len;

	memcpy(*buf, new_cid->stateless_reset_token, sizeof new_cid->stateless_reset_token);
	*buf += sizeof new_cid->stateless_reset_token;

	return 0;
}

/*
 * Encode a RETIRE_CONNECTION_ID frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_retire_connection_id_frame(unsigned char **buf, const unsigned char *end,
                                                        struct quic_frame *frm)
{
	struct quic_retire_connection_id *retire_connection_id = &frm->retire_connection_id;

	return quic_enc_int(buf, end, retire_connection_id->seq_num);
}

/*
 * Encode a PATH_CHALLENGE frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_path_challenge_frame(unsigned char **buf, const unsigned char *end,
                                                  struct quic_frame *frm)
{
	struct quic_path_challenge *path_challenge = &frm->path_challenge;

	if (end - *buf < sizeof path_challenge->data)
		return 0;

	memcpy(*buf, path_challenge->data, sizeof path_challenge->data);
	*buf += sizeof path_challenge->data;

	return 1;
}

/*
 * Encode a PATH_RESPONSE frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_path_response_frame(unsigned char **buf, const unsigned char *end,
                                                 struct quic_frame *frm)
{
	struct quic_path_challenge_response *path_challenge_response = &frm->path_challenge_response;

	if (end - *buf < sizeof path_challenge_response->data)
		return 0;

	memcpy(*buf, path_challenge_response->data, sizeof path_challenge_response->data);
	*buf += sizeof path_challenge_response->data;

	return 1;
}

/*
 * Encode a CONNECTION_CLOSE frame at QUIC layer into <buf> buffer.
 * Note there exist two types of CONNECTION_CLOSE frame, one for the application layer
 * and another at QUIC layer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_connection_close_frame(unsigned char **buf, const unsigned char *end,
                                                    struct quic_frame *frm)
{
	struct quic_connection_close *connection_close = &frm->connection_close;

	if (!quic_enc_int(buf, end, connection_close->error_code) ||
	    !quic_enc_int(buf, end, connection_close->frame_type) ||
	    !quic_enc_int(buf, end, connection_close->reason_phrase_len) ||
	    end - *buf < connection_close->reason_phrase_len)
		return 0;

	memcpy(*buf, connection_close->reason_phrase, connection_close->reason_phrase_len);
	*buf += connection_close->reason_phrase_len;

	return 1;
}

/*
 * Encode a CONNECTION_CLOSE frame at application layer into <buf> buffer.
 * Note there exist two types of CONNECTION_CLOSE frame, one for application layer
 * and another at QUIC layer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_connection_close_app_frame(unsigned char **buf, const unsigned char *end,
                                                        struct quic_frame *frm)
{
	struct quic_connection_close *connection_close_app = &frm->connection_close_app;

	if (!quic_enc_int(buf, end, connection_close_app->error_code) ||
	    !quic_enc_int(buf, end, connection_close_app->reason_phrase_len) ||
	    end - *buf < connection_close_app->reason_phrase_len)
		return 0;

	memcpy(*buf, connection_close_app->reason_phrase, connection_close_app->reason_phrase_len);
	*buf += connection_close_app->reason_phrase_len;

	return 1;
}

/*
 * Encode <frm> QUIC frame into <buf> buffer.
 * Returns 1 if succeded (enough room in <buf> to encode the frame), 0 if not.
 */
static inline int quic_build_frame(unsigned char **buf, const unsigned char *end,
                                   struct quic_frame *frm)
{
	if (end <= *buf)
		return 0;

	*(*buf)++ = frm->type;

	return quic_build_frame_funcs[frm->type](buf, end, frm);
}

/*
 * Decode a QUIC frame from <buf> buffer into <frm> frame.
 * Returns 1 if succeded (enough data to parse the frame), 0 if not.
 */
static inline int quic_parse_frame(struct quic_frame *frm,
                                   const unsigned char **buf, const unsigned char *end)
{
	if (end <= *buf)
		return 0;

	frm->type = *(*buf)++;

	return quic_parse_frame_funcs[frm->type](frm, buf, end);
}

#endif /* _PROTO_QUIC_FRAME_H */
