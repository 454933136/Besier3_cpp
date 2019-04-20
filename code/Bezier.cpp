#include "Bezier.h"

bool  Comp( Point2f a,Point2f b)
{
     return a.y < b.y ;
}

Bezier::Bezier() 
{
       
};

vector<Point2f> Bezier::getSample(int n)//获取n个样点
{
    vector <Point2f> ret;
    int i, k;
    vector <int> order;
    srand((unsigned)time(NULL));
    for (i = 0; i < n; i++)
    {
        float r = rand() % 10;
        order.push_back(r) ;
    } 
    sort(order.begin() , order.end());
    
    for (i = 0; i < n; i++)
    {
        ret.push_back(total_points[order[i]]);
    }
        
    return ret;
 }
//  

void Bezier::addPoint(Point p) 
{
    total_points.push_back(Point2f(p.x, p.y)) ;
}



void Bezier::getT(Mat& _T, float t) 
{
    float dat[1][4] = {t * t * t, t * t, t, 1};
    Mat T(1, 4, CV_32F, dat);
    T.copyTo(_T);
}

void Bezier::getM(Mat& _M)
{
    float datM[4][4] = {{-1, 3, -3, 1}, {3, -6, 3, 0}, {-3, 3, 0, 0}, {1, 0, 0, 0}};
    Mat M(4, 4, CV_32F, datM);
    M.copyTo(_M);
}

void Bezier::getP(Mat& _P, vector<Point2f> ps)
{
    unsigned int i;
    float datP[ps.size()][2];
    
    assert(ps.size() == 4);
    
    for (i = 0; i < ps.size(); i++) 
    {
        Point2f p = ps.at(i);
        datP[i][0] = p.x;
        datP[i][1] = p.y;
    }
    Mat P(ps.size(), 2, CV_32F, datP);
    
    P.copyTo(_P);
}

vector<Point2f> Bezier::fitSpline(vector<Point2f> ps) 
{
   // cout<<"ps:"<<ps.size()<<endl;
    vector<Point2f> pc;
    float t[ps.size()];//大小为样本点的数量，为随机取得n个
    float tA = 0, tB = 0;
    unsigned int i;
    int k;

    assert(ps.size() >= 4);
    
    /// 计算 t_{i}
    t[0] = 0;
    t[ps.size() - 1] = 1;//头尾为1
    
    ///  t_{i} = \frac{tA}{tB}
    
    for (i = 1; i < ps.size(); i++) 
    {
        Point2f p2, p1;
        float d;
        
        p2 = ps.at(i);
        p1 = ps.at(i - 1);
        d = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
        
        tB += d;    
    }
    
    for (i = 1; i < ps.size() - 1; i++) 
    {
        Point2f p2, p1;
        float d;
        
        p2 = ps.at(i);
        p1 = ps.at(i - 1);
        d = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
        
        tA += d;
        t[i] = tA / tB;
    }
        
    
    /// 构建矩阵 T, M, Q
    Mat M;
    getM(M);//4行4列
   // cout << M<<endl;
    Mat T(ps.size(), 4, CV_32F, Scalar::all(1));//100行4列
    for (i = 0; i < ps.size(); i++) 
    {
        float tt = t[i];
        T.at<float>(i, 2) = tt;
        tt *= t[i];
        T.at<float>(i, 1) = tt;
        tt *= t[i];
        T.at<float>(i, 0) = tt;
    }

    //cout<<  T  <<endl;
 
    float datQ[ps.size()][2];//100行2列
    for (i = 0; i < ps.size(); i++) 
    {
        Point2f p = ps.at(i);
        datQ[i][0] = p.x;
        datQ[i][1] = p.y;
    }
    Mat Q(ps.size(), 2, CV_32F, datQ);
    
    Mat P = (T * M).inv(DECOMP_SVD) * Q;
    // Mat P = (T * M).inv() * Q;    
    for (k = 0; k < P.rows; k++) 
    {
        pc.push_back( Point2f(P.at<float>(k, 0) , P.at<float>(k, 1) ));               
    }
    //cout<<"pc:  "<<pc.size()  <<endl;
    return pc;
}

float Bezier::computeScore(vector<Point2f> spline, Mat _image,int Threshold) 
{

    float score = 0;

    float x = 0, y = 0;
    int xx, yy;
    float t3, t2, t1, t;

    /// 计算原始分数
   // for (t = 0; t <= 1; t += 0.01) 
    for (t = 0; t <= 1; t += 0.001)
    {

        t1 = t;
        t2 = t1 * t;
        t3 = t2 * t;

        x += (-1 * t3 + 3 * t2 - 3 * t1 + 1) * spline.at(0).x;
        x += (3 * t3 - 6 * t2 + 3 * t1) * spline.at(1).x;
        x += (-3 * t3 + 3 * t2) * spline.at(2).x;
        x += t3 * spline.at(3).x;

        y += (-1 * t3 + 3 * t2 - 3 * t1 + 1) * spline.at(0).y;
        y += (3 * t3 - 6 * t2 + 3 * t1) * spline.at(1).y;
        y += (-3 * t3 + 3 * t2) * spline.at(2).y;
        y += t3 * spline.at(3).y;

       xx = round(x);
       yy = round(y);
       //cout<< xx<<" "<<yy<<endl;
       if (xx > 0 && xx < _image.cols && yy > 0&& yy < _image.rows) 
       {
           for(int a=-Threshold;a<Threshold;a++)//这里的10是左右距离为10的相关点区域
           {
                score+= _image.at<uchar>(yy,xx+a);
           }

       }
       x=0;
       y=0;
      //cout<<"score:"<<score<<endl;    
    }       

return score;
}

Point  Bezier::getPoint(float t, vector<Point2f> spline) 
{
    float x = 0, y = 0;
    float t3, t2, t1;
    
    t1 = t;
    t2 = t1 * t;
    t3 = t2 * t;
    
    x += (-1 * t3 + 3 * t2 - 3 * t1 + 1) * spline.at(0).x;
    x += (3 * t3 - 6 * t2 + 3 * t1) * spline.at(1).x;
    x += (-3 * t3 + 3 * t2) * spline.at(2).x;
    x += t3 * spline.at(3).x;
    
    y += (-1 * t3 + 3 * t2 - 3 * t1 + 1) * spline.at(0).y;
    y += (3 * t3 - 6 * t2 + 3 * t1) * spline.at(1).y;
    y += (-3 * t3 + 3 * t2) * spline.at(2).y;
    y += t3 * spline.at(3).y;

    //cout<<x<<":"<<y<<endl;
    return Point(x, y);
}


//RANSIC的筛选
vector<Point2f> Bezier::fit(int _iterNum, Mat _image,int Threshold) 
{
    int i;
    vector<Point2f> spline, bestSpline;
    float score, bestScore = -1;
    vector <Point2f> samples;
    vector <Point2f> samples1;
    for (i = 0; i < _iterNum; i++) 
    {
       samples= getSample(5);//这个入口函数就实现了_iterNum次迭代每次都会产生不完全相同的随机数。
       // cout<<"sample:   "<<samples[20]<<","<<samples[9]<<":"<<i <<endl;
        if (samples.size() < 4) 
        {
            continue;
        }

        spline = fitSpline(samples);
      
        cout<<"spline: "<<spline.size() <<endl;
        cout<<spline.at(1)<<endl;
        sort(spline.begin()  ,spline.end() , Comp);
        //得分函数可以自己改成更好的.我考虑到速度,没有用欧式距离的方法
        score = computeScore(spline, _image,Threshold);

        if (score > bestScore) 
        {
            bestSpline = spline;
            bestScore = score;
            swap(samples1,samples);
        }
        vector<Point2f>().swap(samples); //将空间大小也进行初始化
    }  

    return bestSpline;   
}

//将4个点按照坐标顺序排序,(左下,左上,右上,右下)
// vector<Point2f> Bezier::order(vector<Point2f> spline)
// {

//     vector<Point2f> spline_order
//     Point tmp
//     int spline.at(0)
// }