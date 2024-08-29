## 1) shascan 
Scans all the files inside the first argument (path) specified, and stores their hash value in string format inside the second 
argument (output file) specified. 

## 2) shainspect
Scans all the files inside the first argument path and checks if the hashes are coincident with the ones written inside the output file
after executing a shascan in a specified path.

# TODO
I am looking forward to use xxd to do a hexadecimal volc to binary using the "xxd" linux tool with the options -r and -p, so this would reduce the 40 bytes of SHA1
sum outputed in string format to 20 bytes which is the actual size of a SHA1 hash in byte format.
