#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes()
{
    vector<int> pids = LinuxParser::Pids();
    processes_.clear();
    for (int pid : pids)
    {
        Process process(pid);
        processes_.push_back(process);
    }
    std::sort(processes_.rbegin(), processes_.rend());
    return processes_;
}

// Return the system's memory utilization
float System::MemoryUtilization() const { return LinuxParser::MemoryUtilization(); }

// Return the number of seconds since the system started running
long int System::UpTime() const { return LinuxParser::UpTime(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() const { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() const { return LinuxParser::TotalProcesses(); }

// Return the system's kernel identifier (string)
std::string System::Kernel() const { return LinuxParser::Kernel(); }

// Return the operating system name
std::string System::OperatingSystem() const { return LinuxParser::OperatingSystem(); }
