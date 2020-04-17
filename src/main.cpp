#include <opencv2/opencv.hpp>
#include "VideoServer.h"
#include "CameraCapture.h"


int main() {

  CameraCapture cam(0);		// Video eingabe der Kamera '0'; 0, wenn nur eine Kamera angeschlossen ist
  cam.set(cv::CAP_PROP_FPS, 30);			// Kamera Framerate auf 30 fps

  VideoServer srv;				// Klasse für den VideoServer
  srv.namedWindow("input");
  srv.namedWindow("edges");
  srv.namedWindow("circles");

  cv::Mat input;
  cv::Mat gauss;
  cv::Mat edges;
  cv::Mat circles;

  while(1) {
    while(!cam.read(input)){}

    cv::GaussianBlur(input, gauss, cv::Size(5,5),2,2);		// Gaussian blur to normalize image


    edges = cv::Scalar::all(0);
    cv::Canny( gauss, edges, 10, 30, 3 );

    srv.imshow("input", input);
    srv.imshow("edges", edges);
    srv.update();
  }

  return -1;
}
