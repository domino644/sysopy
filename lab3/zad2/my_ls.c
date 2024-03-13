#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    char *dirname;
    struct dirent *dirinfo;
    DIR *dir;
    if (argc > 1)
    {
        dirname = argv[1];
    }
    else
    {
        dirname = "./";
    }

    dir = opendir(dirname);
    if (dir == NULL)
    {
        printf("Error when opening %s", dirname);
        return 1;
    }
    long long all_bytes = 0;
    int status;
    struct stat *stat_buffer;
    while ((dirinfo = readdir(dir)) != NULL)
    {
        status = stat(dirinfo->d_name, stat_buffer);
        if (status == 0)
        {
            if (!S_ISDIR(stat_buffer->st_mode))
            {
                printf("%s %lld\n", dirinfo->d_name, stat_buffer->st_size);
                all_bytes += stat_buffer->st_size;
            }
        }
    }
    printf("All memory used: %lld\n", all_bytes);
    return 0;
}