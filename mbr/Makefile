target: mbr_parser

mbr_parser: mbr_parser.o
	$(CC) $(LDFLAGS) -o $@ $^

mbr_parser.o: mbr_parser.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f mbr_parser mbr_parser.o