#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QMainWindow>
#include <QImage>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
private slots:
    void on_actionOpen_triggered();

    void on_actionEdit_triggered();

    void on_actionSave_triggered();

    void on_actionReset_triggered();

    void on_actionClear_Mask_triggered();

    void on_actionAll_Mask_triggered();

    void on_actionOpacity_triggered();

    void on_actionReverse_Mask_triggered();

    void on_actionPen_Size_triggered();

    void on_actionSimple_Edit_triggered();

private:
    Ui::MainWindow *ui;
    QString srcfile;
    QString trgfile;

    QImage srcqtimg;
    QImage trgqtimg;
    QImage dstqtimg;
    QImage maskqtimg;
    cv::Mat srccvmat;
    cv::Mat trgcvmat;
    cv::Mat dstcvmat;
    cv::Mat maskcvmat;
    double scale;
    int pensize;
    bool masking;
    int srcx,srcy;
    int mx,my;
    bool Load();
    int Save();
    cv::Mat QImageTocvMat(const QImage &);
    QImage cvMatToQImage(const cv::Mat &img){
        cv::Mat tmp;
        cv::cvtColor(img,tmp,CV_RGB2BGR);
        return QImage((uchar*)tmp.data, tmp.cols, tmp.rows,tmp.step, QImage::Format_RGB888).copy(0,0,img.cols,img.rows);
    }
};

#endif // MAINWINDOW_H
