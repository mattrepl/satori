/*
 * satori_app.cxx - Satori Application
 * (c) 2008 Michael Sullivan
 *
 * Version 1.0.0
 * Last Revised: 04/29/08
 *
 * Version History:
 *  1.0.0 - Primary implementation (04/29/08)
 *
 *  This program utilizes the Open Computer Vision Library (OpenCV)
 *
 */

#include "satori_app.h" 	// flow header file
#include "img_template.tpl"	// provides efficient access to pixels

// Constructors

SatoriApp::SatoriApp(){
    // Default Constructor

    // set up state of machine
    ran = false;  
    where_flow_found = NULL;
    num_tracked_points = 0;
    lk_flags = 0;
}

SatoriApp::~SatoriApp(){
    // Destructor
    // release all images
    for(int i = 0; i < orig_images.size(); i++){
        cvReleaseImage(&orig_images[i]);
        cvReleaseImage(&gray_images[i]);
        cvReleaseImage(&annotated_images[i]);
    }
}

// Access Functions

int SatoriApp::get_size(){
    return orig_images.size();
}

// Action Functions

bool SatoriApp::add(string filename){
  
    // load the image
    IplImage *img = NULL, *gray = NULL;
    img = cvLoadImage(filename.c_str(),1);  // scan in color
    if(!img) return false;

    // store the image
    orig_images.push_back(img);

    // convert to grayscale
    gray = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    cvCvtColor(img, gray, CV_BGR2GRAY);

    // store the gray image
    gray_images.push_back(gray);

    return true;
}

int SatoriApp::run(){
    // Default run function (with default verbosity)
    return run(DEFAULT_VERBOSITY);
}

int SatoriApp::run_webcam(bool verbose){
    // capture input from webcam

    CvCapture* capture = 0;
    capture = cvCaptureFromCAM(0);  // capture from default device
    if(!capture){
        return -1;
    }

    cvNamedWindow("Webcam_Capture", 0 );

    IplImage *image = NULL, *prev_grey = NULL, *grey = NULL;

    for(;;){
  
        IplImage* frame = NULL;
  
        frame = cvQueryFrame(capture);
        if( !frame )
            break;

        if(!image){	// initialize data structures the first time
            image = cvCreateImage( cvGetSize(frame), 8, 3 );
            image->origin = frame->origin;
            grey = cvCreateImage( cvGetSize(frame), 8, 1 );

            // acquire usable image, grayscale it
            cvCopy(frame, image, 0);
            cvCvtColor(image, grey, CV_BGR2GRAY);

            prev_grey = cvCreateImage(cvGetSize(frame), 8, 1 );

            flow.init(grey);
        }
        else{ // update the reading
            cvCopy(frame, image, 0);
            cvCvtColor(image, grey, CV_BGR2GRAY);
        }

        // perform operations
        // update pairs with flow information
        flow.pair_flow(prev_grey, grey);
        // track largest moving object
    
        // prepare for next captured picture
        CV_SWAP(prev_grey, grey, swap_img);

        // DEBUGGING OUTPUT
        //     for(int i = 0; i < num_tracked_points; i++){
        //       cvCircle(image, cvPointFrom32f(curr_points[i]), 3, CV_RGB(0,255,0), -1, 8,0);
        //       cout << cvPointFrom32f(curr_points[i]).x << "  " << cvPointFrom32f(curr_points[i]).y << endl;
        //     }

        // display webcam output
        cvShowImage("Webcam_Capture", image);
        cvWaitKey(10);
    }
        
    return 0;      
}

int SatoriApp::run(bool verbose){

    if(orig_images.size() == 0){
        cout << "  * " << "No images to process!" << endl;
        return 0;
    }

    if(verbose)
        cout << endl << "  * " << "Calculating optical flow for " << orig_images.size() - 1 << " pairs of images..." << endl;

    // used to store (grayscale) input images
    IplImage *img1 = NULL, *img2 = NULL, *disp_image = NULL;

    // initialize for first frame
    img1 = cvCreateImage(cvGetSize(gray_images[0]), 8, 1);
    img1->origin = gray_images[0]->origin;
    // acquire usable image, grayscale it
    cvCopy(gray_images[0], img1, 0);

    // DEBUGGING OUTPUT
    for(int j = 0; j < num_tracked_points; j++){
        //cout << cvPointFrom32f(curr_points[j]).x << "  " << cvPointFrom32f(curr_points[j]).y << endl;
    }

    // run flow algorithm on all remaining images
    for(int i = 0; i < orig_images.size() - 1; i++){
    
        // load images
        if(i == 0)	// necessary?
            img1 = cvCreateImage(cvGetSize(gray_images[i]), 8, 1);
        else
            img1 = cvCloneImage(gray_images[i]);
        img2 = cvCloneImage(gray_images[i+1]);
    
        // check that images are consistent
        int height1,width1,channels1;			
        int height2,width2,channels2;

        // parse first image data
        height1    = img1->height;
        width1     = img1->width;
        channels1  = img1->nChannels;
  
        // parse second image data
        height2    = img2->height;
        width2     = img2->width;
        channels2  = img2->nChannels;
 
        // check for inequality in image dimensions, channels
        if(!img1 || !img2 || height1 != height2 || width1 != width2 || channels1 != channels2){
            printf("[ERROR] Images are not same dimensions, number of channels!");
            return IMAGE_CONSISTENCY_FAILED;
        }

        // calculate flow between the two images (results modify private global variables)
        if(verbose)
            cout << "    * " << "Processing optical flow of image pair #" << i << "...";
        flow.pair_flow(img1, img2);

        // annotate resulting image
        annotated_images.push_back(annotate_img(orig_images[i+1]));	// animate colored second pair

        if(verbose)
            cout << "\t\t\t\t[OK]" << endl;
    }

    // mark as run
    ran = true;

    return 0;
}

void SatoriApp::animate(string outfolder){
    // Default animate function (with default verbosity)
    return animate(outfolder, DEFAULT_VERBOSITY);
}

void SatoriApp::animate(string outfolder, bool verbose){

    // make sure state of machine is ready for animation
    if(!ran){
        cout << "  * " << "Optical flow processing has not been performed!" << endl;
        return;
    }
    else if(annotated_images.size() != orig_images.size() - 1){
        cout << "  * " << "Not all images have been annotated!" << endl;
        return;
    }

    if(verbose)
        cout << endl << "  * " << "Outputing animation for " << orig_images.size() - 1 << " pairs of images..." << endl;

    for(int i = 0; i < orig_images.size() - 1; i++){

        if(verbose)
            cout << "    * " << "Animating image pair #" << i << "...";
    
        // output images
        stringstream ostream;
        string outfile;
        ostream << i << ".png";
        outfile = ostream.str();
        if(i < 10) outfile = "0" + outfile;
        if(i < 100) outfile = "0" + outfile;
        outfile = outfolder + outfile;

        cvSaveImage(outfile.c_str(),annotated_images[i]);      // add the frame to a file   

        if(verbose)
            cout << "\t\t\t\t[OK]" << endl;
    }
}

IplImage* SatoriApp::annotate_img(IplImage* img){

    // add circles for each tracked point
    int num_tracked_points = flow.point_count();
    for(int i = 0; i < num_tracked_points; i++)
        cvCircle(img, cvPointFrom32f(flow.curr_points[i]), 3, CV_RGB(0,255,0), -1, 8,0);
    
    // return annotated image
    return img;
}