#include "tlpi_hdr.h"
#include <fcntl.h>

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
    while ((num_read = read(input_fd, buf, BUF_SIZE)) > 0)
        if (write(output_fd, buf, num_read) != num_read)
            fatal("couldn't write whole buffer");
    if (num_read == -1)
        errExit("read");

    // Close the file descriptors
    if (close(input_fd) == -1)
        errExit("close input");
    if (close(output_fd) == -1)
        errExit("close output");

    // Exit the program
    exit(EXIT_SUCCESS);
}
