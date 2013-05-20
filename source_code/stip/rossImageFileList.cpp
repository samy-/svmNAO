#include "rossImageFileList.h"

imageFileList::imageFileList(string listFileName)
{
	string line;
	ifstream myfile(listFileName.c_str());
	currentIndex=0;
	
	if (myfile.is_open())
	{
		while (! myfile.eof())
		{
			getline(myfile,line);
			string newString(line);
			fileList.push_back(newString);
		}
		myfile.close();
	}
	
}

const char * imageFileList::getNextImageName()
{
	return fileList.at(currentIndex++).c_str();
	
}

int imageFileList::remainingFrames()
{
	return (fileList.size()-currentIndex)-1;
}

