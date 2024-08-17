# File system

## MBR

Master Boot Record<br />
![structure-of-mbr](./mbr/mbr-disk-structure.webp)

```
$ cd ./mbr
$ make
$ mbr_parser {mbr_image_file}
```

## GPT

GUID Partition Table<br />
![structure-of-gpt](./gpt/gpt-disk-layout.webp)

```
$ cd ./gpt
$ make
$ gpt_parser {gpt_image_file}
```

## FAT32

How FAT32 works<br />
![structure-of-fat32](./fat32/how-fat32-works.avif)

```
$ cd ./fat32
$ make
$ fat32_parser {fat32_image_file} {starting_cluster_number}
```

## NTFS

NTFS runlist<br />
![structure-of-runlist](./ntfs/ntfs.png)

```
$ cd ./ntfs
$ make
$ ntfs_parser {ntfs_image_file}
```