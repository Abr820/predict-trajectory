#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include<bits/stdc++.h>

using namespace std;
using namespace cv;
#define For(x,v)            for(i=(v).begin();i<(v).end(); i++)
#define lli                 long long int
#define lld                 long double
#define it                  vector <lli>::iterator
#define FOR(i,x)            for(int i=0;i<(x); i++)
#define FOR1(j,x)            for(int j=1;j<=(x); j++)
#define Mod(a)          (((a)>0)? (a) : (-a))
#define pi       4*atan(1)

bool isvalid(int x, int y, int r, int c){
  return (x>=0 && y>=0 && x<r && y<c);
}

Mat threshh(Mat img, double fact, int n){
	int r=img.rows,c=img.cols;
	double sum=0,th;
	Mat thimg=img.clone();
	FOR(i,r-2*n){
		FOR(j,c-2*n){
			FOR(k,(2*n+1)*(2*n+1)){
				sum+=img.at<uchar>(i-n+(k/(2*n+1)),j-n+(k%(2*n+1)));
			}
			th=fact*(sum/((2*n+1)*(2*n+1)));
			if(img.at<uchar>(i+n,j+n)<th) thimg.at<uchar>(i+n,j+n)=0;
			sum=0;
		}
	}
	return thimg;
}

Mat draw(Point2f center, Point2f v0, Point2f g, Mat img){
  Point2f s,v,s2; int t,t2;
  for(t=0;t<150;t++){
    s=center+v0*t+0.5*g*(t*t);
    v=v0+g*t;
    if(isvalid(s.y,s.x,img.rows,img.cols)) 
      circle( img, s , 2, Scalar(255,2*t,2*t), 2, 8, 0 );
    if(s.y<=21 || s.y>=443) break;
  }
  v.y=-v.y;v.x-=(v.x/10);
  if(s.y>=443) {
    v.y-=(v.y/5);
    v.x-=(v.x/7);
  }
  for(t2=0;t2<150-t;t2++){
    s2=s+v*t2+0.5*g*(t2*t2);
    if(isvalid(s2.y,s2.x,img.rows,img.cols)) 
      circle( img, s2 , 2, Scalar(255,2*(t+t2),2*(t+t2)), 2, 8, 0 );
  }
  return img;
}

int main(){
  VideoCapture vid("footballip.mp4");
  if(!vid.isOpened()) cout<<"video not opened\n";
  Mat frame;
  Mat cons=imread("ipconst.jpeg",0);
  int r=cons.rows, c=cons.cols;
  //FOR(i,150) vid >> frame;
  //Mat velocity(400,4000,CV_8UC1,Scalar(0));
  //Mat accln(r,300,CV_8UC1,Scalar(0));
  long int count=0, t=3980;
  vector <Point2f> pt; vector <Point2f> vel(4); Point2f s,v,g(0,0.175);
  while(t--){
    vid >> frame;
    if(frame.empty())  return 0;
    Mat img;
    cvtColor(frame, img, COLOR_BGR2GRAY);
    //int th=0,rat;
    //namedWindow("image",CV_WINDOW_AUTOSIZE);
    //createTrackbar("threshold","image",&th,255);
    //createTrackbar("ratio","image",&rat,400);
    Mat thresh, blurr,canny_output,consthresh,diff;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    blur( img, blurr, Size(3,3) );
    //thresh=threshh(blurr,0.5,6); // thresh=150|81 type= 0 or 3
    threshold(blurr,thresh,150,255,3);
    threshold(cons,consthresh,150,255,0);
    //Canny(thresh,canny_output,160,150*2.5,3);
    diff=(thresh-0.4*consthresh);
    findContours( diff, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    vector<Point> contours_poly;
    Rect boundRect;
    Point2f center;
    float radius,len,brd;

    int flag=0;
    FOR(i,contours.size()){
       approxPolyDP( Mat(contours[i]), contours_poly, 3, true );
       boundRect = boundingRect( Mat(contours_poly) );
       minEnclosingCircle( (Mat)contours_poly, center, radius );
        //Mat drawing = Mat::zeros( thresh.size(), CV_8UC1 );
       //Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       //drawContours( frame, contours_poly, i, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );
      len =boundRect.br().x-boundRect.tl().x;
      brd =boundRect.br().y-boundRect.tl().y;
       if(radius>19.5 && radius<20.5 && (len/brd)<1.05 && (len/brd)>0.95 && Mod(len-brd)<=2) {
        rectangle( frame, boundRect.tl(), boundRect.br(), Scalar(0,0,255), 2, 8, 0 );
        flag=1;
        break;
       //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
        }
     }
     if(flag==0) {
      cout<<"ball not found"<<endl;
       count++;
       Point2f p=pt[3]+v;
       pt.push_back(p);
       if(!pt.empty())pt.erase(pt.begin());
     }
     else{
        //cout<<"radius"<<radius<<" ";
        //cout<<"len:"<<len<<"  br:"<<brd<<endl;
        cout<<"center:"<<center; 
        if(pt.size()>=4){
          pt.push_back(center);
          FOR(i,4){
            vel[i]=(pt[4]-pt[i])/(4-i);
          }
          pt.erase(pt.begin());

        }
        else pt.push_back(center);
        cout<<" v:"<<v<<endl;
        v=(vel[0]+vel[1]+vel[2]+vel[3])/4;
        frame=draw(center,v,g,frame);
        //velocity.at<uchar>(200+v.y,(4000-t))=255;
        //accln.at<uchar>(center.y,(260-t))=255;
     }
    imshow("video",frame); 
    //imshow("canny",diff);
    waitKey(15);
  }
  //frame.release();
  //imshow("vy vs t",velocity);
  //waitKey(100000);
}