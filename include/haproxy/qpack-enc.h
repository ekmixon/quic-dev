#ifndef QPACK_ENC_H_
#define QPACK_ENC_H_

struct buffer;

int qpack_encode_field_section_line(struct buffer *out);
int qpack_encode_int_status(struct buffer *out, unsigned int status);

#endif /* QPACK_ENC_H_ */
