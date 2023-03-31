#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#define DEFAULT_BLOCK_SIZE 4096
#define RESULT_OK 0

typedef enum {
        INPUT_FILE_OPEN_ERR = -1000,
        OUTPUT_FILE_OPEN_ERR,
        WRITE_ERR,
        READ_ERR,
        LSEEK_ERR,
        MEMORY_ALLOCATION_ERR,
        BAD_ARGUMENT_ERR,
        SAME_FILE_ERR,
        END_ERR,
} sparse_error_code_t;


int generate_sparse_file(const char *input_filename,
                         const char *output_filename,
                         size_t block_size)
{
        int read_fd        = -1;
        int write_fd       = -1;
        /* rw-rw-r-- */
        int write_fd_mode  = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
        char *file_buffer  = NULL;
        bool is_zero_block = false;
        ssize_t readn       = 0;
        int ret = RESULT_OK;

        read_fd = input_filename ? 
                open(input_filename, O_RDONLY) : STDIN_FILENO;
        if (read_fd == -1) {
                ret = INPUT_FILE_OPEN_ERR;
                return ret;
        }

        write_fd = open(output_filename, 
                        O_WRONLY | O_CREAT | O_TRUNC, write_fd_mode);
        if (write_fd == -1) {
                ret = OUTPUT_FILE_OPEN_ERR;
                goto out_close_read;
        }

        file_buffer = malloc(sizeof(char) * block_size);
        if (file_buffer == NULL) {
                ret = MEMORY_ALLOCATION_ERR;
                goto out_close_write;
        }

        while ((readn = read(read_fd, file_buffer, block_size))) {
                if (readn == -1) {
                        ret = READ_ERR;
                        goto out_free_buff;
                }
                is_zero_block = true;
                for (ssize_t i = 0; i < readn; ++i) {
                        if (file_buffer[i] != '\0') {
                                is_zero_block = false;
                                break;
                        }
                }

                if (is_zero_block) {
                        if (lseek(write_fd, readn, SEEK_CUR) == -1) {
                                ret = LSEEK_ERR;
                                goto out_free_buff;
                        }
                } else {
                        if (write(write_fd, file_buffer, readn) != readn) {
                                ret = WRITE_ERR;
                                goto out_free_buff;
                        }
                }
        }

out_free_buff:
        free(file_buffer);
out_close_write:
        close(write_fd);
out_close_read:
        if (read_fd != STDIN_FILENO)
                close(read_fd);
        return ret;
}

int cli_ret(sparse_error_code_t err)
{
        char *err_msg = "";

        if (err == RESULT_OK)
                return EXIT_SUCCESS;

        switch (err) {
        case INPUT_FILE_OPEN_ERR:
                err_msg = "Error: fail to open input file";
                break;
        case OUTPUT_FILE_OPEN_ERR:
                err_msg = "Error: fail to create and open output file";
                break;
        case READ_ERR:
                err_msg = "Error: fail to read input file";
                break;
        case WRITE_ERR:
                err_msg = "Error: fail to write to output file";
                break;
        case LSEEK_ERR:
                err_msg =
                 "Error: fail to perfom lseek operation on output file";
                break;
        case MEMORY_ALLOCATION_ERR:
                err_msg = "Error: fail to allocate memory";
                break;
        case BAD_ARGUMENT_ERR:
                err_msg = "Error: wrong argument(s). See --help for details";
                break;
        case SAME_FILE_ERR:
                err_msg = "Error: input and output files must be different";
                break;
        default:
                err_msg = "Error: unknown error";
                break;
        }
        printf("%s\n", err_msg);
        return EXIT_FAILURE;
}

void print_usage(const char *progname)
{
        printf("Usage: %s [OPTIONS...] [input_filename] output_filename\n\n"\
               " Options:\n"\
               "  -h, --help: Print this message\n"\
               "  -b, --block-size: Read/Write block size in bytes\n\n"\
               " Arguments\n"\
               "  input_filename: Name of file you want to sparse. If ommitted set to standard input\n"\
               "  output_filename: Name of new sparse file\n", progname
        );
}

int main(int argc, char **argv)
{
        /* preserve progname for usage */
        char *progname = argv[0];
        int opt = 0;
        int option_idx = 0;
        char *input_filename = NULL;
        char *output_filename = NULL;
        char *end_ptr = NULL;
        size_t block_size = DEFAULT_BLOCK_SIZE;
        static struct option long_options[] = {
          {"help",       no_argument,       NULL, 'h'},
          {"block-size", required_argument, NULL, 'b'},
          {0, 0, 0, 0},
        };
        int ret = 0;
        bool block_size_changed = false;

        if (argc < 2 || argc > 5) {
                ret = BAD_ARGUMENT_ERR;
                goto out;
        }

        /* parse opts */
        while ((opt = getopt_long(argc, argv, "hb:", long_options, &option_idx)) != -1) {
                switch (opt) {
                case 'h':
                        print_usage(progname);
                        goto out;
                case 'b':
                        if (optarg == NULL || optarg[0] == '-') {
                               ret = BAD_ARGUMENT_ERR;
                               goto out;
                        }

                        block_size = strtoul(optarg, &end_ptr, 10);

                        if (errno != 0 || end_ptr == optarg) {
                               ret = BAD_ARGUMENT_ERR;
                               goto out;
                        }

                        block_size_changed = true;
                        break;
                default:
                        ret = BAD_ARGUMENT_ERR;
                        goto out;
                }
        }

        if (optind >= argc) {
                ret = BAD_ARGUMENT_ERR;
                goto out;
        }

        if (block_size_changed)
                argc -= 2; /* remove block size opt from argc */

        if (argc == 2)
                output_filename = argv[optind];
        else {
                input_filename = argv[optind];
                output_filename = argv[optind + 1];
                if (!strcmp(input_filename, output_filename)) {
                        ret = SAME_FILE_ERR;
                        goto out;
                }
        }

        ret = generate_sparse_file(input_filename, output_filename, block_size);
out:
        return cli_ret(ret);
}