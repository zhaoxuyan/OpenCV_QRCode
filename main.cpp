//#include <opencv2/opencv.hpp>
//#include <zbar.h>
//
//using namespace std;
//using namespace zbar;  //添加zbar名称空间
//using namespace cv;
//
//int main(int argc,char*argv[])
//{
//    ImageScanner scanner;
//    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
//    Mat image = imread("QRCode_Locate_result.png");
//    Mat imageGray;
//    cvtColor(image,imageGray,CV_RGB2GRAY);
//    int width = imageGray.cols;
//    int height = imageGray.rows;
//    auto *raw = imageGray.data;
//    Image imageZbar(static_cast<unsigned int>(width), static_cast<unsigned int>(height), "Y800", raw,
//                    static_cast<unsigned long>(width * height));
//    scanner.scan(imageZbar); //扫描条码
//    Image::SymbolIterator symbol = imageZbar.symbol_begin();
//    if(imageZbar.symbol_begin()==imageZbar.symbol_end())
//    {
//        cout<<"查询条码失败，请检查图片！"<<endl;
//    }
//    for(;symbol != imageZbar.symbol_end();++symbol)
//    {
//        cout<<"类型："<<endl<<symbol->get_type_name()<<endl<<endl;
//        cout<<"条码："<<endl<<symbol->get_data()<<endl<<endl;
//    }
//    imshow("Source Image",image);
//    waitKey();
//    imageZbar.set_data(nullptr,0);
//    return 0;
//}