#include "processor.h"
#include "linux_parser.h"

// Dynamic update of utilization information based on Jiffies info returned from LinuxParser
float Processor::Utilization()
{
  long total_old, total_new, idle_old, idle_new, active_new;

  idle_new = CurrentIdle();
  active_new = CurrentActive();
  total_new = CurrentTotal();

  idle_old = PrevIdle();
  total_old = PrevTotal();

  Update(idle_new, active_new, total_new);

  float total_diff = float(total_new - total_old);
  float idle_diff = float(idle_new - idle_old);

  return (total_diff - idle_diff) / total_diff;
}

long Processor::CurrentTotal() const { return LinuxParser::Jiffies(); }
long Processor::CurrentActive() const { return LinuxParser::ActiveJiffies(); }
long Processor::CurrentIdle() const { return LinuxParser::IdleJiffies(); }

long Processor::PrevTotal() const { return total_; }
long Processor::PrevActive() const { return active_; }
long Processor::PrevIdle() const { return idle_; }

void Processor::Update(long idle, long active, long total) 
{
  idle_ = idle;
  active_ = active;
  total_ = total;
}