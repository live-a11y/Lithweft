#pragma once
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

namespace BlockLibraryPaths {
namespace fs = std::filesystem;

inline bool validModuleName(const std::string& name) {
    return !name.empty() && name.find_first_of("/\\") == std::string::npos
        && name.find("..") == std::string::npos;
}

inline std::vector<fs::path> directories(const fs::path& executable,
                                        const fs::path& workingDirectory = fs::current_path()) {
    std::vector<fs::path> result;
    if (const auto* overridePath = std::getenv("LITHWEFT_BLOCKS_DIR"))
        if (*overridePath != '\0') result.emplace_back(overridePath);
    const auto executableDirectory = fs::absolute(executable).parent_path();
    result.push_back(executableDirectory.parent_path() / "Resources" / "Blocks");
    result.push_back(executableDirectory / "Blocks");
    for (auto start : {executableDirectory, fs::absolute(workingDirectory)}) {
        for (auto directory = start; !directory.empty(); directory = directory.parent_path()) {
            result.push_back(directory / "BlockMaker" / "LithweftBlocks");
            if (directory == directory.parent_path()) break;
        }
    }
    return result;
}

inline fs::path find(const std::string& name, const fs::path& executable,
                     const fs::path& workingDirectory = fs::current_path()) {
    if (!validModuleName(name)) return {};
    for (const auto& directory : directories(executable, workingDirectory)) {
        const auto candidate = directory / (name + ".dylib");
        std::error_code error;
        if (fs::is_regular_file(candidate, error)) return fs::absolute(candidate);
    }
    return {};
}
}
