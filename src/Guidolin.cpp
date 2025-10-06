#include "Guidolin.h"

/**
 * @file Guidolin.cpp
 * @author Federico Guidolin
 * @date 2025-09-18
 */

void getFrames(std::string& lblPath, std::vector<int>& frames)
{
    int pos = lblPath.find(".MOV");

    if(pos != std::string::npos)
    {
        lblPath.replace(pos, 14, "_frame/labels");
    }

    for(auto const& dir : std::filesystem::recursive_directory_iterator(lblPath))
    {
        if(!dir.path().empty())
        {
            std::string itemPath = dir.path().string();
            int first, last;
            first = itemPath.find_last_of("_") + 1;
            last = itemPath.find(".");
            frames.push_back(std::stoi(itemPath.substr(first, last-first)));
        }
    }
}
