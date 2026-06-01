// Local Windows tests for the procinspect library.
// Note: Release builds define NDEBUG (asserts disabled), so each check also
// returns a non-zero exit code on failure to remain meaningful under ctest.

#include <procinspect/procinspect.hpp>

#include <cassert>
#include <cstdio>

#define CHECK(cond)                                  \
    do {                                             \
        assert(cond);                                \
        if (!(cond)) {                               \
            std::fprintf(stderr, "FAIL: %s\n", #cond); \
            return 1;                                \
        }                                            \
    } while (0)

int main() {
    const DWORD self = GetCurrentProcessId();

    auto procs = procinspect::listProcesses();
    CHECK(!procs.empty());
    bool foundSelf = false;
    for (const auto& p : procs)
        if (p.pid == self) { foundSelf = true; break; }
    CHECK(foundSelf);

    auto mods = procinspect::listModules(self);
    CHECK(!mods.empty());

    auto regions = procinspect::listRegions(self);
    CHECK(!regions.empty());

    std::printf("OK: %zu processes, %zu modules, %zu regions\n",
                procs.size(), mods.size(), regions.size());
    return 0;
}
