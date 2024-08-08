target: fat32_parser

gpt_parser: fat32_parser.o
	$(CC) $(LDFLAGS) -o $@ $^

gpt_parser.o: fat32_parser.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f fat32_parser fat32_parser.o