#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds)
{
    const int SEC_PER_HOUR = 3600;
    const int SEC_PER_MINUTE = 60;

    long hours, minutes;
    
    hours = seconds / SEC_PER_HOUR;
    seconds %= SEC_PER_HOUR ;
    minutes = seconds / SEC_PER_MINUTE;
    seconds %= SEC_PER_MINUTE;

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":" << minutes << ":" << seconds; 

    return ss.str(); 
}