#include "pendialog.h"
#include "ui_pendialog.h"
#include <QSpinBox>
#include <iostream>
#include <QPainter>
PenDialog::PenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PenDialog)
{
    ui->setupUi(this);
    ui->pensizebox->setMaximum(50);
    ui->pensizebox->setMinimum(1);
}

PenDialog::~PenDialog()
{
    delete ui;
}

void PenDialog::paintEvent(QPaintEvent *){
    int pensize=getPenSize();
    QImage image(QSize(100,100), QImage::Format_ARGB32_Premultiplied);
    QPainter imagePainter(&image);
    imagePainter.setRenderHint(QPainter::Antialiasing, true);
    imagePainter.setBrush(Qt::white);
    imagePainter.setPen(Qt::white);
    imagePainter.drawRect(0,0,100,100);
    imagePainter.setBrush(Qt::black);
    imagePainter.setPen(Qt::black);

    imagePainter.drawEllipse(QPoint(50,50),pensize/2,pensize/2);
    QPainter widgetPainter(this);
    widgetPainter.drawImage(230, 80, image);

}

int PenDialog::getPenSize(){
    return ui->pensizebox->value();
}

void PenDialog::setPenSize(int pensize){
    ui->pensizebox->setValue(pensize);
}

void PenDialog::on_pensizebox_valueChanged(int pensize)
{
    update();
}
