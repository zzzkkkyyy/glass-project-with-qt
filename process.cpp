#include "process.h"

void Process::init(QImage *qimg)
{
    cv::Mat input_3;
    switch(qimg->format())
    {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            input = cv::Mat(qimg->height(), qimg->width(), CV_8UC4, (void*)qimg->constBits(), qimg->bytesPerLine());
            break;
        case QImage::Format_RGB888:
            input = cv::Mat(qimg->height(), qimg->width(), CV_8UC3, (void*)qimg->constBits(), qimg->bytesPerLine());
            namedWindow("input");
            imshow("input", input);
            cv::cvtColor(input, input, CV_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            //input = cv::Mat(qimg->height(), qimg->width(), CV_8UC1, (void*)qimg->constBits(), qimg->bytesPerLine());
            qDebug() << qimg->height() << qimg->width();
            input = cv::Mat(qimg->height(), qimg->width(), CV_8UC1, (uchar*)qimg->bits(), qimg->bytesPerLine());
            qDebug() << input.rows << input.cols;
            //cv::cvtColor(input, input_3, CV_GRAY2BGRA);
            //input_3 = Mat::zeros(input.size(), CV_8U);
            //namedWindow("input");
            //imshow("input", input_3);
            break;
        default:
            break;
    }
}
Mat &Process::getOutput()
{
    return output;
}
QImage Process::generate()
{
    resize(output, output, input.size());
    //qDebug() << "output size: " << output.rows << output.cols;
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(output.type() == CV_8UC1)
    {
        QImage image(output.cols, output.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy output Mat
        uchar *pSrc = output.data;
        for(int row = 0; row < output.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, output.cols);
            pSrc += output.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(output.type() == CV_8UC3)
    {
        // Copy output Mat
        const uchar *pSrc = (const uchar*)output.data;
        // Create QImage with same dimensions as output Mat
        QImage image(pSrc, output.cols, output.rows, output.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(output.type() == CV_8UC4)
    {
        // Copy output Mat
        const uchar *pSrc = (const uchar*)output.data;
        // Create QImage with same dimensions as output Mat
        QImage image(pSrc, output.cols, output.rows, output.step, QImage::Format_ARGB32);
        return image.copy();
    }
}
void Process::bengbian()
{
	Mat initialImage = input.clone();
	cvtColor(initialImage, initialImage, CV_GRAY2BGR);
	Mat image, image_temp;
	pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
	pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
	cvtColor(initialImage, image_temp, CV_BGR2GRAY);

	Mat mask = Mat::zeros(image_temp.size(), CV_8U);
	Rect r1(120, 20, 410, 420);
	mask(r1).setTo(255);
	image_temp.copyTo(image, mask);

	Mat adaptResult;
	threshold(image, adaptResult, 100, 255, CV_THRESH_BINARY);

	std::vector<std::vector<Point>> contours;
	findContours(adaptResult, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double maxLength = 0;
	std::vector<Point> &maxContours = *(contours.begin());
	for (std::vector<std::vector<Point>>::iterator it = contours.begin(); it != contours.end(); it++)
	{
		if (maxLength < arcLength(*it, 1))
		{
			maxLength = arcLength(*it, 1);
			maxContours = *it;
		}
	}
	std::vector<std::vector<Point>> maxc;
	maxc.push_back(maxContours);
	Mat blank(adaptResult.size(), CV_8U, Scalar(0));
	drawContours(blank, maxc, -1, Scalar(255), CV_FILLED);

	Mat blank2(adaptResult.size(), CV_8U, Scalar(0));
	RotatedRect maxRect = minAreaRect(maxContours);

	Point2f pt[4];
	maxRect.points(pt);
	std::vector<std::vector<Point>> maxr;
	std::vector<Point> maxrele;
	std::vector<Vec4f> tempLines;
	for (int i = 0; i < 4; i++)
	{
		maxrele.push_back(pt[i]);
		line(blank2, pt[i], pt[(i + 1) % 4], Scalar(255));
	}

	Point2f recCenter = pt[0] + pt[2];
	recCenter.x /= 2;
	recCenter.y /= 2;

	float verticalLength = 25;//(abs(pt[1].y - pt[0].y) + abs(pt[2].y - pt[1].y)) / 2;
	float horizontalLength = 25;//(abs(pt[1].x - pt[0].x) + abs(pt[2].x - pt[1].x)) / 2;

	std::vector<Point> damagePoint;
	for (int i = 0; i < 4; i++)
	{

		LineIterator lit(blank2, pt[i], pt[(i + 1) % 4]);
		bool isStart = false;
		for (int j = 0; j < lit.count; j++, lit++)
		{
			Point ptlit(lit.pos());
			if (abs(pt[(i + 1) % 4].y - pt[i].y) <= abs(pt[(i + 1) % 4].x - pt[i].x))
			{
				int isUp;
				if ((pt[(i + 1) % 4].y + pt[i].y) >= 2 * recCenter.y)
				{
					isUp = 1;
				}
				else
				{
					isUp = -1;
				}
				bool isTouch = false;
				for (int k = 1; k < verticalLength; k++)
				{
					Point temp(ptlit.x, ptlit.y - isUp * k);
					if (blank.at<uchar>(temp) == 255)
					{
						isTouch = true;
						break;
					}
				}
				if (isTouch == false && isStart == false)
				{
					isStart = true;
					damagePoint.push_back(ptlit);
				}
				else if (isTouch == true && isStart == true)
				{
					isStart = false;
					damagePoint.push_back(ptlit);
				}
			}
			else
			{
				int isLeft;
				if ((pt[(i + 1) % 4].x + pt[i].x) >= 2 * recCenter.x)
				{
					isLeft = 1;
				}
				else
				{
					isLeft = -1;
				}
				bool isTouch = false;
				for (int k = 1; k < horizontalLength; k++)
				{
					Point temp(ptlit.x - isLeft * k, ptlit.y);
					if (blank.at<uchar>(temp) == 255)
					{
						isTouch = true;
						break;
					}
				}
				if (isTouch == false && isStart == false)
				{
					isStart = true;
					damagePoint.push_back(ptlit);
				}
				else if (isTouch == true && isStart == true)
				{
					isStart = false;
					damagePoint.push_back(ptlit);
				}
			}
		}
		if (isStart == true)
		{
			damagePoint.pop_back();
		}
	}
	for (std::vector<Point>::iterator it = damagePoint.begin(); it != damagePoint.end(); it++)
	{
		float thre = min(verticalLength, horizontalLength) / 15;
		Point pt1 = *it;
		Point pt2 = *(++it);

		{
			Point center = pt1 + pt2;
			center.x /= 2;
			center.y /= 2;
			float radius = sqrt((pt1.x - pt2.x) * (pt1.x - pt2.x) + (pt1.y - pt2.y) * (pt1.y - pt2.y));
			if (radius > 10)
                circle(initialImage, center, radius, Scalar(0, 255, 0), 2);
		}
	}
    output = initialImage.clone();
	return;
}
void Process::daobian()
{
    //qDebug() << input.rows << input.cols;
	Mat initialImage = input.clone();
	cvtColor(initialImage, initialImage, CV_GRAY2BGR);
	Mat image, image_temp;
	if (my_flag != 4)
	{
		pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
		pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
	}
	cvtColor(initialImage, image_temp, CV_BGR2GRAY);

	Mat mask = Mat::zeros(image_temp.size(), CV_8U);
	Rect r1(120, 20, 410, 420);
	mask(r1).setTo(255);
	image_temp.copyTo(image, mask);

	Mat adaptresult;
	morphologyEx(image, image, MORPH_OPEN, Mat(), Point(-1, -1), 2);
	threshold(image, adaptresult, 48, 255, THRESH_BINARY);

	vector< vector< Point> > contours;  //用于保存所有轮廓信息  
	vector< vector< Point> > contours2; //用于保存倒边不良的轮廓

	cv::findContours(adaptresult, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	vector<vector<Point> >::iterator itc = contours.begin();

	while (itc != contours.end()) {
		if (itc->size() > 200) {
			contours2.push_back(*itc);
            qDebug() << itc->size();
		}
		itc++;
	}


	//寻找最小外接矩形
	vector<RotatedRect> minRect(contours2.size());
	for (int i = 0; i < contours2.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contours2[i]));
	}

	//为之画一个矩形框
	for (int i = 0; i< contours2.size(); i++)
	{
		Point2f rect_points[4];
		minRect[i].points(rect_points);
		float length, width, ratio;
		length = pow(rect_points[0].x - rect_points[1].x, 2) + pow(rect_points[0].y - rect_points[1].y, 2);
		width = pow(rect_points[1].x - rect_points[2].x, 2) + pow(rect_points[1].y - rect_points[2].y, 2);
		ratio = length / width;
        //cout << ratio << endl;
        if (ratio > 9 || ratio < 0.11) {
			for (int j = 0; j < 4; j++)
				line(initialImage, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 2, 8);
		}
	}

	output = initialImage.clone();
	return;
}
void Process::bengjiao()
{
	Mat initialImage = input.clone();
	cvtColor(initialImage, initialImage, CV_GRAY2BGR);
	Mat image, image_temp;
	if (my_flag != 4)
	{
		pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
		pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
	}
	cvtColor(initialImage, image_temp, CV_BGR2GRAY);

	Mat mask = Mat::zeros(image_temp.size(), CV_8U);
	Rect r1(135, 25, 410, 420);
	mask(r1).setTo(255);
	image_temp.copyTo(image, mask);

	Mat adapt;
	threshold(image, adapt, 40, 255, CV_THRESH_BINARY);	
	std::vector<std::vector<Point>> contours_0;
	findContours(adapt, contours_0, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double maxArea = 0;
	std::vector<Point> &maxContours = *(contours_0.begin());
	for (std::vector<std::vector<Point>>::iterator it = contours_0.begin(); it != contours_0.end(); it++)
	{
		if (maxArea < contourArea(*it))
		{
			maxArea = contourArea(*it);
			maxContours = *it;
		}
	}

	std::vector<std::vector<Point>> maxc;
	maxc.push_back(maxContours);
	Mat blank(adapt.size(), CV_8UC3, Scalar(255, 255, 255));
	drawContours(blank, maxc, -1, Scalar(255, 0, 0), 2);
	RotatedRect maxRect = minAreaRect(maxContours);

	Point2f pt[4];
	maxRect.points(pt);
	std::vector<std::vector<Point>> maxr;
	std::vector<Point> maxrele;
	for (int i = 0; i < 4; i++)
	{
		maxrele.push_back(pt[i]);
		line(blank, pt[i], pt[(i + 1) % 4], Scalar(0, 255, 0));

	}
	maxr.push_back(maxrele);

	Mat blank_2(adapt.size(), CV_8U, Scalar(0));
	Mat blank3 = blank_2.clone();
	std::vector<std::vector<Point>> minusContours;
	drawContours(blank_2, maxr, -1, 255, CV_FILLED);
	drawContours(blank3, maxc, -1, 255, CV_FILLED);

	Mat adaptresult(image.rows, image.cols, CV_8UC3);
	image.copyTo(adaptresult, blank3);
	//
	//Mat adaptresult;
	morphologyEx(adaptresult, adaptresult, MORPH_OPEN, Mat(), Point(-1, -1), 3);
	threshold(adaptresult, adaptresult, 20, 255, THRESH_BINARY);
	dilate(adaptresult, adaptresult, Mat(), Point(-1, -1), 4);
	vector< vector< Point> > contours;
	vector< vector< Point> > contours2;
	cv::findContours(adaptresult, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	vector<vector<Point> >::iterator itc = contours.begin();

	while (itc != contours.end()) {
		if (itc->size() > 10 ) {
			contours2.push_back(*itc);
		}
		itc++;
	}

	vector<RotatedRect> minRect(contours2.size());
	for (int i = 0; i < contours2.size(); i++)
	{
		minRect[i] = minAreaRect(Mat(contours2[i]));
	}

	for (int i = 0; i< contours2.size(); i++)
	{
		// rotated rectangle
		Point2f rect_points[4]; minRect[i].points(rect_points);
		bool flag = true,flag1=false;
		for (int j = 0; j < 4; j++)
		{
			float cha;
			cha = norm(rect_points[j] - rect_points[(j + 1) % 4]);
			if (fabs(cha) > 50.0) {
				flag = false; break;
			}
		}
		
		for (int j = 0; j < 4; j++)
		{
			float cha;
			cha = norm(rect_points[j] - pt[(j + 1) % 4]);
			if (fabs(cha) < 30.0) {
				flag1 = true; break;
			}
		}
		if (flag && flag1)
		for (int j = 0; j < 4; j++)
			line(initialImage, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 0, 255), 2, 8);

	}

	output = initialImage.clone();
	return;
}
void Process::quejiao()
{
	Mat initialImage = input.clone();
	cvtColor(initialImage, initialImage, CV_GRAY2BGR);
	Mat image, image_temp;
	if (my_flag != 4)
	{
		pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
		pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
	}
	
	cvtColor(initialImage, image_temp, CV_BGR2GRAY);

	Mat mask = Mat::zeros(image_temp.size(), CV_8U);
	Rect r1(135, 25, 420, 420);
	mask(r1).setTo(255);
	image_temp.copyTo(image, mask);

	Mat adaptresult;
	threshold(image, adaptresult, 100, 255, THRESH_BINARY);

	std::vector<std::vector<Point>> contours;
	findContours(adaptresult, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double maxArea = 0;
	std::vector<Point> &maxContours = *(contours.begin());
	for (std::vector<std::vector<Point>>::iterator it = contours.begin(); it != contours.end(); it++)
	{
		if (maxArea < contourArea(*it))
		{
			maxArea = contourArea(*it);
			maxContours = *it;
		}
	}

	std::vector<std::vector<Point>> maxc;
	maxc.push_back(maxContours);
	Mat blank(adaptresult.size(), CV_8UC3, Scalar(255, 255, 255));
	drawContours(blank, maxc, -1, Scalar(255, 0, 0), 2);
	RotatedRect maxRect = minAreaRect(maxContours);

	Point2f pt[4];
	maxRect.points(pt);
	std::vector<std::vector<Point>> maxr;
	std::vector<Point> maxrele;
	for (int i = 0; i < 4; i++)
	{
		maxrele.push_back(pt[i]);
		line(blank, pt[i], pt[(i + 1) % 4], Scalar(0, 255, 0));

	}
	maxr.push_back(maxrele);

	Mat blank2(adaptresult.size(), CV_8U, Scalar(0));
	Mat blank3 = blank2.clone();
	std::vector<std::vector<Point>> minusContours;
	drawContours(blank2, maxr, -1, 255, CV_FILLED);
	drawContours(blank3, maxc, -1, 255, CV_FILLED);

	blank2 -= blank3;

	Mat blank4(adaptresult.size(), CV_8U, Scalar(0));
	morphologyEx(blank2, blank2, MORPH_OPEN, Mat(), Point(-1, -1), 1);
	findContours(blank2, minusContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Point2f center;
	float radius;
	for (int i = 0; i < minusContours.size(); i++)
	{
		minEnclosingCircle(minusContours[i], center, radius);
		if (radius>1.7 && radius<10)
			circle(initialImage, center, 2 * radius, Scalar(0, 0, 255), 2);
	}

	output = initialImage.clone();
	return;
}
void Process::huaheng()
{
	Mat initialImage = input.clone();
	cvtColor(initialImage, initialImage, CV_GRAY2BGR);
	Mat image, image_temp;
	if (my_flag != 4)
	{
		pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
		pyrDown(initialImage, initialImage, Size(initialImage.cols / 2, initialImage.rows / 2));
	}
	cvtColor(initialImage, image_temp, CV_BGR2GRAY);

	Mat mask = Mat::zeros(image_temp.size(), CV_8U);
	Rect r1(135, 25, 420, 420);
	mask(r1).setTo(255);
	image_temp.copyTo(image, mask);
	
	Mat adapt;
	threshold(image, adapt, 100, 255, CV_THRESH_BINARY);
	
	std::vector<std::vector<Point>> contours;
	findContours(adapt, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double maxArea = 0;
	std::vector<Point> &maxContours = *(contours.begin());
	for (std::vector<std::vector<Point>>::iterator it = contours.begin(); it != contours.end(); it++)
	{
		if (maxArea < contourArea(*it))
		{
			maxArea = contourArea(*it);
			maxContours = *it;
		}
	}

	std::vector<std::vector<Point>> maxc;
	maxc.push_back(maxContours);
	Mat blank(adapt.size(), CV_8UC3, Scalar(255, 255, 255));
	drawContours(blank, maxc, -1, Scalar(255, 0, 0), 2);
	RotatedRect maxRect = minAreaRect(maxContours);

	Point2f pt[4];
	maxRect.points(pt);
	std::vector<std::vector<Point>> maxr;
	std::vector<Point> maxrele;
	for (int i = 0; i < 4; i++)
	{
		maxrele.push_back(pt[i]);
		line(blank, pt[i], pt[(i + 1) % 4], Scalar(0, 255, 0));
	}
	maxr.push_back(maxrele);

	Mat blank_2(adapt.size(), CV_8U, Scalar(0));
	Mat blank3 = blank_2.clone();
	std::vector<std::vector<Point>> minusContours;
	drawContours(blank_2, maxr, -1, 255, CV_FILLED);
	drawContours(blank3, maxc, -1, 255, CV_FILLED);

	threshold(adapt, adapt, 100, 255, CV_THRESH_BINARY_INV);
	
	Mat blank2(adapt.size(), CV_8U, Scalar(0));
	drawContours(blank2, maxr, -1, Scalar(1), CV_FILLED);
	image = image.mul(blank2);
	
	//自适应阈值化操作
	medianBlur(image, image, 5);
	Mat adaptresult;
	Mat adaptmul(image.size(), CV_8U, Scalar(0));
	int start = 5, end = 70, step = 2;
	for (int i = start; i < end; i += step)
	{
		adaptiveThreshold(image, adaptresult, 1, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, i, -2);
		morphologyEx(adaptresult, adaptresult, MORPH_CLOSE, Mat(), Point(-1, -1), 2);
		adaptmul += adaptresult;
	}
    threshold(adaptmul, adaptmul, (end - start) / step - 2, 255, CV_THRESH_BINARY);
	//erode(adaptmul, adaptmul, Mat(5, 5, CV_8U, Scalar(255)), Point(-1, -1), 1);
	dilate(adaptmul, adaptmul, Mat(5, 5, CV_8U, Scalar(255)), Point(-1, -1), 2);
	//提取划痕
	std::vector<std::vector<Point>> inner_contours;
	findContours(adaptmul, inner_contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    qDebug() << "detected scratch num:" << inner_contours.size();
	std::vector<std::vector<Point>> inner_c;
	for (std::vector<std::vector<Point>>::iterator it = inner_contours.begin(); it != inner_contours.end(); it++)
	{
		if (arcLength(*it, 1) > 60 && arcLength(*it, 1) < 1000)
		{
			RotatedRect rect = minAreaRect(*it);
			Point2f pt[4];
			rect.points(pt);
			float epi = 1.8;
			if (abs((pt[0].x - pt[1].x) / (pt[1].y - pt[2].y) > epi || (pt[0].x - pt[1].x) / (pt[1].y - pt[2].y) < (1.0 / epi)))
			{
				inner_c.push_back(*it);
				for (int i = 0; i < 4; i++)
				{
					line(initialImage, pt[i], pt[(i + 1) % 4], Scalar(255, 0, 0),2);
				}
			}
		}
	}
    qDebug() << "good scratch num:" << inner_c.size();
    drawContours(initialImage, inner_c, -1, Scalar(0, 255, 0), 2);
    namedWindow("contours");
    imshow("contours", initialImage);
	output = initialImage.clone();
    return;
}
void Process::all()
{
    Mat temp_result=input.clone();
	Mat tempin=input.clone();

	my_flag = 4;
    bengbian();
    //temp_result = output - temp;
	input = output.clone();
	cvtColor(input, input, CV_BGR2GRAY);
    daobian();
    // += output - input;
	input = output.clone();
	cvtColor(input, input, CV_BGR2GRAY);
    quejiao();
   // 
	input = output.clone();
	cvtColor(input, input, CV_BGR2GRAY);
	huaheng();
	input = output.clone();
	cvtColor(input, input, CV_BGR2GRAY);
    bengjiao();
 //   temp_result += output - input;
	
	input = temp_result;
	my_flag = 5;
    return;
}
