#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;
typedef unsigned short ushort;

//╔════════╤═════════════════════════════════════════════════════════╗
//║ Bytes  │ Description                                             ║
//╠════════╪═════════════════════════════════════════════════════════╣
//║ 8      │ "farbfeld" magic value                                  ║
//╟────────┼─────────────────────────────────────────────────────────╢
//║ 4      │ 32-Bit BE unsigned integer (width)                      ║
//╟────────┼─────────────────────────────────────────────────────────╢
//║ 4      │ 32-Bit BE unsigned integer (height)                     ║
//╟────────┼─────────────────────────────────────────────────────────╢
//║ [2222] │ 4⋅16-Bit BE unsigned integers [RGBA] / pixel, row-major ║
//╚════════╧═════════════════════════════════════════════════════════╝

typedef struct
{
	ushort r;
	ushort g;
	ushort b;
	ushort a;
} pixel;

#define a2i32(x) ((x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3])
#define a2i16(x) ((x[0] << 8) | x[1])
#define i2a16(x, o) o[1] = x; o[0] = x >> 8;

#define help(argv) \
{\
	printf("%s [e/d] {message (only for e)} {in} {out}", argv[0]);\
	exit(1);\
}

int main(int argc, char **argv)
{
	if(argc < 2) help(argv);
	if(*argv[1] == 'e')
	{
		if(argc < 3) help(argv);
		FILE *in  = stdin;
		FILE *out = stdout;
		FILE *msg = fopen(argv[2], "r");
		if(argc > 3) in  = fopen(argv[3], "r");
		if(argc > 4) out = fopen(argv[4], "w");
		char hdr[8], rwid[4], rhei[4];
		char rr[2], rg[2], rb[2], ra[2];
		fread(hdr, 1, 8, in);
		fwrite(hdr, 1, 8, out);
		fread(rwid, 1, 4, in);
		fwrite(rwid, 1, 4, out);
		fread(rhei, 1, 4, in);
		fwrite(rhei, 1, 4, out);
		uint wid = a2i32(rwid), hei = a2i32(rhei);
		for(uint i = 0; i < wid * hei; i++)
		{
			pixel p;
			fread(rr, 1, 2, in);
			fread(rg, 1, 2, in);
			fread(rb, 1, 2, in);
			fread(ra, 1, 2, in);
			p.r = a2i16(rr);
			p.g = a2i16(rg);
			p.b = a2i16(rb);
			p.a = a2i16(ra);
			char m = fgetc(msg);
			p.r &= 0xfffc;
			p.b &= 0xfffc;
			p.g &= 0xfffc;
			p.a &= 0xfffc;
			p.r |= (m        & 3);
			p.g |= ((m >> 2) & 3);
			p.b |= ((m >> 4) & 3);
			p.a |= ((m >> 6)    );
			i2a16(p.r, rr);
			i2a16(p.g, rg);
			i2a16(p.b, rb);
			i2a16(p.a, ra);
			fwrite(rr, 1, 2, out);
			fwrite(rg, 1, 2, out);
			fwrite(rb, 1, 2, out);
			fwrite(ra, 1, 2, out);
		}
		fclose(in);
		fclose(out);
		return 0;
	}
	else if(*argv[1] == 'd')
	{
		FILE *i = stdin;
		FILE *o = stdout;
		if(argc > 2) i = fopen(argv[2], "r");
		if(argc > 3) o = fopen(argv[3], "w");
		char hdr[8], rwid[4], rhei[4];
		char rr[2], rg[2], rb[2], ra[2];
		fread(hdr, 1, 8, i);
		fread(rwid, 1, 4, i);
		fread(rhei, 1, 4, i);
		uint wid = a2i32(rwid), hei = a2i32(rhei);
		for(uint j = 0; j < wid * hei; j++)
		{
			pixel p;
			fread(rr, 1, 2, i);
			fread(rg, 1, 2, i);
			fread(rb, 1, 2, i);
			fread(ra, 1, 2, i);
			p.r = a2i16(rr);
			p.g = a2i16(rg);
			p.b = a2i16(rb);
			p.a = a2i16(ra);
			char m = 0;
			p.r &= 3;
			p.b &= 3;
			p.g &= 3;
			p.a &= 3;
			m |= (p.r     );
			m |= (p.g << 2);
			m |= (p.b << 4);
			m |= (p.a << 6);
			fputc(m, o);
		}
		fclose(o);
		fclose(i);
		return 0;
	}
	else help(argv);
}
