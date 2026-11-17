#pragma once

#include <string>
#include <vector>

namespace cpuboost {

struct CpuInfo {
    std::string vendor = "Unknown";
    std::string brand = "Unknown";
    std::string application_architecture = "Unknown";
    unsigned int physical_cores = 0;
    unsigned int logical_processors = 0;
    unsigned int family = 0;
    unsigned int model = 0;
    unsigned int stepping = 0;
    unsigned int base_frequency_mhz = 0;
    unsigned int max_frequency_mhz = 0;
    std::vector<std::string> instruction_features;
};

CpuInfo detect_cpu();

} // namespace cpuboost
