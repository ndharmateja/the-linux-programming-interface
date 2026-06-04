#include "tlpi_hdr.h"
#include <fcntl.h>

/**
 * Usage:
 * ./copy <oldfile> <newfile>
 *
 * Eg:
 * ./copy test test.old         // Copy a regular file
 * ./copy a.txt /dev/tty        // Copy a regular file to *this* terminal
 * ./copy /dev/tty b.txt        // Copy input from *this* terminal to a regular file
 * ./copy /dev/pts/16 /dev/tty  // Copy input from another terminal
 */

/**
 * We are using ifndef to allow override BUF_SIZE definition
 * using the cc -D option while compiling
 */
#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, const char *argv[])
{
    // Declare vars
    int input_fd, output_fd, open_flags;
    mode_t file_permissions;
    ssize_t num_read;

    // Buffer
    char buf[BUF_SIZE];

    // Validate args
    // Print the usage message if incorrect number of args
    // or if --help option is given
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s <old-file> <new-file>\n", argv[0]);

    // Open input file
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1)
        errExit("opening file %s", argv[1]);

    // Open output file with rw-rw-rw- permissions
    // If we don't use O_TRUNC and the file already exists, then anything
    // we write will be overwrite the existing bytes from the start of the file
    // If we use O_TRUNC, then file will be truncated to 0 bytes after
    // it gets opened (if it already exists)
    open_flags = O_CREAT | O_WRONLY | O_TRUNC;
    file_permissions = S_IRUSR | S_IWUSR |
                       S_IRGRP | S_IWGRP |
                       S_IROTH | S_IWOTH;
    output_fd = open(argv[2], open_flags, file_permissions);
    if (output_fd == -1)
        errExit("opening file %s", argv[2]);

    // Transfer the data from the old file to the new file
    // until we reach the end or encounter an error
    // ! Do not write() byte by byte because of the system call overhead
    off_t total_bytes_read = 0;
    while ((num_read = read(input_fd, buf, BUF_SIZE)) > 0)
    {
        // Count the total bytes read
        total_bytes_read += num_read;

        // Instead of writing all bytes, we only need to write the non-null bytes
        // and skip the null bytes by using lseek()

        // Loop invariant
        // Write the continuous stretch of non-null bytes block starting at pos to the file
        // and skip the continuous stretch of null bytes block by using lseek()
        // in one iteration
        int pos = 0, end, count;
        while (pos < num_read)
        {
            // Write all the non-null bytes starting at pos
            // Move end until we encounter the first null byte
            end = pos;
            while (end < num_read && buf[end] != '\0')
                end++;

            // At this point [pos, end) is a stretch of non-null bytes block
            // of length 'end - start'
            // We can write buf[pos:end] (pythong style slice) this to the file
            // end could be equal to pos in which case nothing to write
            // To write the buf starting at index pos, we can use "buf + pos" or "$buf[pos]"
            count = end - pos;
            if (count)
                if (write(output_fd, buf + pos, count) != count)
                    fatal("write() returned error or a partial write occurred");

            // If we reached the end of the read bytes we can exit
            if (end == num_read)
                break;

            // At this point 'end' is pointing to the first null byte of the stretch
            // We move end until we find the first non-null byte
            pos = end;
            while (end < num_read && buf[end] == '\0')
                end++;

            // At this point [pos, end) is a stretch of null bytes of length end-pos
            // so we don't need to actually write these bytes, we just need to move the
            // offset of the file by the number of null bytes
            count = end - pos;
            if (count)
                if (lseek(output_fd, (off_t)count, SEEK_CUR) == -1)
                    errExit("lseek");

            // Update pos for the next iteration
            pos = end;
        }
    }

    // Handle read error
    if (num_read == -1)
        errExit("read");

    // At this point if the last stretch of bytes were null bytes
    // only the offset in the kernel's open file descriptors table
    // and not in the actual inode
    // So we can use ftruncate() with the total number of bytes read
    // that forces a write
    if (ftruncate(output_fd, total_bytes_read) == -1)
        errExit("ftruncate");

    // Close the file descriptors
    if (close(input_fd) == -1)
        errExit("close input");
    if (close(output_fd) == -1)
        errExit("close output");

    // Exit the program
    exit(EXIT_SUCCESS);
}
