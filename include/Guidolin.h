#ifndef GUIDOLIN_H
#define GUIDOLIN_H

/**
 * @file Guidolin.h
 * @author Federico Guidolin
 * @date 2025-09-18
 */

#include <string>
#include <vector>
#include <filesystem>

//It recives as input the name of the .MOV file and turns it into the path to the corresponding labls directory, then foe each frame
//in the directory the function load the frame number into the vector frames
/**
 * It recives as input the name of the .MOV file and turns it into the path to the corresponding labls directory, then foe each frame
 * in the directory the function load the frame number into the vector frames
 * @param lblPath the path of the file that will be converted to the path to the labels directory
 * @param frames vector to contain the numbers of frames in the labes directory
  */
void getFrames(std::string& lblPath, std::vector<int>& frames);

#endif
