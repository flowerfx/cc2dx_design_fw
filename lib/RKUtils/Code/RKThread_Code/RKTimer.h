///----------------------------------------------------------------------
/// RKEngine	(c) 2005 + Daniel Stephens
///
/// File		:	timer.h
///
/// Description :	X86 High resolution timer class
///----------------------------------------------------------------------
#ifndef RKTIMER_H
#define RKTIMER_H

#include "../Common/RK.h"

namespace RKUtils {
    ///----------------------------------------------------------------------
    /// Includes
    ///----------------------------------------------------------------------
    void RKTimer_InitModule();
    void RKTimer_DeinitModule();

    float RKTimer_GetUpTime();
    float RKTimer_GetElapsedTime();
    float RKTimer_GetBufferedElapsedTime();
    float RKTimer_GetBufferedUpTime();

///----------------------------------------------------------------------
/// Class RKTimer
///----------------------------------------------------------------------
    class RKTimer
    {
    public:
        RKTimer();
        ~RKTimer();

        void	Init();							  // start the timer
        float GetUpTime();          // time since the timer was initialized
        float	GetElapsedTime();			// get time since last call to GetElapsedTime
        float GetBufferedUpTime();
        float	GetBufferedElapsedTime();

    protected:
        float m_FrameTime;
        float m_UpTime;
#if defined (RKPLATFORM_IOS)
        double m_InitTime;
        double m_LastTime;
#elif defined (RKPLATFORM_WINDOWS)
        LARGE_INTEGER m_PerformanceFrequency;
        LARGE_INTEGER m_InitTime;
        LARGE_INTEGER	m_LastTime;
#elif defined (RKPLATFORM_ANDROID)
        double m_InitTime;
        double m_LastTime;
#endif // RKPLATFORM_IOS
    };
}
///----------------------------------------------------------------------
#endif // RKTIMER_H
