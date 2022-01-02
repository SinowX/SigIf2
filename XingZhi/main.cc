#include "server.h"
#include <thread>
#include "receiver.h"


int main()
{
	std::thread svr_thrd(server);	
	std::thread rcvr_thrd(receiver);
	svr_thrd.join();
	rcvr_thrd.join();
}
