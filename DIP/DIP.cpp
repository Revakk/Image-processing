// DIP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream> 
#include <vector> 
#include "Etalon.h"
#include "structs.h"
#include <fstream>
#include <string>
#include "kMeans.h"

#define height 400
#define width 400


//struct imageColor {
//	uchar r;
//	uchar g;
//	uchar b;
//};

//struct etalon {
//	cv::Scalar color;
//	std::vector<cv::Point> er;
//};

//struct objectMoment {
//	double m00;
//	double m01;
//	double m10;
//	double xt;
//	double yt;
//	double u00;
//	double u11;
//	double u02;
//	double u20;
//	double F1;
//	double F2;
//	unsigned perimeter = 0;
//};

int index = 0;

void train(NN* nn)
{
	int n = 16;
	auto** trainingSet = new double* [n];
	double f1, f2, a1, a2, a3;
	std::ifstream neuralDataFile;
	neuralDataFile.open("dataclear.txt");
	if (!neuralDataFile) {
		std::cout << "Unable to open file";
		return;
	}

	for (int i = 0; i < n; i++) {
		trainingSet[i] = new double[nn->n[0] + nn->n[nn->l - 1]];
		neuralDataFile >> f1 >> f2 >> a1 >> a2 >> a3;
		int key = 0;
		for (auto value : { f1,f2,a1,a2,a3 }) {
			trainingSet[i][key] = value;
			++key;
		}
	}
	neuralDataFile.close();

	double error = 1.0;
	int i = 0;
	while (error > 0.001) {
		setInput(nn, trainingSet[i % n]);
		feedforward(nn);
		error = backpropagation(nn, &trainingSet[i % n][nn->n[0]]);
		i++;
		//std::cout << "err="<< error<< "\n";
	}
	std::cout << "err=" << error << "\n";
	std::cout << "(" << i << " iterations)\n";

	for (int i = 0; i < n; i++) {
		delete[] trainingSet[i];
	}
	delete[] trainingSet;
}



void indexImg(cv::Mat& input_img, int y, int x, int& index)
{

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			//if (((y + i)<0) || ((x + j)<0) || (x + j >= input_img.cols) || (y + i >= input_img.rows))
			if (((y + i) < 0) || ((x + j) < 0) || (x + j >= input_img.cols) || (y + i >= input_img.rows))
			{
				continue;
			}
			if (input_img.at<uchar>(y + i, x + j) == 255)
			{
				input_img.at<uchar>(y + i, x + j) = index;
				indexImg(input_img, y + i, x + j, index);
			}
		}
	}
}

void imageIndexing(cv::Mat& input_img, int &index)
{
	for (int y = 0; y < input_img.rows; y++) {
		for (int x = 0; x < input_img.cols; x++) {
			if ((input_img.at<uchar>(y, x) == 255) && (input_img.at<uchar>(y, x) != index))
			{
				index++;
				indexImg(input_img, y, x, index);
			}
		}
	}
}

void colorMyImage(cv::Mat& input_img, int& index)
{
	//cv::Mat coloredImg;
	cv::cvtColor(input_img, input_img, cv::COLOR_GRAY2BGR);
	imageColor* arr = new imageColor[index];
	for (int c = 0; c < index; c++)
	{
		arr[c].r = uchar(rand() % 255+1);
		arr[c].g = uchar(rand() % 255+1);
		arr[c].b = uchar(rand() % 255+1);
	}
	for (int y = 0; y < input_img.rows; y++) 
	{
		for (int x = 0; x < input_img.cols; x++) 
		{
			for (int i = 1; i <= index; i++)
			{
				if (input_img.at<cv::Vec3b>(y, x)[0] == i)
				{
					input_img.at<cv::Vec3b>(y, x)[0] = arr[i-1].r;
					input_img.at<cv::Vec3b>(y, x)[1] = arr[i - 1].g;
					input_img.at<cv::Vec3b>(y, x)[2] = arr[i - 1].b;

				}
			}
		}
	}

	delete[] arr;
}

void computeFeature(cv::Mat& indexedImg, int index, objectMoment& struc)
{
	imageColor* arr = new imageColor[index];

} 

double moment(int y, int x, int p, int q)
{
	return (pow(x, p) * pow(y, q));
}


double compute_centerOfMass(objectMoment& struc,int y, int x, int p, int q)
{
	return (pow((x - struc.xt), p) * pow((y - struc.yt), q));
}

double feature_F1(double circumference, double area)
{
	/*std::cout << "circumference: " << circumference << std::endl;
	std::cout << "area: " << area << std::endl;*/
	return (pow((circumference), 2) / (100.0 * area));
}

double feature_F2(double umin, double umax)
{
	return umin / umax;
}

double umin_umax(bool max, double u20, double u02, double u11)
{
	if(max)
	{
		return (((1 / 2.0) * (u20 + u02)) + ((1 / 2.0) * sqrt(4 * pow(u11, 2) + pow((u20 - u02), 2))));
	}
	else
	{
		return (((1 / 2.0) * (u20 + u02)) - ((1 / 2.0) * sqrt(4 * pow(u11, 2) + pow((u20 - u02), 2))));
	}
}

unsigned int compute_Nr(cv::Mat IndexedTestImage, int index, unsigned int & Nr)
{

	for (int y = 0; y < IndexedTestImage.rows; y++)
	{
		for (int x = 0; x < IndexedTestImage.cols; x++)
		{
			if (IndexedTestImage.at<uchar>(y, x) == index)
			{
				Nr++;
			}
		}
	}
	return Nr;
}


void compute_ethalon(cv::Mat testImage, const unsigned int Nr, int index)
{
	
}

//void computeMoment(int y, int x, cv::Mat& indexedImg, int index, objectMoment & struc)
//{
//	//struc.m00, struc.m01, struc.m10, struc.u00, struc.u11, struc.u02, struc.u20 = 0,0,0,0,0,0,0;
//	struc.m00 = 0;
//	struc.m01 = 0;
//	struc.m10 = 0;
//	cv::Mat copyIndexedImg = indexedImg.clone();
//	std::queue<std::pair<unsigned int, unsigned int>> q;
//	const unsigned int COLS = indexedImg.cols, ROWS = indexedImg.rows;
//	uchar colorDifferent = (index > 254) ? index - 1 : index + 1;
//	unsigned int var = 1;
//	q.push({ y,x });
//	while (!q.empty())
//	{
//		//std::pair <unsigned int, unsigned int> pp = q.front()
//		auto pp = q.front();
//		y = pp.first;
//		x = pp.second;
//		q.pop();
//		for (int i = -1; i < 2; i++)
//		{
//			for (int j = -1; j < 2; j++)
//			{
//				if (((y + i) < 0) || ((x + j) < 0) || (x + j >= indexedImg.cols) || (y + i >= indexedImg.rows))
//				{
//					continue;
//				}
//				if (copyIndexedImg.at<uchar>(y + i, x + j) == index)
//				{
//					q.push({y+i,x+j});
//					struc.m10 += moment(y + i, x + j, 1, 0);
//					struc.m01 += moment(y + i, x + j, 0, 1);
//					struc.m00++;
//					copyIndexedImg.at<uchar>(y + i, x + j) = colorDifferent;
//				}
//			}
//		}
//	}
//}


void computeMoment(int y, int x, cv::Mat& indexedImg, int index, objectMoment & struc)
{
	//struc.m00, struc.m01, struc.m10, struc.u00, struc.u11, struc.u02, struc.u20 = 0,0,0,0,0,0,0;
	struc.m00 = 0;
	struc.m01 = 0;
	struc.m10 = 0;
	cv::Mat copyIndexedImg = indexedImg.clone();
	std::queue<std::pair<unsigned int, unsigned int>> q;
	const unsigned int COLS = indexedImg.cols, ROWS = indexedImg.rows;
	uchar colorDifferent = (index > 254) ? index - 1 : index + 1;
	unsigned int var = 1;
	q.push({ y,x });
	while (!q.empty())
	{
		//std::pair <unsigned int, unsigned int> pp = q.front()
		auto pp = q.front();
		y = pp.first;
		x = pp.second;
		q.pop();
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (((y + i) < 0) || ((x + j) < 0) || (x + j >= indexedImg.cols) || (y + i >= indexedImg.rows))
				{
					continue;
				}
				if (copyIndexedImg.at<uchar>(y + i, x + j) == index)
				{
					q.push({y+i,x+j});
					struc.m10 += moment(y + i, x + j, 1, 0);
					struc.m01 += moment(y + i, x + j, 0, 1);
					//struc.m11 += moment(y + i, x + j, 1, 1);
					//struc.m20 += moment(y + i, x + j, 2, 0);
					//struc.m02 += moment(y + i, x + j, 0, 2);
					struc.m00++;
					copyIndexedImg.at<uchar>(y + i, x + j) = colorDifferent;
				}
			}
		}
	}
}

void computeFeature(int y, int x, cv::Mat& indexedImg, int index, objectMoment& struc)
{
	struc.u00 = 0;
	struc.u11 = 0;
	struc.u02 = 0;
	struc.u20 = 0; 
	struc.xt = struc.m10 / struc.m00;
	struc.yt = struc.m01 / struc.m00;
	struc.F1 = 0;
	struc.F2 = 0;
	cv::Mat copyIndexedImg = indexedImg.clone();
	std::queue<std::pair<unsigned int, unsigned int>> q;
	const unsigned int COLS = indexedImg.cols, ROWS = indexedImg.rows;
	uchar colorDifferent = (index > 254) ? index - 1 : index + 1;
	unsigned int var = 1;
	q.push({ y,x });
	while (!q.empty())
	{
		//std::pair <unsigned int, unsigned int> pp = q.front()
		auto pp = q.front();
		y = pp.first;
		x = pp.second;
		q.pop();
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (((y + i) < 0) || ((x + j) < 0) || (x + j >= indexedImg.cols) || (y + i >= indexedImg.rows))
				{
					continue;
				}
				if (copyIndexedImg.at<uchar>(y + i, x + j) == index)
				{
					q.push({ y + i,x + j });
					struc.u00 += compute_centerOfMass(struc, y + i, x + j, 0, 0);
					struc.u11 += compute_centerOfMass(struc, y + i, x + j, 1, 1);
					struc.u02 += compute_centerOfMass(struc, y + i, x + j, 0, 2);
					struc.u20 += compute_centerOfMass(struc, y + i, x + j, 2, 0);
					copyIndexedImg.at<uchar>(y + i, x + j) = colorDifferent;
				}
			}
		}
	}
	struc.F1 = feature_F1(struc.perimeter,struc.u00);
	struc.F2 = feature_F2(umin_umax(false, struc.u20, struc.u02, struc.u11), umin_umax(true, struc.u20, struc.u02, struc.u11));

}

int isPerimeter(cv::Mat& input_img, int y, int x, int index)
{
	if (input_img.at<uchar>(y, x + 1) != index || input_img.at<uchar>(y + 1, x) != index || input_img.at<uchar>(y, x - 1) != index || input_img.at<uchar>(y - 1, x) != index)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int perimeter(cv::Mat& input_img, int index)
{
	int pom = 0;
	for (int y = 0; y < input_img.rows; y++)
	{
		for (int x = 0; x < input_img.cols; x++)
		{
			if (input_img.at<uchar>(y, x) == index)
			{
				pom += isPerimeter(input_img, y, x, index);
			}
		}
	}
	return pom;
}

void compute_moments_and_features(cv::Mat indexedImg, objectMoment * structArray)
{
	structArray->perimeter = 0;
	int idx = 1;
	for (int y = 0; y < indexedImg.rows; y++) {
		for (int x = 0; x < indexedImg.cols; x++) {
			if (indexedImg.at<uchar>(y, x)==idx)
			{
				structArray[idx - 1].perimeter = perimeter(indexedImg, idx);
				computeMoment(y, x, indexedImg, idx, structArray[idx-1]);
				computeFeature(y, x, indexedImg, idx, structArray[idx - 1]);
				idx++;
			}
		}
	}

}


void MyFilledCircle(cv::Mat img, cv::Point center, cv::Scalar color)
{
	cv::circle(img,
		center,
		3.5,
		color,
		cv::FILLED,
		cv::LINE_8);
}
std::vector<double> computeEtalons(std::vector<cv::Point> featurePoints)
{
	std::vector<double> classes;
	unsigned index = 0;
	for (int i = 0; i < featurePoints.size(); i++)
	{
		 
	}

	return classes;
}


//void kMeans(const int& index)
//{
//	std::vector<cv::Point> centroids;
//	for(int i = 0; i < index; i++)
//	{
//
//	}
//}


//void trainEtalons(std::vector<cv::Point> & featureVector, int & distance, std::vector<etalon> etalons)
//{
//	int etalonIndex = 0;
//	int lastIndex = 0;
//
//	while (lastIndex != featureVector.size() - 1)
//	{
//		for (int i = 0; i < featureVector.size(); i++)
//		{
//
//		}
//	}
//
//}

int main()
{
	cv::Mat trainImg = cv::imread("images/train.png", CV_LOAD_IMAGE_GRAYSCALE);
	for (int y = 0; y < trainImg.rows; y++) {
		for (int x = 0; x < trainImg.cols; x++) {
			trainImg.at<uchar>(y, x) = (trainImg.at<uchar>(y, x) >= 127) ? (uchar)255 : (uchar)0;
		}
	}
	cv::imshow("trainImg", trainImg);
	imageIndexing(trainImg, index);
	//colorMyImage(trainImg, index);
	objectMoment* momentArr1 = new objectMoment[index];
	compute_moments_and_features(trainImg, momentArr1);
	cv::Scalar colorClass = (255, 255, 255);

	std::vector<cv::Point> featureVector;
	cv::Mat atom_image = cv::Mat::zeros(height, width, CV_8UC3);
	for (int a = 0; a < index; a++)
	{
		MyFilledCircle(atom_image, cv::Point(momentArr1[a].F1 * height, momentArr1[a].F2 * width), colorClass);
		featureVector.push_back(cv::Point(momentArr1[a].F1, momentArr1[a].F2));
	}
	
	Etalons ctverec("ctverec", cv::Scalar(255, 0, 0));
	Etalons obdelnik("obdelnik", cv::Scalar(0, 255, 0));
	Etalons hvezda("hvezda", cv::Scalar(0, 0, 255));
	std::ofstream mujSobour("dataclear.txt");
	//mujSobour.open("dataclearr.txt");
	int a = 0;
	for (int i = 0; i < 4; i++)
	{
		ctverec.set(momentArr1[i]);
		mujSobour << std::to_string(momentArr1[i].F1) + " " + std::to_string(momentArr1[i].F2) + " " + "1 0 0" +"\n";
	}

	for (int i = 4; i < 8; i++)
	{
		hvezda.set(momentArr1[i]);
		mujSobour << std::to_string(momentArr1[i].F1) + " " + std::to_string(momentArr1[i].F2) + " " + "0 1 0" + "\n";
	}

	for (int i = 8; i < 12; i++)
	{
		obdelnik.set(momentArr1[i]);
		mujSobour << std::to_string(momentArr1[i].F1) + " " + std::to_string(momentArr1[i].F2) + " " + "0 0 1" + "\n";
	}
	mujSobour.close();
	cv::Mat etalons_image = cv::Mat::zeros(height, width, CV_8UC3);
	std::vector<Etalons> etalons = { ctverec,obdelnik,hvezda };
	//****************************************************************************************************//

	MyFilledCircle(atom_image, cv::Point(ctverec.getX()*width, ctverec.getY()*height), ctverec.getColor());
	MyFilledCircle(atom_image, cv::Point(hvezda.getX() * width, hvezda.getY() * height), hvezda.getColor());
	MyFilledCircle(atom_image, cv::Point(obdelnik.getX() * width, obdelnik.getY() * height), obdelnik.getColor());
	cv::imshow("train_img_etalons", atom_image);
	cv::Mat testImg = cv::imread("images/test01.png", CV_LOAD_IMAGE_GRAYSCALE);
	for (int y = 0; y < testImg.rows; y++) {
		for (int x = 0; x < testImg.cols; x++) {
			testImg.at<uchar>(y, x) = (testImg.at<uchar>(y, x) >= 127) ? (uchar)255 : (uchar)0;
		}
	}

	cv::imshow("testImg", testImg);
	index = 0;
	imageIndexing(testImg, index);
	objectMoment* testArr = new objectMoment[index];
	compute_moments_and_features(testImg, testArr);
	double distEtalons = 0;
	for (int i = 0; i < etalons.size(); i++)
	{
		if (i + 1 == etalons.size())
		{
			distEtalons += (etalons[i].computeDistE(cv::Point(etalons[0].getY(), etalons[0].getX())))/2.0;
		}
		else
		{
			distEtalons += (etalons[i].computeDistE(cv::Point(etalons[i + 1].getY(), etalons[i + 1].getX())))/2.0;
		}
	}
	std::vector<cv::Point> featureVectorTest;
	for (int a = 0; a < index; a++)
	{
		std::cout << testArr[a].F1 << std::endl;
		featureVectorTest.push_back(cv::Point(static_cast<double>(testArr[a].F1), static_cast<double>(testArr[a].F2)));
	}
	std::cout << "featurevectorTEST size: " << featureVectorTest.size() << std::endl;
	for (int i = 0; i < featureVectorTest.size(); i++)
	{
		std::cout << featureVectorTest[i].x << std::endl;
	}

	distEtalons = distEtalons / 3.0;
	cv::Mat atom_image2 = cv::Mat::zeros(height, width, CV_8UC3);
	//for (int a = 0; a < index; a++)
	//{
	//	//MyFilledCircle(atom_image2, cv::Point(testArr[a].F1 * height, testArr[a].F2 * width), colorClass);
	//	featureVector.push_back(cv::Point(momentArr1[a].F1, momentArr1[a].F2));
	//}

	std::cout << distEtalons << std::endl;
	for (int i = 0; i < etalons.size(); i++)
	{
		for (int c = 0; c < index; c++)
		{
			if (etalons[i].computeDist(testArr[c]) < distEtalons)
			{
				MyFilledCircle(atom_image2, cv::Point(testArr[c].F1 * height, testArr[c].F2 * width), etalons[i].getColor());
			}
		}
	}

	kMeans kMeans(3, 500);
	for (int i = 0; i < index; i++)
	{
		kMeans.addPoint(Point(testArr[i].F1, testArr[i].F2));
	}
	kMeans.runkMeans();

	for (int i = 0; i < index; i++)
	{
		std::cout << static_cast<int>(testArr[i].xt) << "  ";
		std::cout << static_cast<int>(testArr[i].yt) << std::endl;
		testImg.at<uchar>(static_cast<int>(testArr[i].yt), static_cast<int>(testArr[i].xt)) = (uchar)255;
		//std::cout << featureVectorTest[i].x << std::endl;
		const auto idx = kMeans.neighbourCluster(Point(testArr[i].F1, testArr[i].F2));
		cv::Point centerPoint = cv::Point(testArr[i].F1*height,testArr[i].F2*width);
		cv::putText(atom_image2, idx, centerPoint, cv::FONT_HERSHEY_PLAIN, 1.0,
			cv::Scalar(255), 2);
	}




		//const auto idx = kMeans.neighbourCluster(Point(featureVector[i].x, featureVector[i].y));
		//cv::Point centerPoint = cv::Point(static_cast<int>(featureVector[i].x) - 20, featureVector[i].y);
		//cv::putText(atom_image2, idx, centerPoint, cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255), 1);

	/*for (int i = 0; i < index; i++)
	{
		double min = etalons[0].computeDist(testArr[i]);
		int index_min = 0;
		for (int c = 0; c < etalons.size(); c++)
		{
			if (etalons[c].computeDist(testArr[i]) < min)
			{
				min = etalons[c].computeDist(testArr[i]);
				index_min = c;
			}
		}
	}*/
	cv::Mat neural_image = cv::imread("images/test01.png", CV_LOAD_IMAGE_GRAYSCALE);
	std::string objects_name[4] = { "Ctverec", "Hvezda", "Obdelnik" };
	NN* nn = createNN(2, 4, 3);
	train(nn);
	double* in = new double[nn->n[0]];
	for (int i = 0; i < index; i++)
	{
		int key = 0;
		for (auto value : { testArr[i].F1, testArr[i].F2}) {
			in[key] = value;
			key++;
		}
		setInput(nn, in, true);
		feedforward(nn);
		int output = getOutput(nn, true);
		cv::Point centersPoint = cv::Point(static_cast<int>(testArr[i].xt), static_cast<int>(testArr[i].yt));

		cv::putText(neural_image, objects_name[output], centersPoint, cv::FONT_HERSHEY_PLAIN, 1.0,
			cv::Scalar(150), 1);
	}

	cv::imshow("neural", neural_image);
	cv::imshow("b", atom_image2);
	cv::imshow("testimg", testImg);
	//mujSobour.clear();
	cv::waitKey(0); // wait until keypressed
	return 0;
}
