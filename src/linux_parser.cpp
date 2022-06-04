#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stol;
using std::stoi;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float MemTotal{0.0}, MemFree{0.0};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          MemTotal = stof(value);
        } else if (key == "MemFree:") {
          MemFree = stof(value);
        }
      }
    }
  }
  return (MemTotal - MemFree) / MemTotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line, the_uptime_of_the_system, time_spent_in_the_idle_process;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> the_uptime_of_the_system >> time_spent_in_the_idle_process) {
      }
    }
  }
  return stol(the_uptime_of_the_system);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long total_jiffies = 0;
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> value) {
      total_jiffies += stol(value);
      }
    }
    return total_jiffies;
  }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long active_jiffies_for_pid = 0;
  string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 17; ++i) {
      if (i < 13) {
      linestream >> key;
      }
      // Add only utime, stime, cutime, cstime
      if (i >= 13 && i < 17) {
        linestream >> value;
        active_jiffies_for_pid += stol(value);
      }
    }
  }
  return active_jiffies_for_pid;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return stol(jiffies[CPUStates::kUser_]) +
         stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) +
         stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) +
         stol(jiffies[CPUStates::kSteal_]);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return stol(jiffies[CPUStates::kIdle_]) +
         stol(jiffies[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, key, value;
  vector<string> combined_cpu_data;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> value) {
      combined_cpu_data.push_back(value);
    }
  }
  return combined_cpu_data;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, value;
  int total_processes{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key >> value) {
      if (key == "processes") {
        total_processes = stoi(value);
      }
    }
  }
  return total_processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key, value;
  int runningprocesses{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key >> value) {
      if (key == "procs_running") {
        runningprocesses = stoi(value);
      }
    }
  }
  return runningprocesses;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key, value, memory_used_by_process;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          memory_used_by_process = to_string(stol(value) / 1024);
        }
      }
    }
  }
  return memory_used_by_process;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, value, uid{};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          uid = value;
        }
      }
    }
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, key, value, _user ;
  string uid = LinuxParser::Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::replace(line.begin(), line.end(), ':', ' ');
    std::replace(line.begin(), line.end(), 'x', ' ');
    std::istringstream linestream(line);
    while (linestream >> key >> value) {
      if (key == uid) {
        _user = value;
      }
    }
  }
  return _user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, key, value;
  long uptime{0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; i++) {
      linestream >> key;
    }
    linestream >> value;
    uptime = stol(value) / sysconf(_SC_CLK_TCK);
  }
  return uptime;
}
