#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <time.h>
#include <sys/file.h>

#define MAXLINE 4096

struct Time
{
    char* month;
    char* day;
    char* year;
}; 

int create_directories(struct Time*);
int copy_file(struct Time*);

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    time_t now = time(NULL);

    struct tm tm = *gmtime(&now);

    char* str = malloc(MAXLINE);
    strftime(str, MAXLINE, "%d %b %Y", &tm);

    //printf("%s\n", str);

    char* temp = str;

    struct Time* time = malloc(sizeof(struct Time));
    
    //printf("Temp: %s\n", temp);

    char* tok_r;
    char tok = ' ';

    char* str_day = strtok_r(temp, &tok, &tok_r);
    char* str_mon = strtok_r(NULL, &tok, &tok_r);
    char* str_year = strtok_r(NULL, &tok, &tok_r);

    time->day = strdup(str_day);
    time->month = strdup(str_mon);
    time->year = strdup(str_year);

    if(create_directories(time) < 0)
    {
        printf("Issue with creating directories\n");
        exit(1);
    }

    if(copy_file(time) < 0)
    {
        printf("Error in copying contents\n");\
        exit(2);
    }

    free(time->month);
    free(time->year);
    free(time->day);
    free(time);
    free(str);

    printf("Success!\n");

    return 0;
}

int create_directories(struct Time* time)
{
    system("find /home/remote/server/logs > results.txt");
    int fd = open("results.txt", O_RDONLY);
    if(fd < 0)
    {
        printf("Unable to open results.txt\n");
        return -1;
    }

    char buffer[MAXLINE];
    int n = read(fd, buffer, sizeof(buffer) - 1);
    if(n < 0)
    {
        printf("Read failed in results.txt\n");
        return -1;
    }
    buffer[n] = '\0';

    
    char save_buf[MAXLINE];
    strcpy(save_buf, buffer);

    for(int i = 0; i < 2; i++) 
    {
        char work_buf[MAXLINE];
        strcpy(work_buf, save_buf);
        char* tokptr;
        char *line = NULL;
        line = strtok_r(work_buf, "\n", &tokptr);  //doesnt matter

        bool found = false;
        while(1)
        {
            if(found)
                break;

            line = strtok_r(NULL, "\n", &tokptr);
            if(!line)
                break;
        
            //printf("line: %s\n", line);

            //line += 25;
            //printf("working with: %s\n", line);
            
            switch(i)
            {
                case 0:
                    line += 25;
                    if(strcmp(line, time->year) == 0)
                    {
                        //printf("Found year\n");
                        found = true;
                    }
                    break;
                case 1:
                    line += 30;
                    if(strcmp(line, time->month) == 0)
                    {
                        //printf("Found month\n");
                        found = true;
                    }
                    break;
            }

        }

        //printf("Done with %d\n", i);
        tokptr = NULL;

        if(!found)
        {
            char* cmd = malloc(MAXLINE);
            switch(i)
            {
                case 0:
                    sprintf(cmd, "mkdir /home/remote/server/logs/%s", time->year);
                    system(cmd);
                    sprintf(cmd, "sudo chmod 777 /home/remote/server/logs/%s", time->year);
                    system(cmd);
                    //printf("Made: %s\n", cmd);
                    break;
                case 1:
                    sprintf(cmd, "mkdir /home/remote/server/logs/%s/%s", time->year, time->month);
                    system(cmd);
                    sprintf(cmd, "sudo chmod 777 /home/remote/server/logs/%s/%s", time->year, time->month);
                    system(cmd);
                    break;
            }
            free(cmd);
        }
    }

    if(unlink("results.txt") < 0)
    {
        printf("Failed to remove results.txt");
        return -1;
    }
    return 1;
}

int copy_file(struct Time* time)
{
    char* path = malloc(MAXLINE);
    sprintf(path, "/home/remote/server/logs/%s/%s/%s.txt", time->year, time->month, time->day);
    //printf("%s\n", path);

    int copylog_fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 777);
    if(copylog_fd < 0)
    {
        printf("Failed to open path\n");
        return -1;
    }

    int log_fd = open("/home/remote/server/debug.txt", O_RDWR);
    if(log_fd < 0)
    {
        printf("Failed to open log file\n");
        return -1;
    }

    off_t size = lseek(log_fd, 0, SEEK_END);
    if(size == 0)
    {
        if(unlink(path) < 0)
        {
            printf("Issue removing empty file\n");
            free(path);
            return -1;
        }
        return 1;
    }
    free(path);

    lseek(log_fd, 0, SEEK_SET);

    flock(log_fd, LOCK_SH);

    char buffer[MAXLINE];
    memset(buffer, 0, sizeof(buffer));

    while(1)
    {
        ssize_t read_n = read(log_fd, buffer, sizeof(buffer));
        if(read_n < 0)
        {
            printf("Issue with reading\n");
            return -1;
        }
        if(read_n == 0)
            break;

        int write_n = write(copylog_fd, buffer, read_n);
        if(write_n < 0)
        {
            printf("issue with writing\n");
            return -1;
        }
    }

    lseek(log_fd, 0, SEEK_SET);
    ftruncate(log_fd, 0);

    flock(log_fd, LOCK_UN);

    close(log_fd);
    close(copylog_fd);

    return 1;
}