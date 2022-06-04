#include <string>

#include "format.h"

using std::string;
using std::to_string;

static constexpr int seconds_in_hour{3600};
static constexpr int seconds_in_minute{60};

string FormatTimeDigit (int time_digit) {
    if (time_digit >=0 && time_digit <=9) {
        return ('0' + to_string(time_digit));
    } else {
        return to_string(time_digit);
    }
}

string Format::ElapsedTime(long seconds) {
    int hour, min, sec;
    hour = seconds / seconds_in_hour;
    min = (seconds % seconds_in_hour) / seconds_in_minute;
    sec = (seconds % seconds_in_hour) % seconds_in_minute;
    return (FormatTimeDigit(hour) + ":" + FormatTimeDigit(min) + ":" + FormatTimeDigit(sec));
}