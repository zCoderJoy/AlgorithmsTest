//���Զ��ͼƬ
#include <iostream>
#include "cv.h"
#include "highgui.h"
using namespace std;
//ȫ�ֲ���
const int AmplitudeBinNumbs=40;//����ֱ��ͼ�и���
const int AngleBinNumbs=60;//�Ƕ�ֱ��ͼ�и���
vector<double> g_angle;//�Ƕ�
vector<double> g_amplitude;//����
int array9x9[9][9]={0};//��17��
int array5x5[5][5]={0};//С8��
int array3x3L1[3][3]={0};//����8��
int array3x3Z1[3][3]={0};//������8��
int bins_angle_pre[500]={0};//�洢֮ǰ�Ƕ�ֱ��ͼ����
int bins_amplitude_pre[500]={0};//�洢֮ǰ����ֱ��ͼ����
double distants(double x1,double y1,double x2,double y2);//���������󷵻ؾ���
double Angle(double x1,double y1,double x2,double y2);//�󷵻ؽǶ� 360��
bool is_outRect(int m,int n);//�Ƿ�����Χ������
bool is_MotionArea(IplImage *src,int x,int y);//16*16����y�Ǹ߶ȣ�x�ǿ��
void CalcHistogram(vector<double> VectorScr,int BinNum,int bins[],int max,int min);//����ֱ��ͼ2
void drawHistogram(IplImage *dst,vector<double> vectordata,int binsNums,int bins[],int min,int max);//��ֱ��ͼ
void DrawSubHistogram(IplImage *dst,int binsNums,int binsCur[],int binsPre[]);//�����ֱ��ͼ
bool isFighting(vector<double> vectorAmplitude,vector<double> vectorAngle,int binsAmpl[],int binsAngl[]);//�Ƿ��Ǵ�ܶ�Ź,����ʸ����������30
//������
int main()
{
	//9X9��ʼ��
	array9x9[0][0]=1;array9x9[0][4]=1;array9x9[0][8]=1;array9x9[2][4]=1;
	array9x9[3][3]=1;array9x9[3][5]=1;array9x9[4][0]=1;array9x9[4][2]=1;
	array9x9[4][4]=1;array9x9[4][6]=1;array9x9[4][8]=1;array9x9[5][3]=1;
	array9x9[5][5]=1;array9x9[6][4]=1;array9x9[8][0]=1;array9x9[8][4]=1;
	array9x9[8][8]=1;
	//5x5
	array5x5[0][0]=1;array5x5[0][2]=1;array5x5[0][4]=1;array5x5[2][0]=1;
	array5x5[2][4]=1;array5x5[4][0]=1;array5x5[4][2]=1;array5x5[4][4]=1;
	//3x3����
	array3x3L1[0][1]=1;array3x3L1[1][0]=1;array3x3L1[1][2]=1;array3x3L1[2][1]=1;
	//3x3������
	array3x3Z1[0][0]=1;array3x3Z1[1][0]=1;array3x3Z1[1][2]=1;array3x3Z1[2][1]=1;
	array3x3Z1[2][2]=1;array3x3Z1[0][1]=1;array3x3Z1[0][2]=1;array3x3Z1[2][0]=1;
	IplImage* imgA = cvLoadImage("D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/120.bmp",CV_LOAD_IMAGE_GRAYSCALE);//����ͼ��
	IplImage* imgB;// = cvLoadImage("D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/71.bmp",CV_LOAD_IMAGE_GRAYSCALE);
	CvSize  img_sz = cvGetSize( imgA );//�õ�ͼ���С
	IplImage* histimg_Amplitude = cvCreateImage( cvSize(320,200), 8, 3 );//����ֱ��ͼͳ��
	IplImage* histimg_Angle = cvCreateImage( cvSize(320,200), 8, 3 );//�Ƕ�ֱ��ͼͳ��
	IplImage* histimg_Angle_sub = cvCreateImage( cvSize(320,200), 8, 3 );//�Ƕ�ֱ��ͼͳ��
	IplImage* histimg_Amplitude_sub = cvCreateImage( cvSize(320,200), 8, 3 );//�Ƕ�ֱ��ͼͳ��
	cvZero( histimg_Amplitude );
	cvZero( histimg_Angle );

	IplImage* imgTemp = cvCreateImage( img_sz, IPL_DEPTH_8U, 1 );//�����˲����
	IplImage* imgC ;//= cvLoadImage("D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/71.bmp",CV_LOAD_IMAGE_UNCHANGED);//��ʾ���ͼ
	string fname = "D:/VS2012/data/Videos/2014-07-16Fighting/Channel2CutPictures/";

	//�����֡ͼ��
	for (int ii=120;ii<4030;ii++)////###################���ö��ͼƬ���в���##########################$$$$$$$$$$$$$$$
	{
		//cout<<"���ڴ���� "<< i<<"֡��\n";
		string filename="";
		char tmp[20];
		_itoa_s(ii,tmp,20,10);
		filename+=tmp;
		filename+=".bmp";
		filename=fname+filename;
		imgB = cvLoadImage(filename.c_str(),CV_LOAD_IMAGE_GRAYSCALE);
		imgC = cvLoadImage(filename.c_str(),CV_LOAD_IMAGE_UNCHANGED);
		if (!imgB)
		{
			cout<<"��ȡͼ����ȷ��";
			break;
		}
		//cvSmooth(imgA,imgA,CV_GAUSSIAN,5,5);//��˹ƽ��3*3
		//cvSmooth(imgB,imgB,CV_GAUSSIAN,5,5);
		
		int s=clock();
		//֡���
		cvAbsDiff(imgA,imgB,imgTemp);
		cvThreshold(imgTemp,imgTemp,15,255,CV_THRESH_BINARY);
		//�ҳ��Ƚϴ��˶�Ŀ��
		IplConvKernel *core = cvCreateStructuringElementEx(5,5,2,2,CV_SHAPE_RECT);//��������ˣ��ο���λ�����ģ������0��ʼ��
		cvDilate(imgTemp,imgTemp,core,1);//����
		cvErode(imgTemp,imgTemp,core,1);
		cvDilate(imgTemp,imgTemp,core,3);//��������Ŀ��
		cvReleaseStructuringElement(&core);
		//cvShowImage("ImageBinary",imgTemp);
		//e=clock();
		//������
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

		//���˶�ʸ��
		int sum=0,l_min=10000000,minM=-6,minN=-6;
		for (int i=7+16;i<img_sz.height-16;i+=16)//��16*16��44x36��
		{
			for (int j=7+16;j<img_sz.width-16;j+=16)
			{
				if (!is_MotionArea(imgTemp,j,i))//�ж��Ƿ���ǰ������
				{
					continue;
				}
				minM=-6,minN=-6,l_min=10000000;
				for (int m=-4;m<5;m++)//17����
				{
					for (int n=-4;n<5;n++)
					{

						if (array9x9[m+4][n+4])
						{
							sum=0;
							for (int l=-7;l<=8;l++)//������� ��16x16�����ֵ
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
				//���Ϊ17�������ģ�˵�����˶������������һ������
				if ((minM==0&&minN==0)||l_min==10000000)
				{
					continue;
				}
				else 
					if (is_outRect(minM,minN))//���Ϊ��Χ�����ε�
					{
						int t_minM=-6,t_minN=-6,t_min=10000000;
						for (int m=-2;m<3;m+=2)//8���� ��ʱ��8��
						{
							for (int n=-2;n<3;n+=2)
							{

								if (array5x5[m+2][n+2])
								{
									sum=0;
									for (int l=-7;l<=8;l++)//������� ��16x16�����ֵ
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
						//���Ϊ���ģ��޵ݹ�
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
							double t_amplitude = distants(p0.x,p0.y,p1.x,p1.y);//�����
							g_amplitude.push_back(t_amplitude);
							double t_angle = Angle(p1.x,-p1.y,p0.x,-p0.y);//��Ƕȣ�ע�������ʾ
							//double t_angle = Angle(p0.x,p0.y,p1.x,p1.y);//��Ƕ�
							g_angle.push_back(t_angle);
							cvCircle(imgC,p0,3,CV_RGB(0,0,0),-1);//p0����Ŀ��ͼ�����أ�������p0��Բ����ͷ
							cvLine( imgC, p1, p0, CV_RGB(255,0,0),2 ,CV_AA);
						}
						else
						{
							// //�ݹ���3x3С������
							int flagz=1;
							while (flagz)
							{
								int tt_minM=-6,tt_minN=-6,tt_min=10000000;
								for (int m=-1;m<2;m++)//8���� ��ʱ��8��
								{
									for (int n=-1;n<2;n++)
									{

										if (array3x3Z1[m+1][n+1])
										{
											sum=0;
											for (int l=-7;l<=8;l++)//������� ��16x16�����ֵ
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
									double t_amplitude = distants(p0.x,p0.y,p1.x,p1.y);//�����
									g_amplitude.push_back(t_amplitude);
									double t_angle = Angle(p1.x,-p1.y,p0.x,-p0.y);//��Ƕȣ�ע�������ʾ
									//double t_angle = Angle(p0.x,p0.y,p1.x,p1.y);//��Ƕ�
									g_angle.push_back(t_angle);
									cvCircle(imgC,p0,3,CV_RGB(0,0,0),-1);//p0����Ŀ��ͼ�����أ�������p0��Բ����ͷ
									cvLine( imgC, p1, p0, CV_RGB(255,0,0),2 ,CV_AA);
									flagz=0;
								}
								else
								{
									t_minM+=tt_minM;
									t_minN+=tt_minN;
								}				

							}
						}
					}//������������
					else//���Ϊ����
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
										for (int l=-7;l<=8;l++)//������� ��16x16�����ֵ
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
							//���Ϊ���ģ��޵ݹ�
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
								double t_amplitude = distants(p0.x,p0.y,p1.x,p1.y);//�����
								g_amplitude.push_back(t_amplitude);
								double t_angle = Angle(p1.x,-p1.y,p0.x,-p0.y);//��Ƕȣ�ע�������ʾ
								//double t_angle = Angle(p0.x,p0.y,p1.x,p1.y);//��Ƕ�
								g_angle.push_back(t_angle);
								cvCircle(imgC,p0,3,CV_RGB(0,0,0),-1);//p0����Ŀ��ͼ�����أ�������p0��Բ����ͷ
								cvLine( imgC, p1, p0, CV_RGB(255,0,0),2 ,CV_AA);//p1->p0
								flagz=0;
							}
							else //��ʱ���棬���޸�
							{
								minM+=t_minM;
								minN+=t_minN;
							}

						}

					}//����


			}//˫��forѭ��
		}
		//������ֱ��ͼ
		int bins_amplitude[500]={0};
		drawHistogram(histimg_Amplitude,g_amplitude,AmplitudeBinNumbs,bins_amplitude,0,20);
		//���Ƕ�ֱ��ͼ
		int bins_angle[500]={0};
		drawHistogram(histimg_Angle,g_angle,AngleBinNumbs,bins_angle,0,360);
		//���������ֱ��ͼ
		DrawSubHistogram(histimg_Amplitude_sub,AmplitudeBinNumbs,bins_amplitude,bins_amplitude_pre);
		//������Ƕ�ֱ��ͼ
		DrawSubHistogram(histimg_Angle_sub,AngleBinNumbs,bins_angle,bins_angle_pre);
		int e=clock();
		std::cout<<e-s<<"\n";
		//��ܶ�Ź�ж�
		if (g_angle.size()>30)
		{
			if (isFighting(g_amplitude,g_angle,bins_amplitude,bins_angle))
			{
				cout<<"���ڴ���� "<< ii<<"֡��\n";
				cout<<"�����ܶ�Ź����������";
			}
			

		}


		cvShowImage("AmplitudeImage",histimg_Amplitude);//չʾ����ֱ��ͼ
		cvShowImage("AngleImage",histimg_Angle);
		cvShowImage("����ֱ��ͼ֮��",histimg_Amplitude_sub);
		cvShowImage("�Ƕ�ֱ��ͼ֮��",histimg_Angle_sub);
		//cvShowImage("ImageA",imgA);
		//cvShowImage("ImageB",imgB);
		cvShowImage("LKpyr_OpticalFlow",imgC);
		cvZero(histimg_Angle);
		cvZero(histimg_Amplitude);
		cvZero(histimg_Angle_sub);
		cvZero(histimg_Amplitude_sub);
		cvWaitKey(1);
		swap(imgA,imgB);
		memcpy(bins_angle_pre,bins_angle,sizeof(bins_angle));//����֮ǰֱ��ͼ
		memcpy(bins_amplitude_pre,bins_amplitude,sizeof(bins_amplitude));
		//swap(g_amplitude_pre,g_amplitude);
		//swap(g_angle_pre,g_angle);
		g_angle.clear();
		//int a=g_angle.capacity();
		vector<double>(g_angle).swap(g_angle);
		//a=g_amplitude.capacity();
		g_amplitude.clear();
		//a=g_amplitude.capacity();
		vector<double>(g_amplitude).swap(g_amplitude);
		cvReleaseImage(&imgB);
		cvReleaseImage(&imgC);
	}
	
	
	
	cvWaitKey(0);
}

double distants(double x1,double y1,double x2,double y2)//���������󷵻ؾ���
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}
double Angle(double x1,double y1,double x2,double y2)//�󷵻ؽǶ� 360��
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
bool is_outRect(int m,int n)//�Ƿ�����Χ������
{
	return (m==-4&&n==-4)||(m==-4&&n==0)||(m==-4&&n==4)||(m==0&&n==-4)||(m==0&&n==4)||(m==4&&n==-4)||(m==4&&n==0)||(m==4&&n==4);
}
bool is_MotionArea(IplImage *src,int x,int y)//16*16����y�Ǹ߶ȣ�x�ǿ��
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
void CalcHistogram(vector<double> VectorScr,int BinNum,int bins[],int max,int min)//����ֱ��ͼ2
{

	double step = (max-min)/double(BinNum);
	for (unsigned int i=0;i<VectorScr.size();i++)//�������Сֵ
	{
		int pos = (VectorScr[i]-min)/step;
		bins[pos]++;
	}


}
void drawHistogram(IplImage *dst,vector<double> vectordata,int binsNums,int bins[],int min,int max)
{
	CalcHistogram(vectordata,binsNums,bins,max,min);
	int binl_w = dst->width/binsNums;
	if (vectordata.size()==0)
	{
		return;
	}
	for( int i = 0; i < binsNums; i++ )
	{

		double val = ( double(bins[i])*dst->height/vectordata.size()/*double(ArrayMax(bins_amplitude,binNum_amplitude) )*/);//�õ�һά�����i�ĸ߶�
		CvScalar color = CV_RGB(255,255,0); //(hsv2rgb(i*180.f/hdims);
		cvRectangle( dst, cvPoint(i*binl_w,dst->height),
			cvPoint((i+1)*binl_w,(int)(dst->height - val)),//��Ŀ��ͼ���ϻ����Σ����ϵ�����
			color, 1, 8, 0 );
	}
	return;
}
void DrawSubHistogram(IplImage *dst,int binsNums,int binsCur[],int binsPre[])//�����ֱ��ͼ
{
	int bins_w = dst->width/binsNums;
	for( int i = 0; i < binsNums; i++ )
	{
		double val = ( (binsCur[i]-binsPre[i])*dst->height/50.0 );//�õ�һά�����i�ĸ߶�
		CvScalar color = CV_RGB(255,255,0); //(hsv2rgb(i*180.f/hdims);
		cvRectangle( dst, cvPoint(i*bins_w,dst->height/2),
			cvPoint((i+1)*bins_w,(int)(dst->height/2 - val)),//��Ŀ��ͼ���ϻ����Σ����ϵ�����
			color, 1, 8, 0 );
	}	

}
bool isFighting(vector<double> vectorAmplitude,vector<double> vectorAngle,int binsAmpl[],int binsAngl[])//�Ƿ��Ǵ�ܶ�Ź,����ʸ����������30
{
	//����ȵ�ƽ��ֵ����������10��ֵͳ��
	int Nums=vectorAngle.size();
	double sumAmplitude=0,sumAngle=0,averageAmpl=0,averageAngl=0,distantsNumbs=0;
	double averageHistAngl=0,sumHistAngl=0;
	for (int i=0;i<Nums;i++)
	{
		sumAmplitude+=vectorAmplitude[i];
		sumAngle+=vectorAngle[i];
		if (vectorAmplitude[i]>10)
		{
			distantsNumbs++;
		}
	}
	averageAmpl=sumAmplitude/Nums;//��÷���ƽ��ֵ
	averageAngl=sumAngle/Nums;//��Ƕ�ƽ��ֵ
	//��ֱ��ͼ�Ƕȵķ���
	for (int i=0;i<AngleBinNumbs;i++)
	{
		sumHistAngl+=binsAngl[i];


	}
	averageHistAngl=sumHistAngl/AngleBinNumbs;
	double deta=0;
	for (int i=0;i<AngleBinNumbs;i++)
	{
		deta+=(binsAngl[i]-averageHistAngl)*(binsAngl[i]-averageHistAngl);

		
	}
	double stDeta=deta/AngleBinNumbs;
	//�ж���ܶ�Ź,��������
	if (distantsNumbs>Nums/10&&averageAmpl>=5&&stDeta>10)
	{
		return true;
	}
	return false;
}