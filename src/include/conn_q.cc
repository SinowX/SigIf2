#include "conn_q.h"


int Conn_Q::Push(const ConnInfoPtr conn)
{
	{
		std::lock_guard<std::mutex> lock(mtx_);
		conn_q_.push(conn);
	}
	condi_.notify_one();
	return 0;
}


ConnInfoPtr Conn_Q::Pop()
{
	std::unique_lock<std::mutex> lk(mtx_);
	if(conn_q_.empty())
		condi_.wait(lk);
	ConnInfoPtr ptr = conn_q_.front();
	conn_q_.pop();
	lk.unlock();
	return ptr;
}
