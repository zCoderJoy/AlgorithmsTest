//测试三步搜索块运动求矢量法
#include "cv.h"
#include "highgui.h"
#include <iostream>
int array9x9[9][9]={0};
int array5x5[5][5]={0};//小8点
int array3x3L1[3][3]={0};//菱形8点
int array3x3Z1[3][3]={0};//正方形8点
bool is_outRect(int m,int n)
{
	return (m==-4&&n==-4)||(m==-4&&n==0)||(m==-4&&n==4)||(m==0&&n==-4)||(m==0&&n==4)||(m==4&&n==-4)||(m==4&&n==0)||(m==4&&n==4);
}
void main()
{
	array9x9[0][0]=1;array9x9[0][4]=1;array9x9[0][8]=1;array9x9[2][4]=1;
	array9x9[3][3]=1;array9x9[3][5]=1;array9x9[4][0]=1;array9x9[4][2]=1;
	array9x9[4][4]=1;array9x9[4][6]=1;array9x9[4][8]=1;array9x9[5][3]=1;
	array9x9[5][5]=1;array9x9[6][4]=1;array9x9[8][0]=1;array9x9[8][4]=1;array9x9[8][8]=1;
	//5x5
	array5x5[0][0]=1;array5x5[0][2]=1;array5x5[0][4]=1;array5x5[2][0]=1;
	array5x5[2][4]=1;array5x5[4][0]=1;array5x5[4][2]=1;array5x5[4][4]=1;
	//3x3菱形
	array3x3L1[0][1]=1;array3x3L1[1][0]=1;array3x3L1[1][2]=1;array3x3L1[2][1]=1;
	//3x3正方形
	array3x3Z1[0][0]=1;array3x3Z1[1][0]=1;array3x3Z1[1][2]=1;array3x3Z1[2][1]=1;
	array3x3Z1[2][2]=1;array3x3Z1[0][1]=1;array3x3Z1[0][2]=1;array3x3Z1[2][0]=1;
	IplImage* imgA = cvLoadImage("D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/2380.bmp",CV_LOAD_IMAGE_GRAYSCALE);//加载图像
	IplImage* imgB = cvLoadImage("D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/2381.bmp",CV_LOAD_IMAGE_GRAYSCALE);
	CvSize  img_sz = cvGetSize( imgA );//得到图像大小
	//IplImage* histimg_Amplitude = cvCreateImage( cvSize(320,200), 8, 3 );//幅度直方图统计
	//cvSmooth(imgA,imgA,CV_GAUSSIAN,5,5);//高斯平滑3*3
	//cvSmooth(imgB,imgB,CV_GAUSSIAN,5,5);
	IplImage* imgC = cvLoadImage("D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/2381.bmp",CV_LOAD_IMAGE_UNCHANGED);//显示结果图
	//三步搜索法求运动矢量代码
	int s=clock();
	int sum=0,l_min=10000000,minM=-6,minN=-6;
	for (int i=7+16;i<img_sz.height-16;i+=16)//块16*16共44x36块
	{
		for (int j=7+16;j<img_sz.width-16;j+=16)
		{
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
						cvCircle(imgC,p0,3,CV_RGB(0,0,0),-1);
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




	int e=clock();
	std::cout<<e-s;



	
	cvShowImage("ImageA",imgA);
	cvShowImage("ImageB",imgB);
	cvShowImage("LKpyr_OpticalFlow",imgC);
	cvWaitKey(0);
	//system("pause");

};