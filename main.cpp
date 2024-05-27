#include <opencv2/opencv.hpp>
#include <opencv2/text.hpp>

using namespace std;
using namespace cv;

void DrawingCountor(Mat& img, vector<Vec4f>& Lines);
void CountourRelatedSearch(vector<Vec4f>& lines, vector<float>& RelatedCoordinates); // find max x,y and min x,y of HoughLines
void LinesInCountor(vector<float>& RelatedCoordinates, vector<Vec4f>& LinesSrc, vector<Vec4f>& LinesDst); // find HoughLines inside countor

int main(int, char**)
{
    Mat image, imgHSV, mask, m_edges, trim, trim1, trim2, trim3;
    vector<Vec4f> lines;
    vector<float> RelatedCoordinates1, RelatedCoordinates2, RelatedCoordinates3, RelatedCoordinates4;

    String imageName =("Ticket.PNG");
    image = imread(imageName, IMREAD_COLOR);// Load an image
    if( image.empty() )
    {
        return -1;
    }
    imshow("LoadImage", image);

    cvtColor(image, imgHSV, COLOR_BGR2HSV);
    inRange(imgHSV, Scalar(0, 130, 100), Scalar(10, 255, 255), mask);
    morphologyEx(mask, mask, MorphTypes::MORPH_CLOSE, getStructuringElement(MorphShapes::MORPH_RECT, Size(5,5)), Point(-1,-1), 2);
    morphologyEx(mask, mask, MorphTypes::MORPH_OPEN, getStructuringElement(MorphShapes::MORPH_RECT, Size(5,5)), Point(-1,-1), 5);
    imshow("mask", mask);

    Canny(mask, m_edges, 40, 100);
    HoughLinesP(m_edges, lines, 1, CV_PI/180, 15, 30, 30);
    DrawingCountor(image, lines);

    const int offset = 100;
    CountourRelatedSearch(lines, RelatedCoordinates1);
    RelatedCoordinates1[0] += offset;
    RelatedCoordinates1[1] -= offset;
    RelatedCoordinates1[2] += offset;
    RelatedCoordinates1[3] -= offset;

    vector<Vec4f> lines_in_countor_1;
    LinesInCountor(RelatedCoordinates1, lines, lines_in_countor_1);

    CountourRelatedSearch(lines_in_countor_1, RelatedCoordinates2);

    vector<Vec4f> lines_in_countor_2;
    LinesInCountor(RelatedCoordinates2, lines_in_countor_1, lines_in_countor_2);

    float Min_x = RelatedCoordinates2[0];
    float Max_x = RelatedCoordinates2[1];
    float Min_y = RelatedCoordinates2[2];
    float Max_y = RelatedCoordinates2[3];
    trim = image(Range(Min_y, Max_y), Range(Min_x, Max_x));
    trim1 = trim(Range(0, trim.rows/2-10), Range(0, trim.cols));
    imshow("trim 1", trim1);

    trim2 = trim(Range(trim.rows/2, trim.rows), Range(0, trim.cols/2));
    imshow("trim 2", trim2);

    trim3 = trim(Range(trim.rows/2, trim.rows), Range(trim.cols/2, trim.cols));
    imshow("trim 3", trim3);

    char key = (char)waitKey(0);
    return EXIT_SUCCESS;
}

void DrawingCountor(Mat& img, vector<Vec4f>& Lines)
{
    Point2f pt1, pt2;
    for(size_t i = 0; i < Lines.size(); i++)
    {
        pt1.y = Lines[i][1];
        pt1.x = Lines[i][0];
        pt2.x = Lines[i][2];
        pt2.y = Lines[i][3];
        line(img, pt1, pt2, Scalar(0, 255, 0), 1, LineTypes::LINE_4);
    }
    imshow("FinalImage", img);
}

void CountourRelatedSearch(vector<Vec4f>& lines, vector<float>& RelatedCoordinates)
{
    float Max_x = 0, Min_x = 1000, Max_y = 0, Min_y = 1000;
    Point2f pt1, pt2;
    for(size_t i = 0; i < lines.size(); i++)
    {
        pt1.x = lines[i][0];
        if (pt1.x > Max_x)
            Max_x = pt1.x;
        if(pt1.x < Min_x)
            Min_x = pt1.x;

        pt1.y = lines[i][1];
        if (pt1.y > Max_y)
            Max_y = pt1.y;
        if (pt1.y < Min_y)

        pt2.x = lines[i][2];
        if (pt2.x > Max_x)
            Max_x = pt2.x;
        if (pt2.x < Min_x)
            Min_x = pt2.x;

        pt2.y = lines[i][3];
        if (pt2.y > Max_y)
            Max_y = pt2.y;
        if (pt2.y < Min_y)
            Min_y = pt2.y;
    }
    RelatedCoordinates.push_back(Min_x);
    RelatedCoordinates.push_back(Max_x);
    RelatedCoordinates.push_back(Min_y);
    RelatedCoordinates.push_back(Max_y);
}

void LinesInCountor(vector<float>& RelatedCoordinates, vector<Vec4f>& LinesSrc, vector<Vec4f>& LinesDst)
{
    float Min_x = RelatedCoordinates[0];
    float Max_x = RelatedCoordinates[1];
    float Min_y = RelatedCoordinates[2];
    float Max_y = RelatedCoordinates[3];

    for (size_t i = 0; i < LinesSrc.size(); i++)
    {
        if ( Min_x < LinesSrc[i][0] && LinesSrc[i][0] < Max_x &&
            Min_x < LinesSrc[i][2] && LinesSrc[i][2] < Max_x &&
            Min_y < LinesSrc[i][1] && LinesSrc[i][1] < Max_y &&
            Min_y < LinesSrc[i][3] && LinesSrc[i][3] < Max_y   )
        {
            LinesDst.push_back(LinesSrc[i]);
        }
    }
}
