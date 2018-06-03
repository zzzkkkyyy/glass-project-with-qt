#ifndef PROCESS_H
#define PROCESS_H

//#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>
#include <vector>
#include <QString>
#include <QDebug>
using namespace cv;
using namespace std;

class Process
{
private:
    Mat input;
    Mat output;
	int my_flag;
public:
    void init(QImage *qimg);
    Mat &getOutput();
    QImage generate();
    void bengbian();
    void daobian();
	void huaheng();
    void bengjiao();
    void quejiao();
    void all();
};


#endif // PROCESS_H
