// k2cv.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <iomanip>
using namespace std;
using namespace cv;

Scalar gInferedBoneColor(0,0,255);
Scalar gTrackedBoneColor(255,0,0);
int gInferedBoneThickness = 2;
int gTrackedBoneThickness = 2;

Scalar gInferedJointColor(0,255,255);
Scalar gTrackedJointColor(255,255,0);
int gJointThickness = 3;

Scalar gHandOpenColor(255,0,0,100);
Scalar gHandClosedColor(0,255,0,100);
Scalar gHandLassoColor(0,0,255,100);
int gHandSize = 15;

Scalar gRedColor(0,0,255);

Point BodyToScreen(ICoordinateMapper* pCoordinateMapper, const CameraSpacePoint& bodyPoint)
{
    // Calculate the body's position on the screen
    DepthSpacePoint depthPoint = {0};
    pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);
    //float screenPointX = static_cast<float>(depthPoint.X * width) / cDepthWidth;
    //float screenPointY = static_cast<float>(depthPoint.Y * height) / cDepthHeight;
	return Point(depthPoint.X, depthPoint.Y);
}

void DrawHand(HandState handState, const Point& handPosition, Mat &img)
{
    switch (handState)
    {
        case HandState_Closed:
			circle(img, handPosition, gHandSize, gHandClosedColor, -1);
            break;

        case HandState_Open:
			circle(img, handPosition, gHandSize, gHandOpenColor, -1);
            break;

        case HandState_Lasso:
			circle(img, handPosition, gHandSize, gHandLassoColor, -1);
            break;
    }
}

void DrawBone(const Joint* pJoints, const Point* pJointPoints, JointType joint0, JointType joint1, Mat &img)
{
    TrackingState joint0State = pJoints[joint0].TrackingState;
    TrackingState joint1State = pJoints[joint1].TrackingState;

    // If we can't find either of these joints, exit
    if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
    {
        return;
    }

    // Don't draw if both points are inferred
    if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
    {
        return;
    }

    // We assume all drawn bones are inferred unless BOTH joints are tracked
    if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
    {
		line(img, pJointPoints[joint0], pJointPoints[joint1], gTrackedBoneColor, gTrackedBoneThickness);
        //m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneTracked, c_TrackedBoneThickness);
    }
    else
    {
		line(img, pJointPoints[joint0], pJointPoints[joint1], gInferedBoneColor, gInferedBoneThickness);
        //m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneInferred, c_InferredBoneThickness);
    }
}

void DrawBody(const Joint* pJoints, const Point * pJointPoints, Mat &img)
{
    // Draw the bones

    // Torso
    DrawBone(pJoints, pJointPoints, JointType_Head, JointType_Neck,img);
    DrawBone(pJoints, pJointPoints, JointType_Neck, JointType_SpineShoulder,img);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_SpineMid,img);
    DrawBone(pJoints, pJointPoints, JointType_SpineMid, JointType_SpineBase,img);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderRight,img);
    DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderLeft,img);
    DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipRight,img);
    DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipLeft,img);
    
    // Right Arm    
    DrawBone(pJoints, pJointPoints, JointType_ShoulderRight, JointType_ElbowRight,img);
    DrawBone(pJoints, pJointPoints, JointType_ElbowRight, JointType_WristRight,img);
    DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_HandRight,img);
    DrawBone(pJoints, pJointPoints, JointType_HandRight, JointType_HandTipRight,img);
    DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_ThumbRight,img);

    // Left Arm
    DrawBone(pJoints, pJointPoints, JointType_ShoulderLeft, JointType_ElbowLeft,img);
    DrawBone(pJoints, pJointPoints, JointType_ElbowLeft, JointType_WristLeft,img);
    DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_HandLeft,img);
    DrawBone(pJoints, pJointPoints, JointType_HandLeft, JointType_HandTipLeft,img);
    DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_ThumbLeft,img);

    // Right Leg
    DrawBone(pJoints, pJointPoints, JointType_HipRight, JointType_KneeRight,img);
    DrawBone(pJoints, pJointPoints, JointType_KneeRight, JointType_AnkleRight,img);
    DrawBone(pJoints, pJointPoints, JointType_AnkleRight, JointType_FootRight,img);

    // Left Leg
    DrawBone(pJoints, pJointPoints, JointType_HipLeft, JointType_KneeLeft,img);
    DrawBone(pJoints, pJointPoints, JointType_KneeLeft, JointType_AnkleLeft,img);
    DrawBone(pJoints, pJointPoints, JointType_AnkleLeft, JointType_FootLeft,img);

    // Draw the joints
    for (int i = 0; i < JointType_Count; ++i)
    {
        if (pJoints[i].TrackingState == TrackingState_Inferred)
        {
			circle(img, pJointPoints[i], 2, gTrackedJointColor, -1);
            //m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointInferred);
        }
        else if (pJoints[i].TrackingState == TrackingState_Tracked)
        {
			circle(img, pJointPoints[i], 2, gInferedJointColor, -1);
            //m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointTracked);
        }
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
	IKinectSensor *pKinectSensor;
	IMultiSourceFrameReader *pMultiSourceFrameReader;
	HRESULT hr;
	hr = GetDefaultKinectSensor(&pKinectSensor);
	if(FAILED(hr))
		return -1;

	hr = pKinectSensor->Open();
	if(FAILED(hr))
		return -1;

	ICoordinateMapper *pCoordinateMapper;
	hr = pKinectSensor->get_CoordinateMapper(&pCoordinateMapper);
	if(FAILED(hr))
		return -1;

	//hr = pKinectSensor->OpenMultiSourceFrameReader(
	//	FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_BodyIndex | FrameSourceTypes::FrameSourceTypes_Body |
	//	FrameSourceTypes::FrameSourceTypes_Infrared,
	//	&pMultiSourceFrameReader);
	hr = pKinectSensor->OpenMultiSourceFrameReader(
		FrameSourceTypes::FrameSourceTypes_Body | FrameSourceTypes::FrameSourceTypes_BodyIndex | FrameSourceTypes::FrameSourceTypes_Depth,
		&pMultiSourceFrameReader);

	if(FAILED(hr))
		return -1;

	while(1)
	{
		
		IMultiSourceFrame* pMultiSourceFrame = NULL;
		IDepthFrame* pDepthFrame = NULL;
		IColorFrame* pColorFrame = NULL;
		ILongExposureInfraredFrame* pLInfraredFrame = NULL;
		IInfraredFrame* pInfraredFrame = NULL;
		IBodyFrame* pBodyFrame = NULL;
		IBodyIndexFrame* pBodyIndexFrame = NULL;

		HRESULT hr = pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);

		if (SUCCEEDED(hr))
		{
			IDepthFrameReference* pDepthFrameReference = NULL;

			hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
			if (SUCCEEDED(hr))
			{
				hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
			}

			SafeRelease(pDepthFrameReference);
		}

		//if (SUCCEEDED(hr))
		//{
		//	IColorFrameReference* pColorFrameReference = NULL;

		//	hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		//	if (SUCCEEDED(hr))
		//	{
		//		hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		//	}

		//	SafeRelease(pColorFrameReference);
		//}

		if (SUCCEEDED(hr))
		{
			IBodyIndexFrameReference* pBodyIndexFrameReference = NULL;
			hr = pMultiSourceFrame->get_BodyIndexFrameReference(&pBodyIndexFrameReference);
			if (SUCCEEDED(hr))
			{
				hr = pBodyIndexFrameReference->AcquireFrame(&pBodyIndexFrame);
			}
			SafeRelease(pBodyIndexFrameReference);
		}

		if(SUCCEEDED(hr))
		{
			IBodyFrameReference* pBodyFrameReference = NULL;

			hr = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameReference);
			if(SUCCEEDED(hr))
				hr = pBodyFrameReference->AcquireFrame(&pBodyFrame);
		}


		if (SUCCEEDED(hr))
		{
			Mat DepthFrameMat,BodyIndexFrameMat,ColorFrameMat;
			IBody* ppBodies[BODY_COUNT] = {0};
			Vector4 floorClipPlane;

			INT64 nDepthTime = 0;
			IFrameDescription* pDepthFrameDescription = NULL;
			int nDepthWidth = 0;
			int nDepthHeight = 0;
			UINT nDepthBufferSize = 0;
			UINT16 *pDepthBuffer = NULL;

			IFrameDescription* pColorFrameDescription = NULL;
			int nColorWidth = 0;
			int nColorHeight = 0;
			ColorImageFormat imageFormat = ColorImageFormat_None;
			UINT nColorBufferSize = 0;
			RGBQUAD *pColorBuffer = NULL;

			IFrameDescription* pBodyIndexFrameDescription = NULL;
			int nBodyIndexWidth = 0;
			int nBodyIndexHeight = 0;
			UINT nBodyIndexBufferSize = 0;
			BYTE *pBodyIndexBuffer = NULL;

			// get depth frame data
			if(pDepthFrame != NULL)
			{
				hr = pDepthFrame->get_RelativeTime(&nDepthTime);
				if (SUCCEEDED(hr))
					hr = pDepthFrame->get_FrameDescription(&pDepthFrameDescription);
				if (SUCCEEDED(hr))
					hr = pDepthFrameDescription->get_Width(&nDepthWidth);
				if (SUCCEEDED(hr))
					hr = pDepthFrameDescription->get_Height(&nDepthHeight);
				if (SUCCEEDED(hr))
					hr = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);            
				DepthFrameMat = Mat(nDepthHeight, nDepthWidth, CV_16U, pDepthBuffer);
			}

			// get color frame data
			if(pColorFrame != NULL)
			{
				if (SUCCEEDED(hr))
					hr = pColorFrame->get_FrameDescription(&pColorFrameDescription);
				if (SUCCEEDED(hr))
					hr = pColorFrameDescription->get_Width(&nColorWidth);
				if (SUCCEEDED(hr))
					hr = pColorFrameDescription->get_Height(&nColorHeight);
				if (SUCCEEDED(hr))
					hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
				if (SUCCEEDED(hr))
				{
					if (imageFormat == ColorImageFormat_Bgra)
					{
						hr = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));
					}
					//else if (m_pColorRGBX)
					//{
					//    pColorBuffer = m_pColorRGBX;
					//    nColorBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
					//    hr = pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, reinterpret_cast<BYTE*>(pColorBuffer), ColorImageFormat_Bgra);
					//}
					else
					{
						hr = E_FAIL;
					}
				}
			}

			// get body index frame data
			if(pBodyIndexFrame!=NULL)
			{
				if (SUCCEEDED(hr))
					hr = pBodyIndexFrame->get_FrameDescription(&pBodyIndexFrameDescription);
				if (SUCCEEDED(hr))
					hr = pBodyIndexFrameDescription->get_Width(&nBodyIndexWidth);
				if (SUCCEEDED(hr))
					hr = pBodyIndexFrameDescription->get_Height(&nBodyIndexHeight);
				if (SUCCEEDED(hr))
					hr = pBodyIndexFrame->AccessUnderlyingBuffer(&nBodyIndexBufferSize, &pBodyIndexBuffer);            
				BodyIndexFrameMat = Mat(nBodyIndexHeight, nBodyIndexWidth, CV_8U, pBodyIndexBuffer);
			}

			if(pBodyFrame)
			{
				hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
				pBodyFrame->get_FloorClipPlane(&floorClipPlane);
			}

			const float scaleFactor = 0.05f;
			Mat DepthGrayShow;
			DepthFrameMat.convertTo( DepthGrayShow, CV_8UC1, scaleFactor );
			Mat tmparr[] = {DepthGrayShow, DepthGrayShow, DepthGrayShow};
			Mat DepthRGBShow;
			merge(tmparr, 3, DepthRGBShow);

			Mat RadarShow(600, 700, CV_8UC3);
			circle(RadarShow, Point(RadarShow.size().width*0.5,0), 5, gRedColor, -1);
			for(int i=0;i<_countof(ppBodies);++i)
			{
				IBody* pBody = ppBodies[i];
				if (pBody)
				{
					UINT64 tid = 0;
					pBody->get_TrackingId(&tid);
					BOOLEAN bTracked = false;
					hr = pBody->get_IsTracked(&bTracked);

					if (SUCCEEDED(hr) && bTracked)
					{
						Joint joints[JointType_Count]; 
						hr = pBody->GetJoints(_countof(joints), joints);

						if (SUCCEEDED(hr))
						{                        
							Point jointPoints[_countof(joints)];
							for(int j=0;j<_countof(joints);j++)
								jointPoints[j] = BodyToScreen(pCoordinateMapper, joints[j].Position);

							DrawBody(joints, jointPoints, DepthRGBShow);

							CameraSpacePoint headPos = joints[JointType_Head].Position;
							CameraSpacePoint neckPos = joints[JointType_Neck].Position;
							float height = headPos.X*floorClipPlane.x+headPos.Y*floorClipPlane.y+headPos.Z*floorClipPlane.z+floorClipPlane.w;
							float neckHeight = neckPos.X*floorClipPlane.x+neckPos.Y*floorClipPlane.y+neckPos.Z*floorClipPlane.z+floorClipPlane.w;
							height = height + height - neckHeight;

							//HandState leftHandState = HandState_Unknown;
							//HandState rightHandState = HandState_Unknown;
							//pBody->get_HandLeftState(&leftHandState);
							//pBody->get_HandRightState(&rightHandState);
							//DrawHand(leftHandState, jointPoints[JointType_HandLeft], DepthRGBShow);
							//DrawHand(rightHandState, jointPoints[JointType_HandRight], DepthRGBShow);
							CameraSpacePoint SpinePos = joints[JointType_SpineMid].Position;
							Point hipScreenPos = BodyToScreen(pCoordinateMapper, SpinePos);
							stringstream targetSStream;
							targetSStream<<i<<"("<<setprecision(3)<<height<<")";
							string targetInfoStr = targetSStream.str();
							//center the text
							Size textSize = getTextSize(targetInfoStr, FONT_HERSHEY_SIMPLEX, 0.5, 1, NULL);
							Point textPos(hipScreenPos.x-textSize.width*0.5, hipScreenPos.y);
							putText(DepthRGBShow, targetInfoStr, textPos, FONT_HERSHEY_SIMPLEX, 0.5, gRedColor);

							Point targetRadarPos = Point(SpinePos.X*100+RadarShow.size().width*0.5, SpinePos.Z*100);
							circle(RadarShow, targetRadarPos, 5, gRedColor, -1);
							line(RadarShow, targetRadarPos, Point(RadarShow.size().width*0.5,0), gRedColor, 1);
						}
					}
				}
			}
			//putText(DepthRGBShow,"yes", cvPoint(200,200),FONT_HERSHEY_SIMPLEX,2,cvScalar(255,0,0));

			imshow("show",DepthRGBShow);
			imshow("radar", RadarShow);

			for (int i = 0; i < _countof(ppBodies); ++i)
				SafeRelease(ppBodies[i]);
			SafeRelease(pDepthFrameDescription);
			SafeRelease(pColorFrameDescription);
			SafeRelease(pBodyIndexFrameDescription);
		}

		SafeRelease(pDepthFrame);
		SafeRelease(pColorFrame);
		SafeRelease(pBodyIndexFrame);
		SafeRelease(pMultiSourceFrame);
		SafeRelease(pBodyFrame);
		SafeRelease(pInfraredFrame);
		SafeRelease(pLInfraredFrame);

		int retKey = waitKey(1);
	}
	return 0;
}

