sdc_parse: sdc.l sdc.y
	bison -d -v sdc.y
	flex sdc.l
	cc -o $@ sdc.tab.c lex.yy.c -lfl
