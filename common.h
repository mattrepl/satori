#ifdef _CH_
#pragma package <opencv>
#endif

#ifndef _COMMON_H_
#define _COMMON_H_

#include "cv.h"
#include "highgui.h"
#include <algorithm>
#include <iostream>

using namespace std;

const bool DEFAULT_VERBOSITY = true;	// assume verbose
const int MAX_POINTS_TO_TRACK = 500;	// maximum number of points to track
const int WINDOW_SIZE = 5;	// size of neighborhood about a pixel to determine corners
static CvSize FRAME_SIZE = cvSize(640, 480);

#define IMAGE_CONSISTENCY_FAILED -1;
#define NO_IMAGES -2;

void draw_box(const CvBox2D*, IplImage*, const CvScalar&);
void draw_comp(const CvConnectedComp*, IplImage*, const CvScalar&);
void draw_points(CvPoint2D32f*, int, IplImage*, const CvScalar&);
void intersect_amount(IplImage*, IplImage*, IplImage*, 
                      float&, float&, float&);
void rect_to_points(const CvRect& rect, CvPoint points[]);

#endif
