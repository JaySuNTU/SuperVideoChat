// SuperVideoChat.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "pch.h"
#include <iostream>
// opencv include
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect.hpp>

using namespace cv;
using namespace std;

Rect movingscreen(Mat inputframe, Rect ori_rect, Rect next_rect);

// setting Mat
Mat frame, image, cropimage;
// setting feature points
Point center_eye, center_smile, center_face;
Rect ROI_rect, ROI_rect_old;

int main(int argc, const char** argv)
{
	// setting cascade
	string cascadeFaceName;
	string cascadeEyeName;
	string cascadeSmileName;
	cascadeFaceName = "C://opencv//sources//data//haarcascades//haarcascade_frontalface_alt2.xml";
	cascadeEyeName = "C://opencv//sources//data//haarcascades//haarcascade_eye.xml";
	cascadeSmileName = "C://opencv//sources//data//haarcascades//haarcascade_smile.xml";
	CascadeClassifier cascadeFace = cascadeFaceName;
	CascadeClassifier cascadeEye = cascadeEyeName;
	CascadeClassifier cascadeSmile = cascadeSmileName;
	// setting rect for detecting goal
	vector<Rect> eyes;
	vector<Rect> smiles;
	vector<Rect> faces;
	// setting video capture
	VideoCapture capture_video(0);
	//capture_video.open;
	
	// start
	int timemark = 0;
	for (;;)
	{
		capture_video >> frame;		
		if (frame.empty()) cout << "error: camera has problem!" << endl;
		else
		{
			// image process
			flip(frame, frame, 1);
			resize(frame, frame, Size(640, 480));
			cvtColor(frame, image, COLOR_BGR2GRAY);
			equalizeHist(image, image);
			if (cropimage.empty()) cropimage = frame;
			if (timemark==0)
			{				
				// detect
				cascadeFace.detectMultiScale(image, faces, 1.2, 2, 0 || CASCADE_SCALE_IMAGE, Size(150, 150));
				if (faces.size() == 0)
					putText(frame, "There no faces in the screen!", Point(0, 40), 2, 0.4, Scalar(0, 255, 0), 1, LINE_AA);
				
				for (size_t k = 0; k < faces.size(); k++)
				{
					int component_x = faces[k].x + int(faces[k].width*0.5);
					int component_y = faces[k].y + int(faces[k].height*0.5);
					center_face = Point(component_x, component_y);

					//Mat crop(image, faces[k]);
					//cascadeEye.detectMultiScale(crop, eyes, 1.1, 3, CASCADE_FIND_BIGGEST_OBJECT, Size(10, 10), Size(100, 100));
					//cascadeSmile.detectMultiScale(crop, smiles, 1.1, 3, CASCADE_FIND_BIGGEST_OBJECT, Size(10, 10), Size(300, 300));

					//circle(frame, center_face, 5, Scalar(0, 255, 255), -1);
					int rect_R = int((faces[k].width + faces[k].height) / 4) + 50;
					ROI_rect = Rect(component_x - rect_R, component_y - rect_R, rect_R * 2, rect_R * 2);
					rectangle(frame, faces[k], Scalar(0, 255, 255), 2);
					
					break;
				}
				timemark = 15;
			}
			timemark--;
			/*
			if (ROI_rect.x <= 0 || ROI_rect.y <= 0 ) ROI_rect = ROI_rect_old; 
			if (ROI_rect.x + ROI_rect.width >= 640 || ROI_rect.y + ROI_rect.height >= 480) ROI_rect = ROI_rect_old;
			Mat ROI(frame, ROI_rect);
			ROI.copyTo(cropimage);
			ROI_rect_old = ROI_rect;
			*/
			ROI_rect_old = movingscreen(frame, ROI_rect_old, ROI_rect);
			if (waitKey(33) > 0) break;
		}		
	}
	//capture_video.release();
	return -1;
}

Rect movingscreen(Mat inputframe, Rect ori_rect, Rect next_rect)
{

	if (next_rect.x <= 0 || next_rect.y <= 0) next_rect = ori_rect;
	if (next_rect.x + next_rect.width >= 640 || next_rect.y + next_rect.height >= 480) next_rect = ori_rect;
	double distance = sqrt(pow(next_rect.x - ori_rect.x, 2) + pow(next_rect.y - ori_rect.y, 2));
	if (distance < 20) next_rect = ori_rect;
	cout << distance << endl;
	Mat cropframe;
	Mat ROIframe(inputframe, next_rect);
	ROIframe.copyTo(cropframe);
	if (cropframe.rows == 0 || cropframe.cols == 0) {}
	else
	{
		resize(cropframe, cropframe, Size(600, 600), 600 / cropframe.cols, 600 / cropframe.rows);
		resize(inputframe, inputframe, Size(inputframe.cols*2, inputframe.rows*2),2,2);
	}		
	if (!inputframe.empty()) cv::imshow("frame", inputframe);
	if (!cropframe.empty()) cv::imshow("cropframe", cropframe);	
	return next_rect;
};


/*
		videoheight = to_string(int(cap.get(CAP_PROP_FRAME_WIDTH)));
		videowidth = to_string(int(cap.get(CAP_PROP_FRAME_HEIGHT)));
		videoframerate = to_string(int(cap.get(CAP_PROP_FPS)));
		putText(frame, "Height:" + videoheight, Point(10, 20), 0, 0.6, Scalar(0, 0, 255));
		putText(frame, "Width:" + videowidth, Point(10, 20 + 1 * 25), 0, 0.6, Scalar(0, 0, 255));
		putText(frame, "Framerate:" + videoframerate, Point(10, 20 + 2 * 25), 0, 0.6, Scalar(0, 0, 255));
*/