#pragma once
#include <string>
#include <iterator>
#include <sstream>

extern class version {
public:
    version(std::string mj, std::string mn, std::string p):major(mj), minor(mn), patch(p) {
        longString = major + "." + minor + "." + patch;
    }
    std::string major;
    std::string minor;
    std::string patch;
    std::string longString;
    friend std::ostream&
        operator<<(std::ostream& os, const version& ver) {
        return os << ver.longString;
    }
};

bool isUpToDate(version v);