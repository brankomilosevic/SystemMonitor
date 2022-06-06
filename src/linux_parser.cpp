#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
// #include <filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Read data from the filesystem
// Return the name of Operating System
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

// Read data from the filesystem
// Return the Kernel version
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
/*
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for (auto& p : std::filesystem::directory_iterator(kProcDirectory)) {
    string filename = p.path().filename();
    if (p.is_directory() &&
        std::all_of(filename.begin(), filename.end(), isdigit)) {
      int pid = stoi(filename);
      pids.push_back(pid);
    }
  }
  return pids;
}
*/
// Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
  string key, line;
  int value, mem_total, mem_free;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  
  if (stream.is_open()) 
  {
    while (std::getline(stream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value) 
      {
        if (key == "MemTotal:") mem_total = value;
        if (key == "MemFree:") mem_free = value;
      }
    }
  }
  return float(mem_total - mem_free) / float(mem_total);
}

// Read and return the system uptime
long LinuxParser::UpTime()
{
  long uptime {0};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  
  if (stream.is_open())
    stream >> uptime;
  return uptime;
}

// Read and return the number of Jiffies for the system
long LinuxParser::Jiffies()
{
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid)
{
  long totaltime;
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) 
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) 
      values.push_back(value);
  }

  long utime = 0, stime = 0 , cutime = 0, cstime = 0;
  
  const unsigned char UTIME_POS = 13;
  const unsigned char STIME_POS = 14;
  const unsigned char CUTIME_POS = 15;
  const unsigned char CSTIME_POS = 16;

  if (std::all_of(values[UTIME_POS].begin(), values[UTIME_POS].end(), isdigit)) utime = stol(values[UTIME_POS]);
  if (std::all_of(values[STIME_POS].begin(), values[STIME_POS].end(), isdigit)) stime = stol(values[STIME_POS]);
  if (std::all_of(values[CUTIME_POS].begin(), values[CUTIME_POS].end(), isdigit)) cutime = stol(values[CUTIME_POS]);
  if (std::all_of(values[CSTIME_POS].begin(), values[CSTIME_POS].end(), isdigit)) cstime = stol(values[CSTIME_POS]);

  totaltime = utime + stime + cutime + cstime;
  return totaltime / sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies()
{
  vector<string> jiffies = CpuUtilization();

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
vector<string> LinuxParser::CpuUtilization()
{
  string key, line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> value)
        values.push_back(value);
  }
  return values;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
  string key, line;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "processes")
          return value;
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
  string key, line;
  int value;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "procs_running")
          return value;
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid)
{
  string command;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open())
    std::getline(stream, command);
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{
  string key, line;
  long value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key)
      {
        if (key == "VmSize:")
        {
          linestream >> value;
          return to_string(value / 1024);
        }
      }
    }
  }
  return "0";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{
  string key, line, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      while (linestream >> key >> value)
      {
        if (key == "Uid:")
          return value;
      }
    }
  }
  return "0";
}

// Read and return the user associated with a process
string LinuxParser::User(int pid)
{
  string line, name, dummy, uid;
  std::ifstream stream(kPasswordPath);

  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> dummy >> uid)
      {
        if (uid == LinuxParser::Uid(pid))
          return name;
      }
    }
  }
  return "_unknown_";
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid)
{
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value)
      values.push_back(value);

  }
  const unsigned char POS = 21;
  return stol(values[POS]) / sysconf(_SC_CLK_TCK);
}