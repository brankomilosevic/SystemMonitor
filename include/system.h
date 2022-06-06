#ifndef SYSTEM_H
#define SYSTEM_H

#include "process.h"
#include "processor.h"

/*
Basic class for System representation
It contains relevant attributes as shown below
*/
class System {
public:
  Processor& Cpu();                   
  std::vector<Process>& Processes();  
  float MemoryUtilization() const;          
  long UpTime() const;                      
  int TotalProcesses() const;               
  int RunningProcesses() const;             
  std::string Kernel() const;               
  std::string OperatingSystem() const;      

private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif