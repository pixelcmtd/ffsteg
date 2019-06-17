all:
	@gcc ffsteg.c -O3 -s -o ffsteg
clean:
	@rm -f ffsteg secret Mario.ff Mariosteg.ff
test:
	@2ff < Mario.gif > Mario.ff
	@gcc ffsteg.c -Og -fsanitize=address -o ffsteg
	@echo "SECRET MESSAGE" > secret
	@./ffsteg e secret Mario.ff Mariosteg.ff
	@./ffsteg d Mariosteg.ff
