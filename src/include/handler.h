// 处理函数声明头
#ifndef HANDLER_H
#define HANDLER_H

#include "conn_info.h"
#include <nlohmann/json.hpp>
using json=nlohmann::json;
// accept handlers
int accept_fd_hdlr(const int fd);

// read as much as possible to buffer
int read_all_hdlr(const int fd);

// query handlers
int client_query_hdlr(ConnInfoPtr client_conn, BufferPtr json_str);

int machine_msg_hdlr(ConnInfoPtr machine_conn);


// write handlers

// error handlers
int error_fd_hdlr(const int fd);


// machine heart beat

int SendHB_XingZhi(const ConnInfoPtr& conn);


#endif
