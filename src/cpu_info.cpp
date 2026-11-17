#include "cpu_info.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <set>
#include <string>
#include <thread>
#include <vector>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#include <intrin.h>
#elif (defined(__i386__) || defined(__x86_64__)) && (defined(__GNUC__) || defined(__clang__))
#include <cpuid.h>
#endif

namespace cpuboost {
namespace {

bool cpuid(unsigned int leaf, unsigned int subleaf, unsigned int out[4]) {
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    int regs[4] = {};
    __cpuidex(regs, static_cast<int>(leaf), static_cast<int>(subleaf));
    for (int i = 0; i < 4; ++i) out[i] = static_cast<unsigned int>(regs[i]);
    return true;
#elif (defined(__i386__) || defined(__x86_64__)) && (defined(__GNUC__) || defined(__clang__))
    return __get_cpuid_count(leaf, subleaf, &out[0], &out[1], &out[2], &out[3]) != 0;
#else
    (void)leaf;
    (void)subleaf;
    (void)out;
    return false;
#endif
}

std::string trim(const std::string& value) {
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
        return std::isspace(c) != 0;
    }).base();
    if (first >= last) return {};
    return std::string(first, last);
}

[[maybe_unused]] unsigned int detect_linux_physical_cores() {
#if defined(__linux__)
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (!cpuinfo) return 0;

    std::set<std::string> unique_cores;
    std::string line;
    std::string package_id;
    std::string core_id;

    const auto save_core = [&]() {
        if (!core_id.empty()) {
            unique_cores.insert((package_id.empty() ? "0" : package_id) + ":" + core_id);
        }
        package_id.clear();
        core_id.clear();
    };

    while (std::getline(cpuinfo, line)) {
        if (line.empty()) {
            save_core();
            continue;
        }
        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        const std::string key = trim(line.substr(0, colon));
        const std::string value = trim(line.substr(colon + 1));
        if (key == "physical id") package_id = value;
        else if (key == "core id") core_id = value;
    }
    save_core();
    return static_cast<unsigned int>(unique_cores.size());
#else
    return 0;
#endif
}

[[maybe_unused]] unsigned int detect_windows_physical_cores() {
#if defined(_WIN32)
    DWORD bytes = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &bytes);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || bytes == 0) return 0;

    std::vector<unsigned char> buffer(bytes);
    if (!GetLogicalProcessorInformationEx(
            RelationProcessorCore,
            reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data()),
            &bytes)) {
        return 0;
    }

    unsigned int count = 0;
    DWORD offset = 0;
    while (offset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX) <= bytes) {
        const auto* item = reinterpret_cast<const SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(buffer.data() + offset);
        if (item->Size == 0 || offset + item->Size > bytes) break;
        if (item->Relationship == RelationProcessorCore) ++count;
        offset += item->Size;
    }
    return count;
#else
    return 0;
#endif
}

void add_feature(std::vector<std::string>& features, bool supported, const char* name) {
    if (supported) features.emplace_back(name);
}

} // namespace

CpuInfo detect_cpu() {
    CpuInfo info;
    info.logical_processors = std::thread::hardware_concurrency();

#if defined(_M_X64) || defined(__x86_64__)
    info.application_architecture = "x86_64";
#elif defined(_M_IX86) || defined(__i386__)
    info.application_architecture = "x86";
#elif defined(_M_ARM64) || defined(__aarch64__)
    info.application_architecture = "ARM64";
#endif

#if defined(_WIN32)
    info.physical_cores = detect_windows_physical_cores();
#elif defined(__linux__)
    info.physical_cores = detect_linux_physical_cores();
#endif

    unsigned int regs[4] = {};
    if (!cpuid(0, 0, regs)) return info;

    const unsigned int max_basic_leaf = regs[0];
    char vendor[13] = {};
    std::memcpy(vendor + 0, &regs[1], sizeof(regs[1]));
    std::memcpy(vendor + 4, &regs[3], sizeof(regs[3]));
    std::memcpy(vendor + 8, &regs[2], sizeof(regs[2]));
    info.vendor = vendor;

    unsigned int max_extended_leaf = 0;
    if (cpuid(0x80000000U, 0, regs)) max_extended_leaf = regs[0];
    if (max_extended_leaf >= 0x80000004U) {
        char brand[49] = {};
        for (unsigned int i = 0; i < 3; ++i) {
            if (!cpuid(0x80000002U + i, 0, regs)) break;
            for (unsigned int j = 0; j < 4; ++j) {
                const unsigned int value = regs[j];
                brand[i * 16 + j * 4 + 0] = static_cast<char>(value & 0xFFU);
                brand[i * 16 + j * 4 + 1] = static_cast<char>((value >> 8U) & 0xFFU);
                brand[i * 16 + j * 4 + 2] = static_cast<char>((value >> 16U) & 0xFFU);
                brand[i * 16 + j * 4 + 3] = static_cast<char>((value >> 24U) & 0xFFU);
            }
        }
        info.brand = trim(brand);
    }

    if (max_basic_leaf >= 1 && cpuid(1, 0, regs)) {
        const unsigned int eax = regs[0];
        const unsigned int ecx = regs[2];
        const unsigned int edx = regs[3];
        const unsigned int base_family = (eax >> 8U) & 0xFU;
        const unsigned int base_model = (eax >> 4U) & 0xFU;
        const unsigned int extended_family = (eax >> 20U) & 0xFFU;
        const unsigned int extended_model = (eax >> 16U) & 0xFU;

        info.family = base_family;
        if (base_family == 0xFU) info.family += extended_family;
        info.model = base_model;
        if (base_family == 0x6U || base_family == 0xFU) info.model += (extended_model << 4U);
        info.stepping = eax & 0xFU;

        add_feature(info.instruction_features, (edx & (1U << 25U)) != 0, "SSE");
        add_feature(info.instruction_features, (edx & (1U << 26U)) != 0, "SSE2");
        add_feature(info.instruction_features, (ecx & (1U << 0U)) != 0, "SSE3");
        add_feature(info.instruction_features, (ecx & (1U << 9U)) != 0, "SSSE3");
        add_feature(info.instruction_features, (ecx & (1U << 19U)) != 0, "SSE4.1");
        add_feature(info.instruction_features, (ecx & (1U << 20U)) != 0, "SSE4.2");
        add_feature(info.instruction_features, (ecx & (1U << 28U)) != 0, "AVX (hardware flag)");
    }

    if (max_basic_leaf >= 7 && cpuid(7, 0, regs)) {
        add_feature(info.instruction_features, (regs[1] & (1U << 5U)) != 0, "AVX2 (hardware flag)");
    }

    if (max_basic_leaf >= 0x16U && cpuid(0x16U, 0, regs)) {
        info.base_frequency_mhz = regs[0];
        info.max_frequency_mhz = regs[1];
    }

    return info;
}

} // namespace cpuboost
