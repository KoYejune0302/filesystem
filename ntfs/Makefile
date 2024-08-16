target: ntfs_parser

ntfs_parser: ntfs_parser.o
	$(CC) $(LDFLAGS) -o $@ $^

ntfs_parser.o: ntfs_parser.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f ntfs_parser ntfs_parser.o