
#ifndef __Verdi_Timer_H__
#define __Verdi_Timer_H__

#include "VerdiPrerequisites.h"

namespace Verdi {

	/** Timer implementation, used for measuring frame times 
		and frame rate amongst other things.
	*/
	class Timer
	{
	protected:
		uint64_t mResetTime;
		double mFrequency;
	public:
		static uint64_t getTickCount();
	public:

		/** Default constructor.
		*/
		Timer();

		/** Default destructor.
		*/
		~Timer();

		/** Reset the timer.
		*/
		void reset();
		
		/** Get the number of seconds since last reset.
		*/
		double getSeconds() const;

		/** Get the number of milliseconds
			since last reset.
		*/
		uint64_t getMilliseconds() const;
	};
}

#endif // __Verdi_Timer_H__