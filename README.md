

# 第10周上机实验的任务如下：

### 1. 利用opencv和Zbar（或者Zxing）对标准的条形码图片（即没有多余背景干扰，且图片没有倾斜）进行解码，将解码信息显示出来，并与原始信息对比。

- 代码

```c++
#include <opencv2/opencv.hpp>
#include <zbar.h>

using namespace std;
using namespace zbar;  //添加zbar名称空间
using namespace cv;

int main(int argc,char*argv[])
{
    ImageScanner scanner;
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    Mat image = imread("bar_code.png");
    Mat imageGray;
    cvtColor(image,imageGray,CV_RGB2GRAY);
    int width = imageGray.cols;
    int height = imageGray.rows;
    auto *raw = imageGray.data;
    Image imageZbar(static_cast<unsigned int>(width), static_cast<unsigned int>(height), "Y800", raw,
                    static_cast<unsigned long>(width * height));
    scanner.scan(imageZbar); //扫描条码
    Image::SymbolIterator symbol = imageZbar.symbol_begin();
    if(imageZbar.symbol_begin()==imageZbar.symbol_end())
    {
        cout<<"查询条码失败，请检查图片！"<<endl;
    }
    for(;symbol != imageZbar.symbol_end();++symbol)
    {
        cout<<"类型："<<endl<<symbol->get_type_name()<<endl<<endl;
        cout<<"条码："<<endl<<symbol->get_data()<<endl<<endl;
    }
    imshow("Source Image",image);
    waitKey();
    imageZbar.set_data(nullptr,0);
    return 0;
}
```

- 结果

<img src="https://ws1.sinaimg.cn/large/006tKfTcgy1fr4fueodphj30my0aqgua.jpg" width="350"/>

![](https://ws1.sinaimg.cn/large/006tKfTcgy1fr4ft8v4jmj314m07edgk.jpg)

### 2. 利用opencv和Zbar（或者Zxing）对标准的QR二维码图片（即没有多余背景干扰，且图片没有倾斜）进行解码，将解码信息显示出来，并与原始信息对比。

```c++
// 更改第一问中的代码：
Mat image = imread("QR_code.png");
```

<img src="https://ws3.sinaimg.cn/large/006tKfTcgy1fr4g02v8l0j30cs0csmxg.jpg" width="200"/>

![](https://ws1.sinaimg.cn/large/006tKfTcgy1fr54yf0252j30ni06aq3b.jpg)

### 3. 对非标准条形码，QR二维码图片，进行定位，然后用Zbar（或者Zxing）解码显示。

- 代码

```c++
#include <opencv2/opencv.hpp>
#include <zbar.h>

using namespace std;
using namespace zbar;  //添加zbar名称空间
using namespace cv;


Mat src;
Mat src_gray;


RNG rng(12345);
//Scalar colorful = CV_RGB(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));

//获取轮廓的中心点
Point Center_cal(vector<vector<Point> > contours, int i) {
    int centerx = 0, centery = 0, n = static_cast<int>(contours[i].size());
    //在提取的小正方形的边界上每隔周长个像素提取一个点的坐标，
    //求所提取四个点的平均坐标（即为小正方形的大致中心）
    centerx = (contours[i][n / 4].x + contours[i][n * 2 / 4].x + contours[i][3 * n / 4].x + contours[i][n - 1].x) / 4;
    centery = (contours[i][n / 4].y + contours[i][n * 2 / 4].y + contours[i][3 * n / 4].y + contours[i][n - 1].y) / 4;
    Point point1 = Point(centerx, centery);
    return point1;
}

void QRCode_Recognize(String s){
    ImageScanner scanner;
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
    Mat image = imread(s);
    Mat imageGray;
    cvtColor(image,imageGray,CV_RGB2GRAY);
    int width = imageGray.cols;
    int height = imageGray.rows;
    auto *raw = imageGray.data;
    Image imageZbar(static_cast<unsigned int>(width), static_cast<unsigned int>(height), "Y800", raw,
                    static_cast<unsigned long>(width * height));
    scanner.scan(imageZbar); //扫描条码
    Image::SymbolIterator symbol = imageZbar.symbol_begin();
    if(imageZbar.symbol_begin()==imageZbar.symbol_end())
    {
        cout<<"查询条码失败，请检查图片！"<<endl;
    }
    for(;symbol != imageZbar.symbol_end();++symbol)
    {
        cout<<"类型："<<endl<<symbol->get_type_name()<<endl<<endl;
        cout<<"条码："<<endl<<symbol->get_data()<<endl<<endl;
    }
    imshow("Source Image",image);
    waitKey();
    imageZbar.set_data(nullptr,0);
}
int main(int argc, char *argv[]) {
    src = imread("QRCode_locate.png", 1);
    Mat src_all = src.clone();


    //彩色图转灰度图
    cvtColor(src, src_gray, CV_BGR2GRAY);
    //对图像进行平滑处理
    blur(src_gray, src_gray, Size(3, 3));
    //使灰度图象直方图均衡化
    equalizeHist(src_gray, src_gray);
    namedWindow("src_gray");
    imshow("src_gray", src_gray);


    Scalar color = Scalar(1, 1, 255);
    Mat threshold_output;
    vector<vector<Point> > contours, contours2;
    vector<Vec4i> hierarchy;
    Mat drawing = Mat::zeros(src.size(), CV_8UC3);
    Mat drawing2 = Mat::zeros(src.size(), CV_8UC3);
    Mat drawingAllContours = Mat::zeros(src.size(), CV_8UC3);

    //指定112阀值进行二值化
    threshold(src_gray, threshold_output, 112, 255, THRESH_BINARY);

    namedWindow("Threshold_output");
    imshow("Threshold_output", threshold_output);


    /*查找轮廓
     *  参数说明
        输入图像image必须为一个2值单通道图像
        contours参数为检测的轮廓数组，每一个轮廓用一个point类型的vector表示
        hiararchy参数和轮廓个数相同，每个轮廓contours[ i ]对应4个hierarchy元素hierarchy[ i ][ 0 ] ~hierarchy[ i ][ 3 ]，
            分别表示后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号，如果没有对应项，该值设置为负数。
        mode表示轮廓的检索模式
            CV_RETR_EXTERNAL 表示只检测外轮廓
            CV_RETR_LIST 检测的轮廓不建立等级关系
            CV_RETR_CCOMP 建立两个等级的轮廓，上面的一层为外边界，里面的一层为内孔的边界信息。如果内孔内还有一个连通物体，这个物体的边界也在顶层。
            CV_RETR_TREE 建立一个等级树结构的轮廓。具体参考contours.c这个demo
        method为轮廓的近似办法
            CV_CHAIN_APPROX_NONE 存储所有的轮廓点，相邻的两个点的像素位置差不超过1，即max（abs（x1-x2），abs（y2-y1））==1
            CV_CHAIN_APPROX_SIMPLE 压缩水平方向，垂直方向，对角线方向的元素，只保留该方向的终点坐标，例如一个矩形轮廓只需4个点来保存轮廓信息
            CV_CHAIN_APPROX_TC89_L1，CV_CHAIN_APPROX_TC89_KCOS 使用teh-Chinl chain 近似算法
        offset表示代表轮廓点的偏移量，可以设置为任意值。对ROI图像中找出的轮廓，并要在整个图像中进行分析时，这个参数还是很有用的。
     */
    findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

    int c = 0, ic = 0, k = 0, area = 0;

    //通过黑色定位角作为父轮廓，有两个子轮廓的特点，筛选出三个定位角
    int parentIdx = -1;
    for (int i = 0; i < contours.size(); i++) {
        //画出所以轮廓图
        drawContours(drawingAllContours, contours, parentIdx, CV_RGB(255, 255, 255), 1, 8);
        if (hierarchy[i][2] != -1 && ic == 0) {
            parentIdx = i;
            ic++;
        } else if (hierarchy[i][2] != -1) {
            ic++;
        } else if (hierarchy[i][2] == -1) {
            ic = 0;
            parentIdx = -1;
        }

        //有两个子轮廓
        if (ic >= 2) {
            //保存找到的三个黑色定位角
            contours2.push_back(contours[parentIdx]);
            //画出三个黑色定位角的轮廓
            drawContours(drawing, contours, parentIdx,
                         CV_RGB(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), 1, 8);
            ic = 0;
            parentIdx = -1;
        }
    }

    //填充的方式画出三个黑色定位角的轮廓
    for (int i = 0; i < contours2.size(); i++)
        drawContours(drawing2, contours2, i,
                     CV_RGB(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255)), -1, 4,
                     hierarchy[k][2], 0, Point());

    //获取三个定位角的中心坐标
    Point point[3];
    for (int i = 0; i < contours2.size(); i++) {
        point[i] = Center_cal(contours2, i);
    }

    //计算轮廓的面积，计算定位角的面积，从而计算出边长
    area = static_cast<int>(contourArea(contours2[1]));
    int area_side = cvRound(sqrt(double(area)));
    for (int i = 0; i < contours2.size(); i++) {
        //画出三个定位角的中心连线
        line(drawing2, point[i % contours2.size()], point[(i + 1) % contours2.size()], color, area_side / 2, 8);
    }

    namedWindow("DrawingAllContours");
    imshow("DrawingAllContours", drawingAllContours);

    namedWindow("Drawing2");
    imshow("Drawing2", drawing2);

    namedWindow("Drawing");
    imshow("Drawing", drawing);


    //接下来要框出这整个二维码
    Mat gray_all, threshold_output_all;
    vector<vector<Point> > contours_all;
    vector<Vec4i> hierarchy_all;
    cvtColor(drawing2, gray_all, CV_BGR2GRAY);


    threshold(gray_all, threshold_output_all, 45, 255, THRESH_BINARY);
    findContours(threshold_output_all, contours_all, hierarchy_all, RETR_EXTERNAL, CHAIN_APPROX_NONE,
                 Point(0, 0));//RETR_EXTERNAL表示只寻找最外层轮廓


    Point2f fourPoint2f[4];
    //求最小包围矩形
    RotatedRect rectPoint = minAreaRect(contours_all[0]);

    Rect myRect = boundingRect(contours_all[0]);

    //将rectPoint变量中存储的坐标值放到 fourPoint的数组中
    rectPoint.points(fourPoint2f);


    for (int i = 0; i < 4; i++) {
        line(src_all, fourPoint2f[i % 4], fourPoint2f[(i + 1) % 4], Scalar(20, 21, 237), 1);
    }

    namedWindow("Src_all");
    imshow("Src_all", src_all);

    char resultFileNameSring[100];
    sprintf(resultFileNameSring, "QRCode_Locate_result.png");

    Mat resultImage = Mat(src_all, myRect);
    imwrite(resultFileNameSring, resultImage);

    //框出二维码后，就可以提取出二维码，然后使用解码库zxing，解出码的信息。
    //或者研究二维码的排布规则，自己写解码部分
    QRCode_Recognize(resultFileNameSring);

    waitKey(0);
    return (0);
}

```



- 定位

<img src="https://ws4.sinaimg.cn/large/006tKfTcgy1fr550bmfp4j314o16iguv.jpg" width="400px"/>

<img src="https://ws4.sinaimg.cn/large/006tKfTcgy1fr5511yemvj312y16m76l.jpg" width="400px"/>

<img src="https://ws1.sinaimg.cn/large/006tKfTcgy1fr551pwqa6j314o1760vs.jpg" width="400px"/>

<img src="https://ws2.sinaimg.cn/large/006tKfTcgy1fr55d0i93lj314i13wmyi.jpg" width="400px"/>

<img src="https://ws1.sinaimg.cn/large/006tKfTcgy1fr552o35aoj314o14mab8.jpg" width="400px"/>

<img src="https://ws4.sinaimg.cn/large/006tKfTcgy1fr5532eoxvj314s16swom.jpg" width="400px"/>

- 裁剪

<img src="https://ws3.sinaimg.cn/large/006tKfTcgy1fr7tnyiea4j30uo0nq0vo.jpg" width="400px"/>

- 识别结果

![](https://ws2.sinaimg.cn/large/006tKfTcgy1fr7tmrh9czj319a0660tp.jpg)



### 4. 总结

应该是OpenCV的第四个实验了，环境配置已经不再是问题（包括这次的Zbar）。

第1问和第2问，使用OpenCV+Zbar组合，参考博客[1]步骤，很容易的识别了图片中的二维码（标准的二维码：这里标准指的是二维码成像清晰，图片中二维码的空间占比在40%~100%之间）这样标准的图片，Zbar识别起来很容易，不需要Opencv额外的处理。

第3问是非标准QR二维码图片（见QRCode_locate.png），参考[2]首先使用OpenCV对其定位，框出图片中的二维码，再将其裁剪成标准二维码保存下来（见QRCode_Locate_result.png）。最后同第1，2问的方法识别出结果。

总的来说这次实验比较简单，使用OpenCV更加熟练了。

### 5. 参考文档

[1] https://blog.csdn.net/dcrmg/article/details/52132313

[2] https://blog.csdn.net/nick123chao/article/details/77573675

