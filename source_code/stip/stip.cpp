#include "stip.h"
#include "pcheader.h"
#include <cstdio>
#include <cassert>
#include "cvutil.h"
#include "harrisbuffer.h"
#include "msharrisbuffer.h"
#include "cmdline.h"
#include <kmeans/kmeans.hpp>
 

#ifdef USE_CVCAM
#include "cvcam.h"
#endif 

#include <boost/shared_ptr.hpp>
#include <alcommon/almodule.h>
#include <string>
#include <alvalue/alvalue.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>
#include <qi/log.hpp>
#include <string>

Stip::Stip(boost::shared_ptr<AL::ALBroker> broker,const std::string& name):
    AL::ALModule(broker, name)
{

  setModuleDescription("This module extract space-time interest points");
  functionName("STIPcallback", getName(), "");
  BIND_METHOD(Stip::STIPcallback);

//  functionName("Lib_1generate_model",getName(),"cmt");
//  BIND_METHOD(Lib_1::Lib_1generate_model);//

//  functionName(Lib_1svm_testing,getName(),"cmt");
//  BIND_METHOD(Lib_1::Lib_1svm_testing);//
}
void Stip::init(){
show=false;
Processing=false;
frame = 0;
capture = 0;
win1="ST-Demo";
win2="Win2";
wincvcam="cvcam";
vis  = NULL;
vis2 = NULL;
vis3 = NULL;
camimg = NULL;
gray = NULL;
grayvis = NULL;
avg=0;
ifr=0;
TotalIPs=0;
nIPs=0;
frame_begin=0;
frame_end=100000000;
first=true;
const int camresx[]= {80,160,320,400,640,1280,1920};
const int camresy[]= {60,120,240,300,480,720,1080};
resid=5;


    try{

        fMemoryProxy.subscribeToEvent("STIPEvent", "Stip","STIPcallback");
    }
    catch (const AL::ALError& e) {
       qiLogError("module.example") << e.what() << std::endl;
     }

}

void Stip::ConvertRealImage(IplImage* im,IplImage* gray8u,IplImage* rgb8u)
{
	cvNormalize(im,im,1,0,CV_MINMAX); 
	cvScale(im,gray8u,255,0);
	cvCvtColor(gray8u,rgb8u,CV_GRAY2BGR);
}

void Stip::CapProperties( CvCapture* capture)
{
	int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double fps    =  cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int numFrames = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);
	double curPos = cvGetCaptureProperty(capture,  CV_CAP_PROP_POS_MSEC);
	printf("%.1f  ",curPos);
	printf("%d frames ",numFrames);
	printf("[%dx%d]", frameW, frameH); 
	printf(" fps=%.1f  ",fps);
	printf("\n");
}



void Stip::dostuff(IplImage *frm)
{
    frame=frm;

#ifdef WIN32
    cvFlip(frame,NULL,0);
#endif

	if(first)
	{	
		first=false;
		
		if(!hb.Init(frm,outfile))
            qiLogInfo("STIP") << "hb.Init failed!!!!\n" << std::endl;

		if(!mshb.Init(frm,outfile,sourcename,frame_begin))
             qiLogInfo("STIP") << "mshb.Init failed!!!!\n" << std::endl;

		gray = cvCreateImage(cvGetSize(frm), IPL_DEPTH_8U, 1);
	}
	if (frm->nChannels==1) cvCopy(frm,gray);

	else cvCvtColor(frm,gray,CV_RGB2GRAY);
	double t,ft;	
	t = (double)cvGetTickCount();Processing=true;
	mshb.ProcessFrame(gray);
    	t = (double)cvGetTickCount() - t;Processing=false;
	ifr++;
	ft=t/(cvGetTickFrequency()*1000.);
	avg=((ifr-1)* avg + ft)/ifr;
	nIPs=mshb.NumberOfDetectedIPs();TotalIPs+=nIPs;
	if (ifr%20==0){
		printf("Frame: %5d - ",ifr);
		printf("IPs[this:%2d, total:%4d] ",nIPs,TotalIPs);
		printf(" - Perf: Avg FPS=%.1f ",1000/avg);
		printf("\n");
	}
}



void Stip::dovisstuff()
{
	if(!grayvis)  grayvis  = cvCreateImage(cvGetSize(mshb.getMSHBufferImage(0)), IPL_DEPTH_8U, 1);
	if(!vis)  vis  = cvCreateImage(cvGetSize(mshb.getMSHBufferImage(0)), IPL_DEPTH_8U, 3);
	if(!vis2) vis2 = cvCreateImage(cvGetSize(mshb.getMSHBufferImage(0)), IPL_DEPTH_8U, 3);
	if(!vis3) vis3 = cvCreateImage(cvGetSize(mshb.getMSHBufferImage(0)), IPL_DEPTH_8U, 3);
	ConvertRealImage(mshb.getMSHBufferImage(0),grayvis,vis3);
	mshb.DrawInterestPoints(vis3);
	cvShowImage(win2, vis3 );
}


bool Stip::InitCapture(const char* param)
{
   capture = cvCaptureFromFile( param ); 

    if( !capture )
    {
        fprintf(stderr,"Could not initialize capturing from %s...\n",param);
        return false;
    }

	if (frame_begin>0)
	  if (!cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,frame_begin))
	  {
	    printf("Could not start capture from frame %d in %s\n",frame_begin,param);
	    printf(" -> bugfix: fast-forward to the frame %d ...\n",frame_begin);
	    if(capture){
	      int fn=0;
	      while (fn<frame_begin){
		frame = cvQueryFrame( capture );
		if( !frame )
		  break;
		fn++;
	      }
	    }
	    printf("    done.\n");
	  }
	 
	return true;
    //faut faire un raise event Kmeans
}


Stip::~Stip() {

    fMemoryProxy.unsubscribeToEvent("STIPEvent", "Stip");
    qiLogInfo("unsubscribeToEvent") << "unsubscribeToEvent: OK" << std::endl;
}

void Stip::STIPcallback(){
    infile="/home/nao/recordings/cameras/myvideo.avi";
    outfile="/home/nao/data/activities_recognition/stip";
    bool cvcam=false;
    mshb.framemax = 100000000;
    try{
    qiLogInfo("Stip") << "Stip:called" << std::endl;
    AL::ALTextToSpeechProxy fTtsProxy = AL::ALTextToSpeechProxy(getParentBroker());
    fTtsProxy.setLanguage("English");
    fTtsProxy.say("Stip is called");

 	if(!InitCapture(infile.c_str()))
			qiLogInfo("STIP") << "InitCapture :error" << std::endl;
	
	sourcename=infile;
	if(capture)
	{
    int fn=0;
    for(;;)
    {
        if (fn>=mshb.framemax) break;
        if (fn>=frame_end-frame_begin) break;

        fn++;
		frame = cvQueryFrame( capture );
			
        if( !frame )
            break;


		dostuff(frame);
		if(show)
		{
			dovisstuff();
			cvWaitKey(10);
		}  
    }
	}
	mshb.finishProcessing();
	qiLogInfo("STIP") << "STIP executed ok\n" << std::endl;
    	if(capture)
		cvReleaseCapture( &capture );

        //evoi de signal a Kmeans
    qiLogInfo("STIP") << "[stip]:appel de KMeans" << std::endl;
    nao_kmeans();
    qiLogInfo("STIP") << "[stip]:KMeans Ok" << std::endl;
    //evoi de signal a SVM

    fMemoryProxy.raiseEvent("SVMEvent",1);
    qiLogInfo("STIP") << "[stip]:Envoi du signal SVMEvent" << std::endl;
    }
    catch (const AL::ALError& e) {
     }




}
         
