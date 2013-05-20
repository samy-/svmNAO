#ifndef STIP_STIP_H
#define STIP_STIP_H

#include <string>
#include <iostream>
#include "pcheader.h"
#include <cstdio>
#include <cassert>
#include "msharrisbuffer.h"
#include "cmdline.h"


#ifdef USE_CVCAM
#include "cvcam.h"
#endif

#include <boost/shared_ptr.hpp>
#include <alcommon/almodule.h>
#include <string>

#include <stdio.h>
#include <math.h>
#include <string.h>   

#include "stbuffer.h"
#include <alproxies/almemoryproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/alvideorecorderproxy.h>
#include <althread/almutex.h>

namespace AL
{
  class ALBroker;
}

class Stip : public AL::ALModule
{
  public:

    Stip(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);
    void init();
    void STIPcallback();
    void ConvertRealImage(IplImage* im,IplImage* gray8u,IplImage* rgb8u);
    void CapProperties( CvCapture* capture);
    void dostuff(IplImage *frm);
    void dovisstuff();
    bool InitCapture(const char* param);

    virtual ~Stip();
private:
AL::ALMemoryProxy fMemoryProxy;
std::string outfile;
std::string infile;
std::string sourcename;
CCmdLine cmdLine;
bool show;
bool Processing;
IplImage* frame;
HarrisBuffer hb;
MultiScaleHarrisBuffer mshb;
CvCapture* capture;
char* win1;
char* win2;
char* wincvcam;
IplImage* vis;
IplImage* vis2;
IplImage* vis3;
IplImage* camimg;
IplImage* gray;
IplImage* grayvis;
double avg;
int ifr;
int TotalIPs;
int nIPs;
int frame_begin;
int frame_end;
bool first;

int resid;
  };



#endif
