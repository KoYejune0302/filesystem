# File system

## MBR

Master Boot Record
![structure-of-mbr](./mbr/mbr-disk-structure.webp)

```
$ cd ./mbr
$ make
$ mbr_parser {mbr_image_file}
```

## GPT

GUID Partition Table
![structure-of-gpt](./gpt/gpt-disk-layout.webp)

```
$ cd ./gpt
$ make
$ gpt_parser {gpt_image_file}
```
