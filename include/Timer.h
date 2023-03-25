//! @file Timer.h

#ifndef __Nobody_Timer__
#define __Nobody_Timer__

namespace Nobody
{
	class Timer
	{
	public:
		// 增量时间
		static float deltaTime;
		// 总共经过的时间
		static unsigned int ticksCount;
	};
}

#endif	