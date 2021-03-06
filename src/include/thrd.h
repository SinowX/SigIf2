// Threads
// 线程主函数声明
#ifndef THRD_H
#define THRD_H

/* #include "ev_loop.h" */
#include "ev_demul.h"
#include "conf_mgr.h"
#include "conn_map.h"
#include <thread>
#include <list>

void Leader();

void Acceptor();

void IOReceiver(int iorcvr_id);

void Worker(int worker_id);

void HBKeeper(int machine_type);

void Logger();

#endif
