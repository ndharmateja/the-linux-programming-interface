#include "tlpi_hdr.h"
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STR_SIZE 100

int main(int argc, char *argv[])
{
    // Variables
    // struct iovec {
    //      void *iov_base;
    //      size_t iov_len;
    // }
    int fd, flags;
    struct iovec iov[3];
    struct stat my_struct; /* First buffer */
    int x;                 /* Second buffer */
    char str[STR_SIZE];    /* Third buffer */
    ssize_t num_read, total_required;

    // Validate args
    // If the #args is not 2 or if --help is provided
    // we display the usage
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s <file>\n", argv[0]);

    // Open the file for reading
    flags = O_RDONLY;
    if ((fd = open(argv[1], flags)) == -1)
        errExit("open");

    // Set the buffers to the iov array
    iov[0].iov_base = &my_struct;
    iov[0].iov_len = sizeof(my_struct);
    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;

    // Compute the total bytes required
    total_required = 0;
    for (size_t i = 0; i < 3; i++)
        total_required += iov[i].iov_len;

    // Perform the read
    num_read = readv(fd, iov, 3);
    if (num_read == -1)
        errExit("readv");
    if (num_read < total_required)
        printf("Read fewer bytes thatn requested\n");

    // Print stats and exit
    printf("Total bytes requested: %ld; bytes read: %ld\n",
           (long)total_required,
           (long)num_read);
    exit(EXIT_SUCCESS);
}