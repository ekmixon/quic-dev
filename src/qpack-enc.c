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

int my_pow(int base, int exp)
{
	int total = 0;

	if (!exp)
		return 1;

	total = base;
	--exp;

	while (exp--)
		total *= base;
	return total;
}

static int qpack_encode_prefix_integer(struct buffer *out, int i, int prefix_size)
{
	char first_byte = *b_tail(out);

	if (i < (my_pow(2, prefix_size) - 1)) {
		b_putchr(out, first_byte | i);
		return 1;
	}
	else {
		b_putchr(out, first_byte | (int)(my_pow(2, prefix_size) - 1));
		b_putchr(out, i - (my_pow(2, prefix_size) - 1));

		return 2;
		//ABORT_NOW();
	}

	return 0;
}

int qpack_encode_header(struct buffer *out, const struct ist n, const struct ist v)
{
	int total = 0, i;

	*b_tail(out) = '\x20';
	total += qpack_encode_prefix_integer(out, n.len, 3);
	for (i = 0; i < n.len; ++i)
		b_putchr(out, n.ptr[i]);
	total += n.len;

	*b_tail(out) = '\x00';
	total += qpack_encode_prefix_integer(out, v.len, 7);
	for (i = 0; i < v.len; ++i)
		b_putchr(out, v.ptr[i]);
	total += v.len;

	return total;
}
