#include "opencv2/highgui/highgui.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <Windows.h>
// #include <direct.h>
using namespace std;					//	Helps with readability
using namespace cv;

int main(int argc, char* argv[]) {
	int x;

		cout << "Please enter the name of the video file: ";
		string file_name;
		getline(cin, file_name);

		cout << "Please enter a name for the folder: ";
		string folder_name;
		getline(cin, folder_name);

		cout << "'jpg' is best for Negetive sample data \n"
				"'bmp' is best for Postive sample data \n"
				"Enter the extenion: ";
		string ext;
		getline(cin, ext);


		VideoCapture cap(file_name);
		if (!cap.isOpened()) {
			cout << "error with opening" << endl;
			cin >> x;
			return -1;
		}
		
		double fps = cap.get(CV_CAP_PROP_FPS);
		namedWindow("Demo", CV_WINDOW_AUTOSIZE);
		
		
		
		int n = 0;
		CreateDirectory(folder_name.c_str(), NULL);
		//			mkdir(folder_name);

		char charCheckForEscKey = 0;
		while (charCheckForEscKey != 27) {
			Mat frame;
			if (!cap.read(frame)) {
				cout << "end of file reached" << endl;
				break;
			}

			imshow("Demo", frame);
			

			

			try{
			imwrite({ folder_name + "/" + file_name + '_' + to_string(n) + "." + ext }, frame);
			}
			catch (runtime_error& ex) {
				fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
				return 1;
			}



			n++;
			charCheckForEscKey = waitKey(1);

		}

		


	

	return 0;
}
