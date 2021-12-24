// Connection Queue
// 线程间通信、同步的通道
// 当前还包含了HupTaskMap的内容，见下方
#ifndef CONN_Q_H
#define CONN_Q_H

#include "conn_info.h"
#include <mutex>
#include <queue>
#include <condition_variable>


class Conn_Q
{
	public:
		Conn_Q()=default;

		Conn_Q(Conn_Q&)=delete;
		void operator=(Conn_Q&)=delete;
		~Conn_Q()=default;	

		// return 0 for success, -1 for error	
		int Push(const ConnInfoPtr conn);
		/* { */
		/* 	{ */
		/* 		std::lock_guard<std::mutex> lock(mtx_); */
		/* 		conn_q_.push(conn); */
		/* 	} */
		/* 	condi_.notify_one(); */
		/* 	return 0; */
		/* } */

		ConnInfoPtr Pop();
		/* { */
		/* 	std::unique_lock<std::mutex> lk(mtx_); */
		/* 	if(conn_q_.empty()) */
		/* 		condi_.wait(lk); */
		/* 	ConnInfoPtr ptr = conn_q_.front(); */
		/* 	conn_q_.pop(); */
		/* 	lk.unlock(); */
		/* 	return ptr; */
		/* } */
		
		ConnInfoPtr TryPop()
		{
			if(Size()==0) return nullptr;
			else return Pop();
		}

		int Size() const
		{
			return conn_q_.size();
		}

	private:

		std::queue<ConnInfoPtr, std::deque<ConnInfoPtr>> conn_q_;
		std::mutex mtx_;
		std::condition_variable condi_;
};

/* using NewConnQ = Conn_Q; */
static Conn_Q& NewConnQ()
{
	static Conn_Q conn_q_;
	return conn_q_;
}

/* using TaskQ = Conn_Q; */
static Conn_Q& TaskQ()
{
	static Conn_Q conn_q_;
	return conn_q_;
}






#endif
