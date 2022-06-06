#ifndef PROCESSOR_H
#define PROCESSOR_H

/*
Basic class for Processor representation
It contains relevant attributes as shown below
*/
class Processor {
public:
  float Utilization();

private:
  long CurrentTotal() const;
  long CurrentActive() const;
  long CurrentIdle() const;

  long PrevTotal() const;
  long PrevIdle() const;
  long PrevActive() const;

  void Update(long idle, long active, long total);
  
  long idle_;
  long active_;
  long total_;
};

#endif