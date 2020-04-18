#include <opencv2/opencv.hpp>
#include <vector>
#include "VideoServer.h"
#include "CameraCapture.h"
#include "config_file.h"

int main() {

  CameraCapture cam(0);		// Video eingabe der Kamera '0'; 0, wenn nur eine Kamera angeschlossen ist
  cam.set(cv::CAP_PROP_FPS, 30);			// Kamera Framerate auf 30 fps

  VideoServer srv;				// Klasse für den VideoServer
  srv.namedWindow("input");
  srv.namedWindow("edges");
  srv.namedWindow("circles");

  cv::Mat input, gauss, gray;
  cv::Mat grad_x, grad_y, abs_grad_x, abs_grad_y;
  cv::Mat edges;

  configuration::data configdata;

  std::ifstream ifs("/home/pi/projects/Balldetection/src/config.info", std::ifstream::in);			// Config datei einlesen
	ifs >> configdata;			// Config laden
	ifs.close();

  int mindist = configdata.getintvalue("MINDIST");
  double dp = configdata.getdoublevalue("DP");
  double param1 = configdata.getdoublevalue("PARAM1");
  double param2 = configdata.getdoublevalue("PARAM2");

  while(1) {
    while(!cam.read(input)){}

    cv::GaussianBlur(input, gauss, cv::Size(5,5),2,2);		// Gaussian blur to normalize image

    cv::cvtColor(gauss, gray, cv::COLOR_BGR2GRAY);
    //edges = cv::Scalar::all(0);
    //cv::Canny( gauss, edges, 10, 30, 3 );

    cv::Sobel(gray, grad_x, CV_16S, 1, 0, 3);
    cv::Sobel(gray, grad_y, CV_16S, 0, 1, 3);
    // converting back to CV_8U
    cv::convertScaleAbs(grad_x, abs_grad_x);
    cv::convertScaleAbs(grad_y, abs_grad_y);
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edges);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, dp,
      mindist,  // change this value to detect circles with different distances to each other
      param1, param2); // change the last two parameters
        // (min_radius & max_radius) to detect larger circles


    for( size_t i = 0; i < circles.size(); i++ ) {
      cv::Vec3i c = circles[i];
      cv::Point center = cv::Point(c[0], c[1]);
      // circle center
      cv::circle( input, center, 1, cv::Scalar(0,100,100), 3, cv::LINE_AA);
      // circle outline
      int radius = c[2];
      cv::circle( input, center, radius, cv::Scalar(255,0,255), 3, cv::LINE_AA);
    }




    srv.imshow("input", input);
    srv.imshow("edges", edges);
    //srv.imshow("circles", circles);
    srv.update();
  }

  return -1;
}
