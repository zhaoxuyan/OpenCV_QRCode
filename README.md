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

![image-20180509012504868](/var/folders/pz/8hdd81_959q73c1y_m3gpm6c0000gn/T/abnerworks.Typora/image-20180509012504868.png)

### 3. 对非标准条形码，QR二维码图片，进行定位，然后用Zbar（或者Zxing）解码显示。

