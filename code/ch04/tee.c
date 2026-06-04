#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * Exercise 1
 */

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[])
{
    // Variables
    int output_fd, flags;
    mode_t file_permissions;
    char buf[BUF_SIZE];
    ssize_t num_read;
    bool to_append = false;

    // Validate args
    if ((argc != 2 && argc != 3) || strcmp(argv[1], "--help") == 0)
        usageErr("%s [-a] <file>\n", argv[0]);

    // Check if the -a option is given
    if (getopt(argc, argv, ":a") == 'a')
        to_append = true;

    // Open the file to write to (create with rw-rw-rw- permissions)
    // The last arg is the filename
    // We use the flags O_WRONLY and O_CREAT
    // If the -a flag is given to our program, we need to append
    // to the file instead, so we have to use the O_APPEND flag instead
    // of the O_TRUNC as the third flag n
    flags = O_WRONLY | O_CREAT;
    flags |= to_append ? O_APPEND : O_TRUNC;
    file_permissions = S_IRUSR | S_IWUSR |
                       S_IRGRP | S_IWGRP |
                       S_IROTH | S_IWOTH;
    output_fd = open(argv[argc - 1], flags, file_permissions);
    if (output_fd == -1)
        errExit("open");

    // Keep reading from the terminal until we reach EOF
    while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
    {
        // Write the read bytes to the terminal (standard output)
        // and to the given file
        if (write(STDOUT_FILENO, buf, num_read) != num_read)
            fatal("write returned error or a partial write has occurred");
        if (write(output_fd, buf, num_read) != num_read)
            fatal("write returned error or a partial write has occurred");
    }

    // If we reach here if num_read was 0 or -1
    // 0 => EOF so nothing to do
    // -1 => read error
    if (num_read == -1)
        errExit("read");

    // Close the output file descriptor and exit
    if (close(output_fd) == -1)
        errExit("close");
    exit(EXIT_SUCCESS);
}