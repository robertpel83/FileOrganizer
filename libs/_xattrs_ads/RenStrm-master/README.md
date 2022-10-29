# RenStrm
NTFS Alternate Data Stream  (ADS)  Rename utility

This utility allows to rename Alternate Data Streams (ADS) in NTFS file systems. 


## Syntax

```
RENSTRM oldName newName

oldName         The old stream name, including parent file name.
newName         The new stream name, in :name:[$DATA] format.

Examples:

RENSTRM test.txt:A :U
RENSTRM test.txt:A :X:$DATA
RENSTRM e:\temp\test.txt:ABC :stream2
```

NOTE: It's possible to rename & copy the default(main) data stream; the default stream data will be moved to the new stream and the latter reset to zero length. This can be done using the source filename without specifying any stream name:

```
RENSTRM test.txt :NEWSTREAM
```

Doing a `DIR /R` will yield now:

```
11/06/2018  21:06                 0 TEST.TXT
                                 15 TEST.TXT:NEWSTREAM:$DATA
```


## How to build

Use any C++11 conformant compiler with the Windows SDK distribution, or use the binary releases for x86/x64 systems provided in this repository.

## License

```
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non - commercial, and by any
```
