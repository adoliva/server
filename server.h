#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#include "node.h"

#define PORT 80
#define MAXLINE 4096
#define MAX_RESPONSE 32300
#define MAX_CLIENTS 50

#define SERVER "Snap/0.1"

extern volatile sig_atomic_t SIGNAL_FLAG;
void signal_handler(int signum);

struct Client {
    char* client_ip;
    int client_port;
    
    int client_fd;
    int fd;
    char* full_path;
    
    char* method;
    char* path;
    char* version;
    
    char* host;
    int connection_status;
    
    int DNT;
    int GPC;
    
    char* user_agent;
    
    char* referer;
    int upgrade_tls;
    
    char* accept;
    char* encoding;
    char* language;
    
    char* priority;
    char* request;
};

struct Client* init_request(char* request, int client_fd, struct Node* tree_head);
int process_request(struct Client* client);
int send_response(struct Client* client);
char* get_time(int offset);
char* content_type(char* filepath);
int master_log(int code, struct Client* client);
int send_error(int code, int client_fd);

#endif 