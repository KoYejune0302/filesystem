target: gpt_parser

gpt_parser: gpt_parser.o
	$(CC) $(LDFLAGS) -o $@ $^

gpt_parser.o: gpt_parser.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f gpt_parser gpt_parser.o