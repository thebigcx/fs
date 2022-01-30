
# fs
Tools and specification of my filesystem.

## Layout
| Length | Desc |
|--|--|
| 1 | Superblock|
| ? (Disk size) | Block usage bitmap |
| Rest of disk | Data & nodes |

## Superblock
Found at LBA 0
| Field | Desc |
|--|--|
| char[506] | Boot code |
| uint32_t | Root node |
|uint16_t|Boot signature (0xaa55)|

## Node
Occupies first 32 bytes of LBA, rest is for the data
| Field | Desc |
|--|--|
| uint64_t | Size |
| uint8_t | Flags |
|uint32_t[8]|Direct sectors|
|uint32_t |Singly indirect sectors|
|uint32_t |Doubly indirect sectors|
|uint32_t |Trebly indirect sectors|
|uint8_t[7] |Padding|
|uint8_t[480] | Data|
Data can be a list of directory entries, or file data.

## Directory Entry
Variable length, contiguous in memory.
| Field | Desc |
|--|--|
| uint32_t | Node |
| char[] | Name (null-terminated) |
