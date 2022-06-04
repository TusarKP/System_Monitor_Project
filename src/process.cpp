#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
    int pid = Process::Pid();
    string line, key, value;
    vector<string> process_stat_data;
    long utime, stime, cutime, cstime, starttime, uptime, hertz, seconds, total_time {0};
    std::ifstream filestream(LinuxParser::kProcDirectory+ to_string(pid) + LinuxParser::kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        while (linestream >> value) {
            process_stat_data.push_back(value);
        }
    }
    uptime = LinuxParser::UpTime();
    hertz = sysconf(_SC_CLK_TCK);
    utime = stol(process_stat_data[13]);
    stime = stol(process_stat_data[14]);
    cutime = stol(process_stat_data[15]);
    cstime = stol(process_stat_data[16]);
    starttime = stol(process_stat_data[21]);
    total_time = utime + stime;
    total_time = total_time + cutime + cstime;
    seconds = uptime - (starttime / hertz);
    cpuUtilization_ = static_cast<float>(100 * ((total_time / hertz) / seconds));
    return cpuUtilization_;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return cpuUtilization_ < a.cpuUtilization_; }