/**
 * @function 
 * @brief Demo code for detecting corners using Harris-Stephens method
 * whith a video flux 
 * @author HADAPTIC team 2013
 */
#include <highgui.h>
#include <cv.h>

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat cornerHarris(int,void *, Mat src_gray);

int main() {
  // Touche clavier
  char key;
  // Image
  IplImage *image;
  Mat src, src_gray;
  
  // Capture vidéo
  CvCapture *capture;
	
  // Ouvrir le flux vidéo
  capture = cvCreateCameraCapture(CV_CAP_ANY);
  
  // Vérifier si l'ouverture du flux est ok
  if (!capture) {
    printf("Ouverture du flux vidéo impossible !\n");
    return 1;
  }
  
  // Définition de la fenêtre
  cvNamedWindow("Corners detection", CV_WINDOW_AUTOSIZE);
  
  // Boucle tant que l'utilisateur n'appuie pas sur la touche q (ou Q)
  while(key != 'q' && key != 'Q') {
    
    // On récupère une image
    image = cvQueryFrame(capture);
    
    // On la convertie en Mat de niveaux de gris ...
    src = image;
    cvtColor(src,src_gray,CV_BGR2GRAY);
    
    // Avant d'en récupérer les points d'intérets spatiaux 
    // (corners ou SIPs)
    src_gray = cornerHarris(0,0,src_gray);
    image = new IplImage(src_gray);
    // On affiche l'image dans une fenêtre
    cvShowImage("Corners detection",image);
    
    // On attend 1s
    key = cvWaitKey(1000);
  }
  
  cvReleaseCapture(&capture);
  cvDestroyWindow("Corners detection");
  return EXIT_SUCCESS;
}

/**
 * @function cornerHarris
 * @brief Demo code for detecting corners using Harris-Stephens method
 * @author OpenCV team
 * http://docs.opencv.org/doc/tutorials/features2d/trackingmotion/harris_detector/harris_detector.html
 */
Mat cornerHarris(int, void*,Mat src_gray){
  Mat dst, dst_norm, dst_norm_scaled;
  dst = Mat::zeros( src_gray.size(), CV_32FC1 );
  
  // CHOISIR LE SEUIL DE DETECTION ENTRE 0 ET 255
  // POUR LES CORNERS
  int thresh = 150;
  
  
  /// Detector parameters
  int blockSize = 2;
  int apertureSize = 3;
  double k = 0.04;

  /// Detecting corners
  cornerHarris(src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT );
  
  /// Normalizing
  normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
  convertScaleAbs( dst_norm, dst_norm_scaled );

  /// Drawing a circle around corners
  for( int j = 0; j < dst_norm.rows ; j++ )
     { for( int i = 0; i < dst_norm.cols; i++ )
	 {
            if( (int) dst_norm.at<float>(j,i) > thresh )
              {
               circle( dst_norm_scaled, Point( i, j ), 5,  Scalar(0), 2, 8, 0 );
              }
          }
     }
  /// Showing the result
  // namedWindow( corners_window, CV_WINDOW_AUTOSIZE );
  // imshow( corners_window, dst_norm_scaled );
  return dst_norm_scaled;
}
