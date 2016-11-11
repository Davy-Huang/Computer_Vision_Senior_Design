/**********************************
Basic program to test xml files.
Default is faces.

**********************************/


//=================================
//	included dependencies
	//	opencv
#include<opencv2/core/core.hpp>
#include<opencv2/objdetect/objdetect.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
	//	standard
#include<iostream>
#include <stdio.h>


//=================================
//	namespaces for readabilty
using namespace std;
using namespace cv;


//=================================
// Global variables 
String face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
String window_name_orig = "Capture - Original";
String window_name_mod = "Capture - Modified";

//=================================
// main function
int main() {
	int error_pause;
	//	1. Setup camera
	VideoCapture capWebcam(0);							// declare a VideoCapture object and associate to webcam, 0 => use 1st webcam
	if (capWebcam.isOpened() == false)					// check if VideoCapture object was associated to webcam successfully
	{				
		cout << "error: capWebcam not accessed successfully"<<endl;
		cin >> error_pause;
		return(0);											// and exit program
	}

	//	2. setup Image data
	Mat imgOriginal;									// input image
	Mat frame_gray;

	//	3. Setup Cascades
	if (!face_cascade.load(face_cascade_name))			// Try to load the cascade and prints error if it fails
	{
		printf("--(!)Error loading face cascade\n"); 
		cin >> error_pause;
		return -1;
	};
	vector<Rect> faces;									//	vector for faceoutline
	

	//	4. While loops to get next picture from camera
	char charCheckForEscKey = 0;
	while (charCheckForEscKey != 27 && capWebcam.isOpened()) {			// Exits when the Esc key is pressed or webcam connection is lost
		bool blnFrameReadSuccessfully = capWebcam.read(imgOriginal);		// get next frame. Bool=1 when it read properly

		if (!blnFrameReadSuccessfully || imgOriginal.empty()) {				// if frame not read successfully
			cout << "error: frame not read from webcam\n";					// print error message to std out
			cin >> error_pause;
			break;																// and jump out of while loop
		}
		
		// 5. declare windows
		namedWindow(window_name_orig, CV_WINDOW_AUTOSIZE);			// note: you can use CV_WINDOW_NORMAL which allows resizing the window
		namedWindow(window_name_mod, CV_WINDOW_AUTOSIZE);			// note: you can use CV_WINDOW_NORMAL which allows resizing the window



		// 6. greyscale image and detect object
		cvtColor(imgOriginal, frame_gray, COLOR_BGR2GRAY);			// use defined condition to make grayscale
		equalizeHist(frame_gray, frame_gray);

		face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		for (size_t i = 0; i < faces.size(); i++)
		{
			Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
			ellipse(imgOriginal, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
			Mat faceROI = frame_gray(faces[i]);
		}



		imshow(window_name_orig, imgOriginal);						// Show what you got!
		imshow(window_name_mod, frame_gray);


		charCheckForEscKey = waitKey(1);							// delay (in ms) and get key press, if any cv::?
	}
	cout << "While loop exited" << endl;
	cin >> error_pause;
	return(0);
}
