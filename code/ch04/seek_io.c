#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"

int main(int argc, const char *argv[])
{
    // Variables
    size_t len;
    off_t offset;
    int fd, ap, j, flags;
    mode_t file_permissions;
    char *buf;
    ssize_t num_read, num_written;

    // Validate args
    // Print the usage message if incorrect number of args
    // or if --help option is given
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s <file> {r<length>|R<length>|w<string>|s<offset>}...\n", argv[0]);

    // Open the file with rw-rw-rw- permissions
    flags = O_RDWR | O_CREAT;
    file_permissions = S_IRUSR | S_IWUSR |
                       S_IRGRP | S_IWGRP |
                       S_IROTH | S_IWOTH;
    fd = open(argv[1], flags, file_permissions);
    if (fd == -1)
        errExit("open");

    // Parse each argument after the file
    for (ap = 2; ap < argc; ap++)
    {
        switch (argv[ap][0])
        {
        case 'r':
        case 'R':
            // Parse r124 or R1234 etc
            len = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);

            // Allocate space for len bytes to read them
            buf = malloc(len);
            if (buf == NULL)
                errExit("malloc");

            // Read the bytes
            num_read = read(fd, buf, len);
            if (num_read == -1)
                errExit("read");

            // If the number of bytes read is 0, then it is EOF
            if (num_read == 0)
                printf("%s: end-of-file\n", argv[ap]);

            // Otherwise print the bytes from the buffer
            else
            {
                // Print "R1234: " etc
                printf("%s: ", argv[ap]);

                // Print each of the bytes
                for (j = 0; j < num_read; j++)
                {
                    // r => text format
                    // Print the character if printable otherwise print a '?'
                    if (argv[ap][0] == 'r')
                        printf("%c", isprint((unsigned char)buf[j]) ? buf[j] : '?');

                    // R => hexadecimal
                    else
                        printf("x%02x ", (unsigned int)buf[j]);
                }
                printf("\n");
            }

            // Free the buffer and exit
            free(buf);
            break;

        // Write string at the current offset
        case 'w':
            num_written = write(fd, &argv[ap][1], strlen(&argv[ap][1]));
            if (num_written == -1)
                errExit("write");
            printf("%s: wrote %ld bytes\n", argv[ap], (long)num_written);
            break;

        // Seek to the given offset from the beginning of the file (SEEK_SET)
        case 's':
            offset = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
            if (lseek(fd, offset, SEEK_SET) == -1)
                errExit("lseek");
            printf("%s: seek succeeded\n", argv[ap]);
            break;

        default:
            cmdLineErr("Argument must start with [rRws]: %s\n", argv[ap]);
        }
    }

    // Exit the program
    exit(EXIT_SUCCESS);
}