#include <vector>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

const int corners_row = 5;
const int corners_column = 7;

int main(int argc, char** argv) {
    String image_path = samples::findFile("resource/src_balls.jpg");
    Mat dst, src = imread(image_path, IMREAD_COLOR);
    vector<Point2f> corners;

    namedWindow("The direction of the ball movement", WINDOW_NORMAL | WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);
    resizeWindow("The direction of the ball movement", 700, 700 * src.rows / src.cols);

    // Поиск, калибрвка и отрисовка шахматной доски
    if (!findChessboardCorners(src, Size(7, 5), corners)) {
        return -1;
    }
    cvtColor(src, dst, COLOR_BGR2GRAY);
    cornerSubPix(dst, corners, Size(10, 10), Size(-1, -1), TermCriteria(TermCriteria::COUNT, 100, 0.01));
    drawChessboardCorners(src, Size(corners_row, corners_column), corners, true);

    // Калибровка камеры
    vector<vector<Point3f>> object_points; // Трехмерные координаты угловых точек на калибровочной доске
    vector<vector<Point2f>> image_points; // Двумерные координаты угловых точек на калибровочной доске
    Mat camera_matrix(3, 3, CV_32FC1, Scalar::all(0)); // Матрица параметров в камере
    Mat dist_coeffs(1, 5, CV_32FC1, Scalar::all(0)); // 5 коэффициентов искажения камеры: k1, k2, p1, p2, k3
    vector<Mat> tvecs; // Вектор вращения изображения
    vector<Mat> rvecs; // Вектор перевода для изображения

    vector<Point3f> obj_p;
    for(int i = 0; i < corners_row; i++) {
        for(int j = 0; j < corners_column; j++) {
            obj_p.push_back(Point3f(i, j, 0));
        }
    }
    object_points.push_back(obj_p);
    image_points.push_back(corners);

    calibrateCamera(object_points, image_points, Size(src.rows, src.cols), camera_matrix, dist_coeffs, rvecs, tvecs);

    // Преобразование изображения
    Mat rmat;
    undistort(src, dst, camera_matrix, dist_coeffs); // Исправление искривлений камеры

    // Не работает
    // Rodrigues(rvecs[0], rmat); // поворот изображения
    // warpPerspective(dst, dst, rmat, Size(dst.rows, dst.cols));

    imshow("The direction of the ball movement", dst);

    waitKey(0);
    return 0;
}
