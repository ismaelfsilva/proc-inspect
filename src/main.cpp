// proc-inspect CLI: read-only process / module / memory-region inspector.

#include <procinspect/procinspect.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace {

void printUsage() {
    std::printf(
        "proc-inspect - read-only Windows process inspector\n"
        "Usage:\n"
        "  proc-inspect ps              list all processes\n"
        "  proc-inspect mods [pid]      list modules (default: current process)\n"
        "  proc-inspect mem  [pid]      list memory regions (default: current process)\n");
}

DWORD parsePid(int argc, char** argv, int idx) {
    if (idx < argc) return static_cast<DWORD>(std::strtoul(argv[idx], nullptr, 10));
    return GetCurrentProcessId();
}

const char* stateStr(DWORD s) {
    switch (s) {
        case MEM_COMMIT:  return "COMMIT";
        case MEM_RESERVE: return "RESERVE";
        case MEM_FREE:    return "FREE";
        default:          return "?";
    }
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    const std::string cmd = argv[1];

    if (cmd == "ps") {
        for (const auto& p : procinspect::listProcesses())
            std::printf("%6lu  %ls\n", static_cast<unsigned long>(p.pid), p.name.c_str());
        return 0;
    }

    if (cmd == "mods") {
        DWORD pid = parsePid(argc, argv, 2);
        for (const auto& m : procinspect::listModules(pid))
            std::printf("0x%016llx  %10zu  %ls\n",
                        static_cast<unsigned long long>(m.base), m.size, m.name.c_str());
        return 0;
    }

    if (cmd == "mem") {
        DWORD pid = parsePid(argc, argv, 2);
        for (const auto& r : procinspect::listRegions(pid))
            std::printf("0x%016llx  %12zu  %-8s  0x%08lx\n",
                        static_cast<unsigned long long>(r.baseAddress), r.regionSize,
                        stateStr(r.state), static_cast<unsigned long>(r.protect));
        return 0;
    }

    printUsage();
    return 1;
}
