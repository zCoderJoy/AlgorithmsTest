//处理两幅图片
#include <iostream>
#include "cv.h"
#include "highgui.h"
using namespace std;
//全局参数
vector<double> g_angle;//角度
vector<double> g_amplitude;//幅度
int array9x9[9][9]={0};//大17点
int array5x5[5][5]={0};//小8点
int array3x3L1[3][3]={0};//菱形8点
int array3x3Z1[3][3]={0};//正方形8点
double distants(double x1,double y1,double x2,double y2);//根据两点求返回距离
double Angle(double x1,double y1,double x2,double y2);//求返回角度 360度
bool is_outRect(int m,int n);//是否是外围正方形
bool is_MotionArea(IplImage *src,int x,int y);//16*16区域，y是高度，x是宽度
void CalcHistogram(vector<double> VectorScr,int BinNum,int bins[],int max,int min);//计算直方图2
void drawHistogram(IplImage *dst,vector<double> vectordata,int binsNums,int bins[],int min,int max);//画直方图
//主函数
int main()
{
	//9X9初始化
	array9x9[0][0]=1;array9x9[0][4]=1;array9x9[0][8]=1;array9x9[2][4]=1;
	array9x9[3][3]=1;array9x9[3][5]=1;array9x9[4][0]=1;array9x9[4][2]=1;
	array9x9[4][4]=1;array9x9[4][6]=1;array9x9[4][8]=1;array9x9[5][3]=1;
	array9x9[5][5]=1;array9x9[6][4]=1;array9x9[8][0]=1;array9x9[8][4]=1;
	array9x9[8][8]=1;
	//5x5
	array5x5[0][0]=1;array5x5[0][2]=1;array5x5[0][4]=1;array5x5[2][0]=1;
	array5x5[2][4]=1;array5x5[4][0]=1;array5x5[4][2]=1;array5x5[4][4]=1;
	//3x3菱形
	array3x3L1[0][1]=1;array3x3L1[1][0]=1;array3x3L1[1][2]=1;array3x3L1[2][1]=1;
	//3x3正方形
	array3x3Z1[0][0]=1;array3x3Z1[1][0]=1;array3x3Z1[1][2]=1;array3x3Z1[2][1]=1;
	array3x3Z1[2][2]=1;array3x3Z1[0][1]=1;array3x3Z1[0][2]=1;array3x3Z1[2][0]=1;
	IplImage* imgA = cvLoadImage("D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/70.bmp",CV_LOAD_IMAGE_GRAYSCALE);//加载图像
	IplImage* imgB = cvLoadImage("D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/71.bmp",CV_LOAD_IMAGE_GRAYSCALE);
	CvSize  img_sz = cvGetSize( imgA );//得到图像大小
	IplImage* histimg_Amplitude = cvCreateImage( cvSize(320,200), 8, 3 );//幅度直方图统计
	IplImage* histimg_Angle = cvCreateImage( cvSize(320,200), 8, 3 );//角度直方图统计
	cvZero( histimg_Amplitude );
	cvZero( histimg_Angle );
	//cvSmooth(imgA,imgA,CV_GAUSSIAN,5,5);//高斯平滑3*3
	//cvSmooth(imgB,imgB,CV_GAUSSIAN,5,5);
	IplImage* imgTemp = cvCreateImage( img_sz, IPL_DEPTH_8U, 1 );//保存滤波后的
	IplImage* imgC = cvLoadImage("D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/71.bmp",CV_LOAD_IMAGE_UNCHANGED);//显示结果图
	int s=clock();
	//帧差处理
	cvAbsDiff(imgA,imgB,imgTemp);
	cvThreshold(imgTemp,imgTemp,15,255,CV_THRESH_BINARY);
	//找出比较大运动目标
	IplConvKernel *core = cvCreateStructuringElementEx(5,5,2,2,CV_SHAPE_RECT);//创建卷积核，参考点位于中心，坐标从0开始。
	cvDilate(imgTemp,imgTemp,core,1);//膨胀
	cvErode(imgTemp,imgTemp,core,1);
	cvDilate(imgTemp,imgTemp,core,3);//膨胀扩大目标
	cvReleaseStructuringElement(&core);
	//cvShowImage("ImageBinary",imgTemp);
	//e=clock();
	//画轮廓
	//IplImage *src = cvLoadImage("f:\\images\\test2.bmp",CV_LOAD_IMAGE_GRAYSCALE);  
	//CvMemStorage *storage = cvCreateMemStorage();  
	//CvSeq *seq = NULL;  
	//int cnt = cvFindContours(imgTemp,storage,&seq,sizeof(CvContour),CV_RETR_TREE); 
	////IplImage *dst = cvCreateImage(cvGetSize(imgTemp),8,3); cvZero(dst);  
	//while (seq )
	//{
	//	double length = cvArcLength(seq);  
	//	double area = cvContourArea(seq);  
	//	CvRect rect = cvBoundingRect(seq,1);  
	//	//CvBox2D box = cvMinAreaRect2(seq,NULL);  
	//	
	//	cout<<"Length = "<<length<<endl;  
	//	cout<<"Area = "<<area<<endl;  
	//	if (area>1000)
	//	{
	//		cvRectangleR(imgC,rect,CV_RGB(0,255,0));  
	//	}
	//	//cvDrawContours(dst,seq,CV_RGB(255,0,0),CV_RGB(255,0,0),0);  
	//	
	//	seq = seq->h_next;
	//}
	
	//求光流
	int sum=0,l_min=10000000,minM=-6,minN=-6;
	for (int i=7+16;i<img_sz.height-16;i+=16)//块16*16共44x36块
	{
		for (int j=7+16;j<img_sz.width-16;j+=16)
		{
			if (!is_MotionArea(imgTemp,j,i))
			{
				continue;
			}
			minM=-6,minN=-6,l_min=10000000;
			for (int m=-4;m<5;m++)//17个点
			{
				for (int n=-4;n<5;n++)
				{

					if (array9x9[m+4][n+4])
					{
						sum=0;
						for (int l=-7;l<=8;l++)//相减矩阵 求16x16矩阵差值
						{
							for (int k=-7;k<=8;k++)
							{
								sum+=abs(int(imgA->imageData[(i+m+l)*img_sz.width+j+n+k])-int(imgB->imageData[(i+l)*img_sz.width+j+k]));
							}
						}
						if (sum<l_min||(sum==l_min&&m==0&&n==0))
						{
							minM=m;
							minN=n;
							l_min=sum;

						}
					}
				}

			}
			//如果为17个点中心，说明无运动，则继续求下一组数据
			if ((minM==0&&minN==0)||l_min==10000000)
			{
				continue;
			}
			else 
				if (is_outRect(minM,minN))//如果为外围正方形点
				{
					int t_minM=-6,t_minN=-6,t_min=10000000;
					for (int m=-2;m<3;m+=2)//8个点 暂时求8个
					{
						for (int n=-2;n<3;n+=2)
						{

							if (array5x5[m+2][n+2])
							{
								sum=0;
								for (int l=-7;l<=8;l++)//相减矩阵 求16x16矩阵差值
								{
									for (int k=-7;k<=8;k++)
									{
										sum+=abs(int(imgA->imageData[(i+minM+m+l)*img_sz.width+j+minN+n+k])-int(imgB->imageData[(i+l)*img_sz.width+j+k]));
									}
								}
								if (sum<l_min)
								{
									t_minM=m;
									t_minN=n;
									l_min=sum;

								}
							}
						}
					}
					//如果为中心，无递归
					if (t_minM==-6&&t_minN==-6)
					{
						CvPoint p0 = cvPoint(
							cvRound(j ),
							cvRound( i )
							);
						CvPoint p1 = cvPoint(
							cvRound(j+minN  ),
							cvRound(i+minM  )
							);
						double t_amplitude = distants(p0.x,p0.y,p1.x,p1.y);//求距离
						g_amplitude.push_back(t_amplitude);
						double t_angle = Angle(p1.x,-p1.y,p0.x,-p0.y);//求角度，注意坐标表示
						//double t_angle = Angle(p0.x,-p0.y,p1.x,-p1.y);//求角度
						g_angle.push_back(t_angle);
						cvCircle(imgC,p0,3,CV_RGB(0,0,0),-1);
						cvLine( imgC, p0, p1, CV_RGB(255,0,0),2 ,CV_AA);
					}
					else
					{
						// //递归求3x3小正方形
						int flagz=1;
						while (flagz)
						{
							int tt_minM=-6,tt_minN=-6,tt_min=10000000;
							for (int m=-1;m<2;m++)//8个点 暂时求8个
							{
								for (int n=-1;n<2;n++)
								{

									if (array3x3Z1[m+1][n+1])
									{
										sum=0;
										for (int l=-7;l<=8;l++)//相减矩阵 求16x16矩阵差值
										{
											for (int k=-7;k<=8;k++)
											{
												sum+=abs(int(imgA->imageData[(i+minM+t_minM+m+l)*img_sz.width+j+minN+t_minN+n+k])-int(imgB->imageData[(i+l)*img_sz.width+j+k]));
											}
										}
										if (sum<l_min)
										{
											tt_minM=m;
											tt_minN=n;
											l_min=sum;

										}
									}
								}
							}
							if (tt_minM==-6&&tt_minN==-6)
							{
								CvPoint p0 = cvPoint(
									cvRound(j ),
									cvRound( i )
									);
								CvPoint p1 = cvPoint(
									cvRound( j+minN+t_minN ),
									cvRound( i+minM+t_minM )
									);
								double t_amplitude = distants(p0.x,p0.y,p1.x,p1.y);//求距离
								g_amplitude.push_back(t_amplitude);
								double t_angle = Angle(p1.x,-p1.y,p0.x,-p0.y);//求角度，注意坐标表示
								//double t_angle = Angle(p0.x,-p0.y,p1.x,-p1.y);//求角度
								g_angle.push_back(t_angle);
								cvCircle(imgC,p0,3,CV_RGB(0,0,0),-1);
								cvLine( imgC, p0, p1, CV_RGB(255,0,0),2 ,CV_AA);
								flagz=0;
							}
							else
							{
								t_minM+=tt_minM;
								t_minN+=tt_minN;
							}				

						}
					}
				}//最外层大正方形
				else//如果为菱形
				{
					int flagz=1;
					while (flagz)
					{
						int t_minM=-6,t_minN=-6,t_min=10000000;
						for (int m=-1;m<2;m++)
						{
							for (int n=-1;n<2;n++)
							{
								if (array3x3L1[m+1][n+1])
								{
									sum=0;
									for (int l=-7;l<=8;l++)//相减矩阵 求16x16矩阵差值
									{
										for (int k=-7;k<=8;k++)
										{
											sum+=abs(int(imgA->imageData[(i+m+minM+l)*img_sz.width+j+minN+n+k])-int(imgB->imageData[(i+l)*img_sz.width+j+k]));
										}
									}
									if (sum<l_min)
									{
										t_minM=m;
										t_minN=n;
										l_min=sum;

									}
								}
							}
						}
						//如果为中心，无递归
						if (t_minM==-6&&t_minN==-6)
						{
							CvPoint p0 = cvPoint(
								cvRound(j ),
								cvRound( i )
								);
							CvPoint p1 = cvPoint(
								cvRound( j+minN ),
								cvRound( i+minM )
								);
							double t_amplitude = distants(p0.x,p0.y,p1.x,p1.y);//求距离
							g_amplitude.push_back(t_amplitude);
							double t_angle = Angle(p1.x,-p1.y,p0.x,-p0.y);//求角度，注意坐标表示
							g_angle.push_back(t_angle);
							cvCircle(imgC,p0,3,CV_RGB(0,0,0),-1);//p0代表目标图像像素，所以在p0画圆点表箭头
							cvLine( imgC, p0, p1, CV_RGB(255,0,0),2 ,CV_AA);
							flagz=0;
						}
						else //暂时保存，需修改
						{
							minM+=t_minM;
							minN+=t_minN;
						}

					}

				}//菱形


		}//双重for循环
	}
	//画幅度直方图
	int bins_amplitude[500]={0};
	drawHistogram(histimg_Amplitude,g_amplitude,40,bins_amplitude,0,20);
	//画角度直方图
	int bins_angle[500]={0};
	drawHistogram(histimg_Angle,g_angle,60,bins_angle,0,360);






	int e=clock();
	std::cout<<e-s;



	cvShowImage("AmplitudeImage",histimg_Amplitude);
	cvShowImage("AngleImage",histimg_Angle);
	//cvShowImage("ImageA",imgA);
	//cvShowImage("ImageB",imgB);
	cvShowImage("LKpyr_OpticalFlow",imgC);
	cvWaitKey(0);
}

double distants(double x1,double y1,double x2,double y2)//根据两点求返回距离
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}
double Angle(double x1,double y1,double x2,double y2)//求返回角度 360度
{
	if (x1-x2==0&&y2-y1==0)
	{
		return 0;
	}
	double t_angle = atan2(y2-y1,x2-x1);
	if (t_angle>0)
	{
		return t_angle*180/3.141592654;
	}
	else
		return atan2(y2-y1,x2-x1)*180/3.141592654+360;


}
bool is_outRect(int m,int n)//是否是外围正方形
{
	return (m==-4&&n==-4)||(m==-4&&n==0)||(m==-4&&n==4)||(m==0&&n==-4)||(m==0&&n==4)||(m==4&&n==-4)||(m==4&&n==0)||(m==4&&n==4);
}
bool is_MotionArea(IplImage *src,int x,int y)//16*16区域，y是高度，x是宽度
{
	int sum=0;
	int t_nWidth = src->width;
	for (int i=-7;i<=8;i++)
	{
		for (int j=-7;j<=8;j++)
		{
			if (src->imageData[(y+i)*t_nWidth+j+x])
			{
				sum++;
			}
		}
	}
	if (sum>100)
	{
		return true;
	}
	return false;
}
void CalcHistogram(vector<double> VectorScr,int BinNum,int bins[],int max,int min)//计算直方图2
{

	double step = (max-min)/double(BinNum);
	for (unsigned int i=0;i<VectorScr.size();i++)//找最大最小值
	{
		int pos = (VectorScr[i]-min)/step;
		bins[pos]++;
	}


}
void drawHistogram(IplImage *dst,vector<double> vectordata,int binsNums,int bins[],int min,int max)
{
	CalcHistogram(vectordata,binsNums,bins,max,min);
	int binl_w = dst->width/binsNums;
	for( int i = 0; i < binsNums; i++ )
	{

		double val = ( double(bins[i])*dst->height/vectordata.size()/*double(ArrayMax(bins_amplitude,binNum_amplitude) )*/);//得到一维矩阵第i的高度
		CvScalar color = CV_RGB(255,255,0); //(hsv2rgb(i*180.f/hdims);
		cvRectangle( dst, cvPoint(i*binl_w,dst->height),
			cvPoint((i+1)*binl_w,(int)(dst->height - val)),//在目标图像上画矩形，左上到右下
			color, 1, 8, 0 );
	}

}
