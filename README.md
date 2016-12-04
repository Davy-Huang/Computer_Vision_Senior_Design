Computer_Vision_Senior_Design
The goal of the project is to create a method of opening doors for the handicap.
	The project will use computer vision to identify a doorknob and then use a robot arm to unlatch it.

	
This repository focuses primarily on creating a Haar classifier and testing the identification through a generic camera.
	As of 17 Nov 2016, the system has been trained to detected DoorKNOBS, DoorHANDLES
 	
vid_2_img is a program to convert video into individual frames
	This is utilized for faster acquisition of positive and negative images for training
	
The training folder contain the a basic directory structure to prepare the indexing of positive and negative images, and creation of a vector and actual training. 
	Please read Creating_a_Cascade_of_Haar-Like_Classifiers_Step_by_Step.pdf from the Department of Computer Science, the University of Auckland for a detailed explanation.
	cv_setup.txt contains the footnotes on the process.
		
main.cpp is a basic program to test object detection from the default camera on a laptop
	Open_cv 3.10 is utilized
	There three options currently. The .xml files are include
	"haarcascade_frontalface_alt.xml";	//	Faces
	"Door_handle_Cascade_v2.xml";		//	DOORHANDLE Option
	"DavyCascade4.xml";					//	DOORKNOB Option

	
	
