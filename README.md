## 1) shascan 
Scans all the files inside the first argument (path) specified, and stores their hash value in string format inside the second 
argument (output file) specified. 
There are some files that can't be accessed at or that takes too long to scan because they might be locked so the proccess will sleep until 
it can scan that file. To avoid this, you can send a SIGQUIT signal (Ctrl + \) to continue the proccess without scanning that specific file.

## 2) shainspect
Scans all the files inside the first argument path and checks if the hashes are coincident with the ones written inside the output file
after executing a shascan in a specified path.
I recommend you to redirect the error output to /dev/null so you don't get innecessary information about where the scan is being done and only shows 
the modified files.

**Warning**
It is recommended to encrypt the hashes' file or atleast restrict the access to it.

# TODO
I am looking forward to use xxd to do a hexadecimal volc to binary using the "xxd" linux tool with the options -r and -p, so this would reduce the 40 bytes of SHA1
sum outputed in string format to 20 bytes which is the actual size of a SHA1 hash in byte format. The final idea of the project is to be able to use the file system
optimization that uses Block-Level Compression, so the final file would not be that heavy.


