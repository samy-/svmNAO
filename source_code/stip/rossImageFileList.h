#ifndef ROSSIMAGEFILELIST_H
#define ROSSIMAGEFILELIST_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;


class imageFileList
{
private:
	vector<string> fileList;
	int currentIndex;
	
public:
	imageFileList(string listFileName);
	const char * getNextImageName();
	int remainingFrames();
};






#endif //ROSSIMAGEFILELIST_H
