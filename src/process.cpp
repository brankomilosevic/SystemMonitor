#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Construct process from process_id
Process::Process (int pid) : pid_{pid}
{}

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const 
{ 
  long seconds = LinuxParser::UpTime() - LinuxParser::UpTime(pid_);
  long total_time = LinuxParser::ActiveJiffies(pid_);
  return float(total_time) / float(seconds);
}

// Return the command that generated this process
string Process::Command() const { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// Comparison operator (CPU utilization)
bool Process::operator<(Process const& a) const { return CpuUtilization() < a.CpuUtilization(); }

// Comparison operator (RAM consumption)
// bool Process::operator<(Process const& a) const { return Ram() < a.Ram(); }
