CC ?= cc
CFLAGS ?= -O3

all: ffsteg

ffsteg: ffsteg.c
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -f ffsteg secret Mario.ff Mariosteg.ff

test:
	2ff < Mario.gif > Mario.ff
	$(CC) ffsteg.c -Og -fsanitize=address -o ffsteg
	echo "SECRET MESSAGE" > secret
	./ffsteg e secret Mario.ff Mariosteg.ff
	./ffsteg d Mariosteg.ff

.PHONY: clean test
