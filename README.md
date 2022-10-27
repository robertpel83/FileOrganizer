
It's (maybe eventually) the best file organizer program.

Cross platform C++ with Qt UI.
Every function optimized for maximum speed and compatibility.
Unicode support.

Scans your files, gets all the metadata and info it can from the filesystem, does a "fast hash" (hashes five 16kb chunks, beginning, middle, end) on them with all the hash algorithms.
If the hashes match, the size matches, and the modified date matches, it considers it a match.
If the modified date differs, it does a byte-by-byte comparison.

Uses regex to get any date info from the filename.
Uses exif to get any embedded dates in images.

TODO:

Compares duplicate images with every "fuzzy compare" type and makes sure they are actually dupes.
Combines all the dates collected from metadata, timestamps, and filenames and decides which one is likely the most correct one.
Keeps all the info from all files just in case.

Blazing fast regex file renamer with scriptable renaming GUI, can handle maximum number of files at maximum speed your system can handle.

OCR support with multiple engines and AI engine to scan your images and PDFs for text.

AI image classification to search your images for objects.

---

Do you have a giant messy folder with years of crap thrown in it?

Do you have duplicate images in different resolutions with different names and datestamps?
Can you never figure out the real date an image was taken on and wish all the resized dupes all had the correct date?
Do you wish you could just keep the best quality version but also keep all the dates and filenames and info from all the dupes just in case?

Do you have duplicate files all over the place but all the dedupe programs crash or are too slow or too sketchy?
Do you want to know for totally sure the duplicates are 100% for sure exact duplicates and possibly leave symlinks in place?
Do you want to save all the filenames and dates and metadata for all the dupes and store the info just in case?
Do you want to configure the deduplication method with exactly what you want and know exactly what it's doing?

Do you want to rename ridiculous amounts of files with scriptable renaming but all the existing renamer programs are too slow or crash?

Do you want to OCR your images and PDFs and search them for text like Google/Apple Photos without uploading everything to the cloud?
Want to run AI image classification on your pictures and search them by object like Google/Apple Photos without uploading everything to the cloud?








