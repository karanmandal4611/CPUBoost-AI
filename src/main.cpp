#include "cpu_info.hpp"

#include <iostream>
#include <string>

namespace {

void print_value(const char* label, const std::string& value) {
    std::cout << "  " << label << ": " << (value.empty() ? "Not reported" : value) << '\n';
}

void print_number(const char* label, unsigned int value, const char* suffix = "") {
    std::cout << "  " << label << ": ";
    if (value == 0) std::cout << "Not reported";
    else std::cout << value << suffix;
    std::cout << '\n';
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc > 1) {
        const std::string argument = argv[1];
        if (argument == "--version" || argument == "-v") {
            std::cout << "CPUBoost AI 0.1.0-dev\n";
            return 0;
        }
        if (argument == "--help" || argument == "-h") {
            std::cout << "CPUBoost AI - read-only CPU information utility\n"
                      << "Usage: cpuboost-ai [--help] [--version]\n";
            return 0;
        }
    }

    const cpuboost::CpuInfo cpu = cpuboost::detect_cpu();

    std::cout << "====================================\n"
              << "          CPUBoost AI 0.1.0         \n"
              << "     Read-only CPU information      \n"
              << "====================================\n\n";

    print_value("Manufacturer", cpu.vendor);
    print_value("Processor", cpu.brand);
    print_value("App architecture", cpu.application_architecture);
    print_number("Physical cores", cpu.physical_cores);
    print_number("Logical processors", cpu.logical_processors);

    if (cpu.family != 0 || cpu.model != 0) {
        std::cout << "  Family / model / stepping: "
                  << cpu.family << " / " << cpu.model << " / " << cpu.stepping << '\n';
    } else {
        print_value("Family / model / stepping", "Not reported");
    }

    print_number("Reported base frequency", cpu.base_frequency_mhz, " MHz");
    print_number("Reported maximum frequency", cpu.max_frequency_mhz, " MHz");

    std::cout << "\nInstruction-set features (hardware flags):\n";
    if (cpu.instruction_features.empty()) {
        std::cout << "  No supported features reported or CPUID unavailable.\n";
    } else {
        for (const auto& feature : cpu.instruction_features) {
            std::cout << "  - " << feature << '\n';
        }
    }

    std::cout << "\nNotes:\n"
              << "  - Frequency values are reported nominal values, not live clock readings.\n"
              << "  - AVX flags describe CPU hardware support; OS state support is not checked yet.\n"
              << "  - This version only reads system information and does not change settings.\n"
              << "  - Some details may be unavailable on certain CPUs or operating systems.\n";

    return 0;
}
