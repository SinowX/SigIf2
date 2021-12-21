#ifndef EV_LOOP_H
#define EV_LOOP_H

#include "ev_demul.h"
#include <memory>




class EvLoop
{
	public:
		EvLoop(std::shared_ptr<EvDemul> ev_demul)
			:ev_demul_(ev_demul)
		{};
		EvLoop(EvLoop&)=delete;
		void operator=(EvLoop&)=delete;
		~EvLoop(){};

		int RunOnce()
		{
			return 0;
		}

		std::shared_ptr<EvDemul>& getEvDemul()
		{
			return ev_demul_;
		}
		
		

	private:
		std::shared_ptr<EvDemul> ev_demul_;
};

#endif
