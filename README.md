# CPUBoost AI

**Lightweight CPU identification and, over time, safe performance insights for Intel and AMD computers.**

CPUBoost AI is an open-source project focused on helping people understand their processor and make informed decisions about computer performance. It is designed with older and low-end hardware in mind.

The project follows a safety-first approach: it reads hardware and operating-system information, explains its limitations, and never claims that software can change a processor's physical capabilities.

> **Current version:** 0.1.0-dev  
> **Project status:** Early development. The first read-only C++ CPU identification utility is implemented. Monitoring, temperature reporting, AI recommendations, benchmarking, and the desktop dashboard are not implemented yet.

## What works now

The current command-line prototype can report:

- CPU manufacturer/vendor string using CPUID.
- CPU brand/model name where the processor provides it.
- CPU family, model, and stepping.
- Application build architecture (such as x86 or x86_64).
- Logical processor count reported by the operating system.
- Physical core count using Windows processor-topology APIs or Linux `/proc/cpuinfo`, where available.
- Selected CPU instruction-set hardware flags, including SSE-family features and AVX/AVX2 flags.
- Nominal base and maximum frequency when CPUID leaf 0x16 provides those values.
- Basic `--help` and `--version` commands.

Unavailable information is shown as unavailable rather than guessed.

### Important measurement notes

- Reported base and maximum frequencies are not live clock-speed measurements.
- AVX and AVX2 are shown as CPU hardware flags. This prototype does not yet verify whether the operating system has enabled the required extended register state for application use.
- Physical-core counts depend on operating-system information and may be unavailable in restricted environments or virtual machines.
- The current prototype is read-only. It does not change power settings, clocks, voltage, or hardware configuration.

## Planned features

| Area | Planned capability | Status |
|---|---|---|
| CPU identification | Vendor, brand, family/model, topology, instruction features | Initial prototype implemented; testing continues |
| Live CPU monitoring | CPU utilization and live frequency | Planned |
| Memory monitoring | RAM utilization and available memory | Planned |
| Temperature | CPU temperature where trustworthy sensor access exists | Planned |
| Workload analysis | Identify unusually heavy background workloads | Planned |
| Safe recommendations | Explain possible performance improvements without risky changes | Planned |
| AI assistance | Help interpret measurements and prioritize recommendations | Planned |
| Desktop dashboard | Lightweight Qt interface | Planned |
| Benchmarking | Repeatable tests and transparent result comparisons | Planned |

## Technology

The project is being built with free and open-source tools wherever practical.

| Technology | Purpose |
|---|---|
| C++17 | CPU information engine |
| CMake | Cross-platform build configuration |
| Python | Planned monitoring and analysis components |
| Qt | Planned desktop dashboard |
| Git and GitHub | Version control, collaboration, and releases |

The first CPU information utility has no third-party C++ library dependency.

## Build from source

### Requirements

- CMake 3.16 or newer.
- A C++17 compiler.
- Windows: Visual Studio 2019/2022 C++ build tools, or another supported C++17 toolchain.
- Linux: GCC or Clang with C++17 support.

### Windows

Open PowerShell or Command Prompt in the repository folder:

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\cpuboost-ai.exe
```

Check the version or help:

```powershell
.\build\Release\cpuboost-ai.exe --version
.\build\Release\cpuboost-ai.exe --help
```

### Linux

Open a terminal in the repository folder:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
./build/cpuboost-ai
```

Check the version or help:

```bash
./build/cpuboost-ai --version
./build/cpuboost-ai --help
```

## Development roadmap

The project will move forward through small, verifiable milestones.

| Stage | Milestone | Status |
|---|---|---|
| 1 | Repository setup and professional documentation | Completed |
| 2 | CPU detection engine | Initial implementation; Windows and target-hardware testing pending |
| 3 | CPU and memory monitoring | Not started |
| 4 | Safe performance analysis and recommendations | Not started |
| 5 | AI-assisted decision engine | Not started |
| 6 | Lightweight desktop dashboard | Not started |
| 7 | Benchmarking and result reports | Not started |
| 8 | Intel/AMD and Windows/Linux compatibility testing | In progress |

A milestone will only be marked complete after the implementation has been tested on its intended platforms and hardware.

## Compatibility and first test machine

The initial development target is:

| Component | Specification |
|---|---|
| Laptop | HP 240 G3 |
| Processor | Intel Pentium N3540 |
| Memory | 4 GB RAM |
| Operating system | Tiny11 64-bit |
| Graphics | Integrated graphics; no dedicated GPU |

The long-term goal is support for Intel and AMD processors on Windows and Linux. Compatibility is not guaranteed for every CPU, sensor, firmware, or operating-system version until it has been tested.

## Safety policy

CPUBoost AI prioritizes hardware safety, stability, and transparency.

The project will never:

- Force CPU overclocking or voltage changes.
- Disable thermal protection or essential safety mechanisms.
- Attempt unsupported hardware modifications.
- Claim to unlock physically unavailable CPU cores or features.
- Promise that software can turn a low-end processor into a high-end processor.

The project will:

- Prefer read-only monitoring and recommendations.
- Explain why a recommendation is being made.
- Respect operating-system and firmware power-management controls.
- Treat temperature and sensor readings cautiously.
- Require clear user awareness and consent before any future system-level setting change.
- Report limitations and unavailable measurements honestly.

**CPUBoost AI cannot change the physical limits of a processor.** Real performance depends on the CPU, cooling, power limits, memory, software, and workload.

## CI and releases

GitHub Actions is configured to build the project on Windows and Linux when changes are pushed to `main` or a pull request is opened.

A tagged version matching `v*` starts the release workflow. It builds platform binaries and creates a GitHub Release with generated release notes.

**Do not create the stable `v1.0.0` tag yet.** The project will be ready for 1.0.0 only after the planned core features are implemented, Windows and Linux builds pass, and testing is completed on supported Intel and AMD hardware. Development versions will use the `0.x` series until then.

## Contributing

Bug reports, testing feedback, and suggestions are welcome. When reporting a problem, include:

- Operating system and version.
- CPU manufacturer and model, if known.
- Steps to reproduce the problem.
- Relevant logs or screenshots, with personal information removed.

Never share passwords, private keys, or other sensitive system information.

## License

CPUBoost AI is distributed under the **MIT License**. See [LICENSE](LICENSE) for the complete license text.

---

**Developer:** Karan Mandal  
**Repository:** [karanmandal4611/CPUBoost-AI](https://github.com/karanmandal4611/CPUBoost-AI)

*An open-source engineering project focused on useful measurements, safe recommendations, and realistic performance improvements.*
