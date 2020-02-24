/**
    \file Timer.h
    Header file for (definition and implementation of) Timer class.

    \author George J. Grevera, Ph.D., ggrevera@sju.edu

    Copyright (C) 2002, George J. Grevera

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
    USA or from http://www.gnu.org/licenses/gpl.txt.

    This General Public License does not permit incorporating this
    code into proprietary programs.  (So a hypothetical company such
    as GH (Generally Hectic) should NOT incorporate this code into
    their proprietary programs.)
 */
#ifndef Timer_h
#define Timer_h
//----------------------------------------------------------------------
#include <iostream>
#include <stdio.h>
#include <time.h>

#ifndef WIN32
#  include <sys/time.h>
#  include <sys/times.h>
#  include <unistd.h>
#endif

using namespace std;
//----------------------------------------------------------------------
/// Timer class for reporting elapsed time and CPU time.
class Timer {
  private:
    static long    indent;  ///< indentation level for nested timers
    //for summary stats
    static double  best;    ///< keeps track of best
    static double  worst;   ///< keeps track of worst
    static double  total;   ///< keeps track of total time
    static long    count;   ///< keeps track of # of timers

    double  mStart;         ///< timer start to determine elapsed time
    #ifdef WIN32
        //long            tc_start;
        FILETIME  mKernelTime, mUserTime;
    #else
        //struct timeval  tv_start;
        struct tms  mStartUsage;
    #endif
    char*  msg;        ///< output messsage associated with timer
    bool   summary;    ///< output summary stats

  public:
    /** \brief ctor.
     *  \param summary produce summary information.
     */
    inline Timer ( const bool summary=false ) {
        Timer( (char*)NULL, summary );
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    /** \brief ctor.
     *  \param m message associated with timer.
     *  \param summary produce summary information.
     */
    inline Timer ( const char* const m, const bool summary=false ) {
        //init our "timer" to determine how long will this take.
        ++Timer::indent;
        this->msg = m;
        this->summary = summary;
        reset();
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    /** \brief reset timer to 0.
     */
    inline void reset ( void ) {
        #ifdef WIN32
            mStart = GetTickCount();

            FILETIME  creationTime, exitTime;
            GetProcessTimes( GetCurrentProcess(), &creationTime, &exitTime,
                &mKernelTime, &mUserTime );
        #else
            struct timeval   tv_start;
            struct timezone  tz_start;
            gettimeofday( &tv_start, &tz_start );
            mStart = tv_start.tv_sec + (tv_start.tv_usec / 1E6);

            times( &mStartUsage );
        #endif
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    /** \brief get elapsed time (in seconds).
     *  \returns elapsed time in seconds.
     */
    inline double getElapsedTime ( void ) {
        #ifdef WIN32
            long  tc_end = GetTickCount();
            return  (tc_end-mStart) / 1E3;
        #else
            struct timeval  tv_end;
            struct timezone tz_end;
            gettimeofday(&tv_end, &tz_end);
            double d_end   = tv_end.tv_sec + (tv_end.tv_usec / 1E6);
            return (d_end-mStart);
        #endif
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    /** \brief report the elapsed time.
     *
     *  note: many timers may be active at any given time.
     */
    inline void report ( void ) {
        for (int i=0; i<Timer::indent; i++)    cout << "  ";
        if (this->msg != NULL) {
            cout << this->msg << " ";
        }
        cout << "elapsed time=" << getElapsedTime() << "s,";
        cout << " CPU time=" << getCPUTime() << "s";

        if (this->summary) {
            cout << ", best=" << Timer::best << ", worst=" << Timer::worst
                 << ", average=" << (Timer::total/Timer::count);
        }

        cout << endl;
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    /** \brief get actual cpu time used.
     *  \returns actual cpu time used.
     */
    inline double getCPUTime ( void ) {
      #ifdef  WIN32
        FILETIME  creationTime, exitTime, kernelTime, userTime;
        GetProcessTimes( GetCurrentProcess(), &creationTime, &exitTime,
            &kernelTime, &userTime );
        //as recommended by bill:
        // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/filetime_str.asp
        ULARGE_INTEGER  u2;
        memcpy( &u2, &userTime, sizeof(userTime) );
        __int64*  i2 = (__int64*)&u2;
        ULARGE_INTEGER  u1;
        memcpy( &u1, &mUserTime, sizeof(mUserTime) );
        __int64*  i1 = (__int64*)&u1;
        __int64   i3 = *i2 - *i1;
        //from http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/filetime_str.asp
        // The FILETIME structure is a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
        const double  userCPUTime = i3 * 100 / 1E9;

        //determine kernel cpu time
        memcpy( &u2, &kernelTime, sizeof(kernelTime) );
        i2 = (__int64*)&u2;
        memcpy( &u1, &mKernelTime, sizeof(mKernelTime) );
        i1 = (__int64*)&u1;
        i3 = *i2 - *i1;
        const double  kernelCPUTime = i3 * 100 / 1E9;

        return userCPUTime + kernelCPUTime;
      #else
        struct tms  usageAfter;
        times( &usageAfter );
        double  cpuTime = ((double)usageAfter.tms_utime - mStartUsage.tms_utime)
                                 / sysconf(_SC_CLK_TCK);
        cpuTime += ((double)usageAfter.tms_stime - mStartUsage.tms_stime)
                                 / sysconf(_SC_CLK_TCK);
        return cpuTime;
      #endif
    }
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    /** \brief dtor.  report elapsed time.
     */
    inline ~Timer ( ) {
        const double t = getElapsedTime();
        if (this->summary) {
            Timer::total += t;
            ++Timer::count;
            if (t>Timer::worst)  Timer::worst=t;
            if (t<Timer::best)   Timer::best =t;
        }
        report();
        --Timer::indent;
    }
};

long   Timer::indent = 0;
double Timer::best   = FLT_MAX;
double Timer::worst  = 0.0;
double Timer::total  = 0.0;
long   Timer::count  = 0;

#endif
//----------------------------------------------------------------------
