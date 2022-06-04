#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();
 private:
  float prev_total_cpu_time{0};
  float prev_total_idle_time{0};
};

#endif