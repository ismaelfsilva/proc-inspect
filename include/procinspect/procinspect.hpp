#ifndef PROCINSPECT_PROCINSPECT_HPP
#define PROCINSPECT_PROCINSPECT_HPP

// Read-only Windows process / module / memory-region inspection.
// Uses Toolhelp (CreateToolhelp32Snapshot, Process32/Module32) and
// VirtualQueryEx. No process memory is read, written, injected or modified.

#include <windows.h>
#include <tlhelp32.h>
#include <cstdint>
#include <string>
#include <vector>

namespace procinspect {

struct ProcessInfo {
    DWORD pid;
    std::wstring name;
};

struct ModuleInfo {
    std::wstring name;
    std::uintptr_t base;
    std::size_t size;
};

struct RegionInfo {
    std::uintptr_t baseAddress;
    std::size_t regionSize;
    DWORD state;
    DWORD protect;
};

// Lists all processes visible to the caller (pid, name).
inline std::vector<ProcessInfo> listProcesses() {
    std::vector<ProcessInfo> result;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return result;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            result.push_back({pe.th32ProcessID, pe.szExeFile});
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return result;
}

// Lists modules loaded in the given process (name, base, size).
inline std::vector<ModuleInfo> listModules(DWORD pid) {
    std::vector<ModuleInfo> result;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snap == INVALID_HANDLE_VALUE) return result;
    MODULEENTRY32W me{};
    me.dwSize = sizeof(me);
    if (Module32FirstW(snap, &me)) {
        do {
            result.push_back({me.szModule,
                              reinterpret_cast<std::uintptr_t>(me.modBaseAddr),
                              static_cast<std::size_t>(me.modBaseSize)});
        } while (Module32NextW(snap, &me));
    }
    CloseHandle(snap);
    return result;
}

// Lists memory regions of the given process via VirtualQueryEx (read-only query).
inline std::vector<RegionInfo> listRegions(DWORD pid) {
    std::vector<RegionInfo> result;
    HANDLE proc = (pid == GetCurrentProcessId())
                      ? GetCurrentProcess()
                      : OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!proc) return result;

    MEMORY_BASIC_INFORMATION mbi{};
    std::uintptr_t addr = 0;
    while (VirtualQueryEx(proc, reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi)) == sizeof(mbi)) {
        result.push_back({reinterpret_cast<std::uintptr_t>(mbi.BaseAddress),
                          mbi.RegionSize, mbi.State, mbi.Protect});
        std::uintptr_t next = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        if (next <= addr) break;  // guard against wrap / no progress
        addr = next;
    }

    if (proc != GetCurrentProcess()) CloseHandle(proc);
    return result;
}

}  // namespace procinspect

#endif  // PROCINSPECT_PROCINSPECT_HPP
