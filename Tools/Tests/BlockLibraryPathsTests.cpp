#include "../../BlockMaker/LithweftBlocks/BlockLibraryPaths.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

int main() {
    namespace fs = std::filesystem;
    const auto root = fs::temp_directory_path() / ("archive path tests " + std::to_string(getpid()));
    struct Cleanup { fs::path path; ~Cleanup() { std::error_code ec; fs::remove_all(path, ec); } } cleanup {root};
    try {
        const auto checkout = root / "renamed project";
        const auto sourceModules = checkout / "BlockMaker/LithweftBlocks";
        const auto executable = checkout / "build/Sample.app/Contents/MacOS/Sample";
        const auto bundledModules = executable.parent_path().parent_path() / "Resources/Blocks";
        const auto overrideModules = root / "custom modules";
        const auto make = [](const fs::path& directory) { fs::create_directories(directory); std::ofstream(directory / "divide.dylib") << "fixture"; };
        make(sourceModules);
        make(bundledModules);
        make(overrideModules);
        int checks = 0;
        const auto check = [&](bool condition) { ++checks; if (!condition) throw std::runtime_error("module path check failed"); };
        const std::string oldEnvironment = std::getenv("LITHWEFT_BLOCKS_DIR") ? std::getenv("LITHWEFT_BLOCKS_DIR") : "";
        unsetenv("LITHWEFT_BLOCKS_DIR");
        check(BlockLibraryPaths::find("divide", executable, root) == bundledModules / "divide.dylib");
        fs::remove(bundledModules / "divide.dylib");
        check(BlockLibraryPaths::find("divide", executable, root) == sourceModules / "divide.dylib");
        setenv("LITHWEFT_BLOCKS_DIR", overrideModules.c_str(), 1);
        check(BlockLibraryPaths::find("divide", executable, root) == overrideModules / "divide.dylib");
        check(BlockLibraryPaths::find("missing", executable, root).empty());
        for (const auto& invalid : {"", "../divide", "/divide", "a/b", "a\\b"})
            check(BlockLibraryPaths::find(invalid, executable, root).empty());
        if (oldEnvironment.empty()) unsetenv("LITHWEFT_BLOCKS_DIR");
        else setenv("LITHWEFT_BLOCKS_DIR", oldEnvironment.c_str(), 1);
        std::cout << "PASS: " << checks << " portable module path checks\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
