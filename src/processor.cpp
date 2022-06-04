#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    float active_time = static_cast<float>(LinuxParser::ActiveJiffies());
    float idle_time = static_cast<float>(LinuxParser::IdleJiffies());

    float cur_total_cpu_time = active_time + idle_time;
    
    float delta_total_cpu_time = cur_total_cpu_time - prev_total_cpu_time;
    float delta_total_idle_time = idle_time - prev_total_idle_time;
    
    prev_total_cpu_time = active_time + idle_time;
    prev_total_idle_time = idle_time;
    
    return (delta_total_cpu_time - delta_total_idle_time) / delta_total_cpu_time;
}