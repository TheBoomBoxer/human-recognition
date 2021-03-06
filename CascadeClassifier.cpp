#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <cstdio>
#include <iostream>

using namespace std;
using namespace cv;

void detectAndDisplay(Mat frame);

String face_cascade_name, eyes_cascade_name, upperbody_cascade_name, profileface_cascade_name, lowerbody_cascade_name;
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
CascadeClassifier upperbody_cascade;
CascadeClassifier profile_face_cascade;
CascadeClassifier lowerbody_cascade;

const String window_name = "Capture - Face  detection";

int main(int argc, const char** argv) 
{
    CommandLineParser parser(argc, argv,
        "{help h||}"
        "{face_cascade|../../../opencv-3.3.1/data/haarcascades/haarcascade_frontalface_alt.xml|}"
        "{eyes_cascade|../../../opencv-3.3.1/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml|}"
        "{upperbody_cascade|../../../opencv-3.3.1/data/haarcascades/haarcascade_upperbody.xml|}"
        "{profile_face_cascade|../../../opencv-3.3.1/data/haarcascades/haarcascade_profileface.xml|}"
        "{lowerbody_cascade|../../../opencv-3.3.1/data/haarcascades/haarcascade_lowerbody.xml|}");
    
    

    parser.about("\nThis program demonstrates using cv::CascadeClassifier class to detect objects (Face + eyes) in a video stream.\n"
            "You can use Haar or LBP features.\n\n");

    parser.printMessage();

    face_cascade_name = parser.get<String>("face_cascade");
    eyes_cascade_name = parser.get<String>("eyes_cascade");
    upperbody_cascade_name = parser.get<String>("upperbody_cascade");
    profileface_cascade_name = parser.get<String>("profile_face_cascade");
    lowerbody_cascade_name = parser.get<String>("lowerbody_cascade");

    VideoCapture capture;

    Mat frame;

    //-- 1. Load the cascades
    if (!face_cascade.load(face_cascade_name))
    {
        cout << "(!) Error loading the face cascade." << endl;
        return -1;
    }

    if (!eyes_cascade.load(eyes_cascade_name))
    {
        cout << "(!) Error loading the eyes cascade." << endl;
        return -1;
    }

    if (!upperbody_cascade.load(upperbody_cascade_name))
    {
        cout << "(!) Error loading the upper body cascade." << endl;
        return -1;
    }

    if (!profile_face_cascade.load(profileface_cascade_name))
    {
        cout << "(!) Error loading the profile face cascade." << endl;
        return -1;
    }

    if (!lowerbody_cascade.load(lowerbody_cascade_name))
    {
        cout << "(!) Error loading the lower body cascade." << endl;
    }

    //-- 2. Read the video stream
    capture.open(0);

    if (!capture.isOpened())
    {
        cout << "(!) Error opening the video capture." << endl;
        return -1;
    }

    while (capture.read(frame)) 
    {
        if (frame.empty()) 
        {
            cout << "(!) No captured frame -- Break!" << endl;
            break;
        }

        // -- 3. Apply the classfier to the frame
        detectAndDisplay(frame);

        if(waitKey(10) == 27) // Escape
        {
            break;
        }
    }

    return 0;
}

void detectAndDisplay(Mat frame)
{
    vector<Rect> faces;
    

    Mat frame_gray;

    vector<Rect> profile_faces;

    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist( frame_gray, frame_gray);

    
    // -- Detect faces

    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(60, 60));

    for (size_t i = 0; i < faces.size(); i++)
    {
        // Paint the circle for the face
        Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

        
        Mat faceROI = frame_gray(faces[i]);
        vector<Rect> eyes;
        /*
        // -- In each face detect the eyes
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        for (size_t j = 0; j < eyes.size(); j++)
        {
            Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);

            int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
            circle(frame, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0);
        }
        */
    }

    // Detect profile faces
    if(faces.size()==0)
    {    
        profile_face_cascade.detectMultiScale( frame_gray, profile_faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(60, 60));

        for (size_t i = 0; i < profile_faces.size(); i++)
        {
            Point center(profile_faces[i].x + profile_faces[i].width / 2, profile_faces[i].y + profile_faces[i].height / 2);
        // ellipse(frame, center, Size(profile_faces[i].width / 2, profile_faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
        }

        if (profile_faces.size()==0)
        {

            vector<Rect> lowerBodies;

            lowerbody_cascade.detectMultiScale(frame_gray, lowerBodies, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(60 ,60));

            for (size_t i = 0; i < lowerBodies.size(); i++)
            {
                Point center(lowerBodies[i].x + lowerBodies[i].width / 2, lowerBodies[i].y + lowerBodies[i].height / 2);

            Point p1(lowerBodies[i].x, lowerBodies[i].y);
            Point p2(lowerBodies[i].x + lowerBodies[i].width, lowerBodies[i].y + lowerBodies[i].height);
            rectangle(frame, p1, p2, Scalar(0, 255, 0), 3, 8, 0);   


            }

            if(lowerBodies.size()==0)
            {
                vector<Rect> upperBodies;

                upperbody_cascade.detectMultiScale(frame_gray, upperBodies, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(60, 60));

                for (size_t i = 0; i < upperBodies.size(); i++)
                {
                // Paint the upper bodies ellipses
                    Point center(upperBodies[i].x + upperBodies[i].width / 2, upperBodies[i].y + upperBodies[i].height / 2);
                // ellipse(frame, center, Size(upperBodies[i].width / 2, upperBodies[i].height / 2), 0, 0, 360, Scalar(0, 255, 0), 4, 8, 0);
        
                    Point p1(upperBodies[i].x, upperBodies[i].y);
                    Point p2(upperBodies[i].x + upperBodies[i].width, upperBodies[i].y + upperBodies[i].height);
                }
            }
        }
    }

/*
    vector<Rect> upperBodies;

    // -- Detect upper bodies
    upperbody_cascade.detectMultiScale(frame_gray, upperBodies, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(60, 60));

    for (size_t i = 0; i < upperBodies.size(); i++)
    {
        // Paint the upper bodies ellipses
        Point center(upperBodies[i].x + upperBodies[i].width / 2, upperBodies[i].y + upperBodies[i].height / 2);
        // ellipse(frame, center, Size(upperBodies[i].width / 2, upperBodies[i].height / 2), 0, 0, 360, Scalar(0, 255, 0), 4, 8, 0);
        
        Point p1(upperBodies[i].x, upperBodies[i].y);
        Point p2(upperBodies[i].x + upperBodies[i].width, upperBodies[i].y + upperBodies[i].height);
        rectangle(frame, p1, p2, Scalar(0, 255, 0), 3, 8, 0);
    }
*/
    // -- Show what you got
    imshow(window_name, frame);
}