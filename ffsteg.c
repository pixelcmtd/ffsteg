#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;
typedef unsigned short ushort;

//╔══════╤══════════════════════════════════════════════════════════╗
//║Bytes │Description                                               ║
//╠══════╪══════════════════════════════════════════════════════════╣
//║8     │"farbfeld" magic value                                    ║
//╟──────┼──────────────────────────────────────────────────────────╢
//║4     │32-Bit ̶B̶E LE unsigned integer (width)                     ║
//╟──────┼──────────────────────────────────────────────────────────╢
//║4     │32-Bit ̶B̶E LE unsigned integer (height)                    ║
//╟──────┼──────────────────────────────────────────────────────────╢
//║[2222]│4⋅16-Bit ̶B̶E LE unsigned integers [RGBA] / pixel, row-major║
//╚══════╧══════════════════════════════════════════════════════════╝
//about that BE/LE: https://tinyurl.com/xxscklss

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
		uint sz = a2i32(rwid) * a2i32(rhei);
		ushort r, g, b, a;
		for(uint i = 0; i < sz; i++)
		{
			fread(rr, 1, 2, in);
			fread(rg, 1, 2, in);
			fread(rb, 1, 2, in);
			fread(ra, 1, 2, in);
			char m = fgetc(msg);
			r = (a2i16(rr) & 0xfffc) | (m        & 3);
			g = (a2i16(rg) & 0xfffc) | ((m >> 2) & 3);
			b = (a2i16(rb) & 0xfffc) | ((m >> 4) & 3);
			a = (a2i16(ra) & 0xfffc) | ((m >> 6)    );
			i2a16(r, rr);
			i2a16(g, rg);
			i2a16(b, rb);
			i2a16(a, ra);
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
		uint sz = a2i32(rwid) * a2i32(rhei);
		ushort r, g, b, a;
		for(uint j = 0; j < sz; j++)
		{
			fread(rr, 1, 2, i);
			fread(rg, 1, 2, i);
			fread(rb, 1, 2, i);
			fread(ra, 1, 2, i);
			r = a2i16(rr) & 3;
			g = a2i16(rg) & 3;
			b = a2i16(rb) & 3;
			a = a2i16(ra) & 3;
			fputc(r | g << 2 | b << 4 | a << 6, o);
		}
		fclose(o);
		fclose(i);
		return 0;
	}
	else help(argv);
}
