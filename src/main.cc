#include "thrd.h"

int main()
{
	ConfMgr::getInstance().ReadConfig("config.ini");
	std::thread thrd_leader(Leader);

	thrd_leader.join();
	exit(0);
}
