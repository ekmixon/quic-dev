#include <haproxy/qpack-enc.h>

#include <haproxy/buf.h>

int qpack_encode_field_section_line(struct buffer *out)
{
	char qpack_field_section[] = {
	  '\x00',   /* required insert count */
	  '\x00',   /* S + delta base */
	};
	b_putblk(out, qpack_field_section, 2);

	return 2;
}

int qpack_encode_int_status(struct buffer *out, unsigned int status)
{
	switch (status) {
	case 200:
		b_putchr(out, 0xd9);
		return 1;

	case 302:
		b_putchr(out, 0xff);
		b_putchr(out, 0x03);
		return 2;

	case 400:
		b_putchr(out, 0xff);
		b_putchr(out, 0x04);
		return 2;

	case 503:
		b_putchr(out, 0xdc);
		return 1;

	default:
		BUG_ON(1);
		return 0;
	}
}
