#pragma once
#include <iostream>
#include <iomanip>
#include <string>

static const std::string VERSION("1.0");

inline std::string print_version_info()
{
    std::cout
        << "id3-tags-cli " << "\n"
        << "License GPL-3.0: GNU GPL version 3 <https://www.gnu.org/licenses/>" << "\n"
        << "This is free software: you are free to change and redistribute it." << "\n"
        << "This program comes with ABSOLUTELY NO WARRANTY!" << "\n"
        << "\n"
        << "Written by Zereges <https://github.com/Zereges/id3-tag-cli>" << std::endl
        ;
    
    return VERSION;
}
