#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMenuBar>
#include <QWheelEvent>
#include "poissonimageediting.h"
#include <iostream>
#include "pendialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    masking=false;
    scale=1;
    pensize=50;
    srcx=srcy=0;
    ui->setupUi(this);
    ui->actionEdit->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->menuEdit->setEnabled(false);
    setWindowTitle("Poisson Image Editing");
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    if(!ui->menuEdit->isEnabled()&&!ui->actionSave->isEnabled()) return;
    QImage image(trgqtimg.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter imagePainter(&image);
    imagePainter.setRenderHint(QPainter::Antialiasing, true);
    if(!ui->actionSave->isEnabled()){
        qreal alpha=ui->actionOpacity->isChecked()?0.5:1;
        imagePainter.drawImage(0,0,trgqtimg);
        imagePainter.setOpacity(alpha);


        cv::Mat blackimg(srccvmat.rows*scale,srccvmat.cols*scale,srccvmat.type());
        cv::Mat resizesrcimg(srccvmat.rows*scale,srccvmat.cols*scale,srccvmat.type());
        cv::Mat resizemaskimg(srccvmat.rows*scale,srccvmat.cols*scale,srccvmat.type());
        blackimg=cv::Scalar::all(0);
        cv::resize(srccvmat, resizesrcimg, resizesrcimg.size(), cv::INTER_CUBIC);
        cv::resize(maskcvmat, resizemaskimg, resizemaskimg.size(), cv::INTER_CUBIC);

        resizesrcimg.copyTo(blackimg,resizemaskimg);

        imagePainter.drawImage(srcx,srcy,cvMatToQImage(blackimg));
    }else{
        imagePainter.drawImage(0,0,dstqtimg);
    }
    QPainter widgetPainter(this);
    widgetPainter.drawImage(0, 0, image);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if(!ui->actionEdit->isEnabled())return;
    if(masking){
        cv::Scalar pen=ui->actionEraser->isChecked()?cv::Scalar::all(255):cv::Scalar::all(0);
        cv::Point s((mx-srcx)/scale,(my-srcy)/scale);
        cv::Point e((event->x()-srcx)/scale,(event->y()-srcy)/scale);
        cv::line(maskcvmat,s,e,pen,pensize/scale);
    }else{
        srcx+=event->x()-mx;
        srcy+=event->y()-my;
    }
    mx=event->x();
    my=event->y();
    update();
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    if(!ui->actionEdit->isEnabled())return;
    if(event->button()==Qt::RightButton){
        masking=true;
    }
    mx=event->x();
    my=event->y();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *){
    masking=false;
}

void MainWindow::wheelEvent(QWheelEvent *event){

    double step=(double)event->delta();
    if(step>0&&scale<10)
        scale*=1.1;
    else if(step<0&&scale>0.1)
        scale*=1.0/1.1;
    update();
}

bool MainWindow::Load(){
    srcfile = QFileDialog::getOpenFileName(this,tr("Source File"),"/home/terasaki/ImageforPoissonImageEditing",tr("Image(*.tif *.png *.jpg)"));
    if(srcfile=="") return false;
    srccvmat=cv::imread(srcfile.toStdString());
    maskcvmat=srccvmat.clone();
    maskcvmat=cv::Scalar::all(255);
    if(srccvmat.empty()) return false;
    srcqtimg=cvMatToQImage(srccvmat);

    trgfile = QFileDialog::getOpenFileName(this,tr("Target File"),"/home/terasaki/ImageforPoissonImageEditing",tr("Image(*.tif *.png *.jpg)"));
    if(trgfile=="") return false;
    trgcvmat=cv::imread(trgfile.toStdString());
    if(trgcvmat.empty())return false;

    trgqtimg=cvMatToQImage(trgcvmat);
    setFixedSize(trgqtimg.size());
    srcx=srcy=0;

    return true;
}

void MainWindow::on_actionOpen_triggered()
{
    bool LoadisSucceed=Load();
    if(LoadisSucceed){
        scale=1;
        srcx=srcy=0;
    }
    ui->actionSave->setEnabled(false);
    ui->menuEdit->setEnabled(LoadisSucceed);
    ui->actionEdit->setEnabled(LoadisSucceed);
    ui->actionSimple_Edit->setEnabled(LoadisSucceed);
    ui->actionReset->setEnabled(false);

}

void MainWindow::on_actionEdit_triggered()
{
    cv::Mat resizesrcmat(srccvmat.rows*scale,srccvmat.cols*scale,srccvmat.type());
    cv::Mat resizemaskmat(srccvmat.rows*scale,srccvmat.cols*scale,srccvmat.type());
    cv::resize(srccvmat, resizesrcmat, resizesrcmat.size(), cv::INTER_CUBIC);
    cv::resize(maskcvmat, resizemaskmat, resizemaskmat.size(), cv::INTER_CUBIC);
    cv::Rect area;
    if(srcx<0){
        area.x=0;
        area.width=resizesrcmat.cols+srcx;
    }else{
        area.x=srcx;
        if(trgcvmat.cols<=srcx+resizesrcmat.cols){
            area.width=trgcvmat.cols-srcx;
        }else{
            area.width=resizesrcmat.cols;
        }
    }
    if(srcy<0){
        area.y=0;
        area.height=resizesrcmat.rows+srcy;
    }else{
        area.y=srcy;
        if(trgcvmat.rows<=srcy+resizesrcmat.rows){
            area.height=trgcvmat.rows-srcy;
        }else{
            area.height=resizesrcmat.rows;
        }
    }

    cv::Mat mask(resizemaskmat.rows,resizemaskmat.cols,CV_8UC1);

    cv::cvtColor(resizemaskmat,mask,CV_BGR2GRAY);

    cv::Mat src_img=cv::Mat(resizesrcmat,cv::Rect(area.x-srcx,area.y-srcy,area.width,area.height));
    cv::Mat mask_img=cv::Mat(mask,cv::Rect(area.x-srcx,area.y-srcy,area.width,area.height));

    dstcvmat=Editing(src_img,trgcvmat,mask_img,area,ui->actionGradient_Mixture->isChecked());

    if(dstcvmat.empty()) return;
    dstqtimg=cvMatToQImage(dstcvmat);
    ui->actionEdit->setEnabled(false);
    ui->actionSimple_Edit->setEnabled(false);

    ui->actionSave->setEnabled(true);
    ui->actionReset->setEnabled(true);

    update();

}

void MainWindow::on_actionSave_triggered()
{
    QString dstfile = QFileDialog::getSaveFileName(this,tr("dst File"),".",tr("Image(*.tif *.png *.jpg)"));
    if(dstfile=="") return;
    cv::imwrite(dstfile.toStdString(),dstcvmat);
    QMessageBox::about(this,"success","dst file wrote:"+dstfile);
}

void MainWindow::on_actionReset_triggered()
{
    ui->actionSave->setEnabled(false);
    ui->actionEdit->setEnabled(true);
    ui->actionSimple_Edit->setEnabled(true);
    ui->actionReset->setEnabled(false);
    update();
}

void MainWindow::on_actionClear_Mask_triggered()
{
    maskcvmat=cv::Scalar::all(255);
    update();
}

void MainWindow::on_actionAll_Mask_triggered()
{
    maskcvmat=cv::Scalar::all(0);
    update();
}


void MainWindow::on_actionOpacity_triggered()
{
    update();
}

void MainWindow::on_actionReverse_Mask_triggered()
{
    maskcvmat=~maskcvmat;
    update();
}

void MainWindow::on_actionPen_Size_triggered()
{
    PenDialog d;
    d.setPenSize(pensize);
    if(d.exec()){
        pensize=d.getPenSize();
    }
}

void MainWindow::on_actionSimple_Edit_triggered()
{
    cv::Mat resizesrcmat(srccvmat.rows*scale,srccvmat.cols*scale,srccvmat.type());
    cv::Mat resizemaskmat(srccvmat.rows*scale,srccvmat.cols*scale,srccvmat.type());
    cv::resize(srccvmat, resizesrcmat, resizesrcmat.size(), cv::INTER_CUBIC);
    cv::resize(maskcvmat, resizemaskmat, resizemaskmat.size(), cv::INTER_CUBIC);
    cv::Rect area;
    if(srcx<0){
        area.x=0;
        area.width=resizesrcmat.cols+srcx;
    }else{
        area.x=srcx;
        if(trgcvmat.cols<=srcx+resizesrcmat.cols){
            area.width=trgcvmat.cols-srcx;
        }else{
            area.width=resizesrcmat.cols;
        }
    }
    if(srcy<0){
        area.y=0;
        area.height=resizesrcmat.rows+srcy;
    }else{
        area.y=srcy;
        if(trgcvmat.rows<=srcy+resizesrcmat.rows){
            area.height=trgcvmat.rows-srcy;
        }else{
            area.height=resizesrcmat.rows;
        }
    }

    cv::Mat mask(resizemaskmat.rows,resizemaskmat.cols,CV_8UC1);

    cv::cvtColor(resizemaskmat,mask,CV_BGR2GRAY);

    cv::Mat src_img=cv::Mat(resizesrcmat,cv::Rect(area.x-srcx,area.y-srcy,area.width,area.height));
    cv::Mat mask_img=cv::Mat(mask,cv::Rect(area.x-srcx,area.y-srcy,area.width,area.height));

    dstcvmat=SimpleEditing(src_img,trgcvmat,mask_img,area);

    if(dstcvmat.empty()) return;
    dstqtimg=cvMatToQImage(dstcvmat);
    ui->actionEdit->setEnabled(false);
    ui->actionSimple_Edit->setEnabled(false);
    ui->actionSave->setEnabled(true);
    ui->actionReset->setEnabled(true);

    update();


}
