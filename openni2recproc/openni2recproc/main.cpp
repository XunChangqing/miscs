#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "OpenNI.h"
#include <unistd.h>

#include <iostream>
#include <iomanip>

using namespace cv;
using namespace std;
using namespace openni;

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

const char* keys =
{
    "{1 | | ./samplein | inputfile}"
    "{e | extract | false | extract every frame}"
    "{bs| bgstart | -1 | start frame of background}"
    "{bn| bgnum | -1 | number of background frmaes}"
};

int main( int argc, char* argv[] )
{
    cout<<"Hello from mythxcq!\n";
    string sInputOniFile;
    
    CommandLineParser cmdparser(argc, argv, keys);
    cmdparser.printParams();
    sInputOniFile = cmdparser.get<string>("1");
    
    int iBGFrameStart = -1;
    int iBGFrameNum = -1;
    iBGFrameStart = cmdparser.get<int>("bgstart");
    iBGFrameNum = cmdparser.get<int>("bgnum");
    
    bool bExtractAllFrame = false;
    bExtractAllFrame = cmdparser.get<bool>("extract");
    //cout<<sInputOniFile<<endl;
    //cout<<iBGFrameStart<<"\t"<<iBGFrameNum<<endl;
    //cout<<bExtractAllFrame<<endl;
    //return 0;
    
	Status rc = OpenNI::initialize();
	if (rc != STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
		return 1;
	}
    
	Device device;
    rc = device.open(sInputOniFile.c_str());
	if (rc != STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
		return 2;
	}
    rc = device.setDepthColorSyncEnabled(true);
    if(rc != STATUS_OK)
        printf("Failed to Set Depth Color Sync Enabled!\n");
    
    if(device.isFile())
    {
        PlaybackControl *pPlaybackCtrl = device.getPlaybackControl();
        pPlaybackCtrl->setRepeatEnabled(false);
        //pPlaybackCtrl->setSpeed(0);
    }
    
    string sSamplePath;
    sSamplePath = sInputOniFile.substr(0, sInputOniFile.rfind(".oni"));
    if(bExtractAllFrame)
        sSamplePath += "all";
    
    mkpath_np(sSamplePath.c_str(),0777);
    
	VideoStream depth, color;
    
	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, SENSOR_DEPTH);
		if (rc == STATUS_OK)
		{
			rc = depth.start();
			if (rc != STATUS_OK)
			{
				printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
			}
		}
		else
		{
			printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
		}
	}
    
	if (device.getSensorInfo(SENSOR_COLOR) != NULL)
	{
		rc = color.create(device, SENSOR_COLOR);
		if (rc == STATUS_OK)
		{
			rc = color.start();
			if (rc != STATUS_OK)
			{
				printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
			}
		}
		else
		{
			printf("Couldn't create color stream\n%s\n", OpenNI::getExtendedError());
		}
	}
    
	VideoFrameRef ColorFrame,DepthFrame;
    
	VideoStream* streams[] = {&depth, &color};
    VideoMode DepthVM = depth.getVideoMode();
    VideoMode ColorVM = color.getVideoMode();
	
    Mat DepthMat(DepthVM.getResolutionY(),DepthVM.getResolutionX(),CV_16UC1);
    int iDepthFPS = DepthVM.getFps();
    Mat ColorMat(ColorVM.getResolutionY(),ColorVM.getResolutionX(),CV_8UC3);
    Mat ColorGrayMat(ColorVM.getResolutionY(),ColorVM.getResolutionX(),CV_8UC1);
    int iColorFPS = ColorVM.getFps();
    int iDepthFrameCount = 0;
    int iColorFrameCount = 0;
    bool ret;
    bool bRecBG = false;
    int iBGFrameCount = 0;
	while (1)
	{
		int readyStream = -1;
		rc = OpenNI::waitForAnyStream(streams, 2, &readyStream, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
			break;
		}
        if(iBGFrameStart>=0)
        {
            if (iDepthFrameCount>=iBGFrameStart && iDepthFrameCount<=iBGFrameStart+iBGFrameNum)
                bRecBG = true;
            else
                bRecBG = false;
        }
        stringstream DepthName;
        string strDepthName;
        stringstream ColorName;
        string strColorName;
        stringstream DepthBGName;
        string strDepthBGName;
        stringstream FrameCountStr;
        string strFrameCount;
		switch (readyStream)
		{
            case 0:
                // Depth
                depth.readFrame(&DepthFrame);
                memcpy(DepthMat.data, DepthFrame.getData(), DepthFrame.getDataSize());
                if(bExtractAllFrame)
                {
                    DepthName<<setfill('0')<<setw(5)<<iDepthFrameCount<<"d.png";
                    strDepthName = sSamplePath+"/"+DepthName.str();
                    ret = imwrite(strDepthName, DepthMat);
                }
                else if(iDepthFrameCount%iDepthFPS == 0)
                {
                    DepthName<<setfill('0')<<setw(5)<<iDepthFrameCount/iDepthFPS<<"d.png";
                    strDepthName = sSamplePath+"/"+DepthName.str();
                    ret = imwrite(strDepthName, DepthMat);
                }
                if(bRecBG && !bExtractAllFrame)
                {
                    DepthBGName<<setfill('0')<<setw(5)<<iBGFrameCount++<<"dbg.png";
                    strDepthBGName = sSamplePath+"/"+DepthBGName.str();
                    ret = imwrite(strDepthBGName, DepthMat);
                }
                imshow("depth",DepthMat);
                iDepthFrameCount++;
                //cout<<"Depth Frame Count: "<<iDepthFrameCount<<"\tFPS: "<<iDepthFPS<<endl;
                break;
            case 1:
                // Color
                color.readFrame(&ColorFrame);
                memcpy(ColorMat.data, ColorFrame.getData(), ColorFrame.getDataSize());
                cvtColor(ColorMat, ColorGrayMat, CV_RGB2GRAY);
                if(bExtractAllFrame)
                {
                    ColorName<<setfill('0')<<setw(5)<<iColorFrameCount<<"c.png";
                    strColorName = sSamplePath+"/"+ColorName.str();
                    ret = imwrite(strColorName, ColorGrayMat);
                }
                else if(iColorFrameCount%iColorFPS == 0)
                {
                    ColorName<<setfill('0')<<setw(5)<<iColorFrameCount/iColorFPS<<"c.png";
                    strColorName = sSamplePath+"/"+ColorName.str();
                    ret = imwrite(strColorName, ColorGrayMat);
                }
                if(bRecBG && !bExtractAllFrame)
                    putText(ColorMat, "BG Recording", Point(50,20), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255));
                FrameCountStr<<iColorFrameCount;
                putText(ColorMat, FrameCountStr.str(), Point(5,20), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255));
                imshow("color",ColorMat);
                //cout<<"Color Frame Count: "<<iColorFrameCount<<"\tFPS: "<<iColorFPS<<endl;
                iColorFrameCount++;
                break;
            default:
                printf("Unxpected stream\n");
		}
        
        int key = waitKey(1);
        if(key == 32)
        {
            cout<<"Space pressed!\n";
            bRecBG = !bRecBG;
        }
	}
    
	depth.stop();
	color.stop();
	depth.destroy();
	color.destroy();
	device.close();
	OpenNI::shutdown();
    
    return 0;
}
