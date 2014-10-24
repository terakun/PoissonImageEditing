#ifndef PENDIALOG_H
#define PENDIALOG_H

#include <QDialog>

namespace Ui {
class PenDialog;
}

class PenDialog : public QDialog
{
    Q_OBJECT
    
protected:
    void paintEvent(QPaintEvent *);
public:
    explicit PenDialog(QWidget *parent = 0);
    ~PenDialog();
    int getPenSize();
    void setPenSize(int pensize);
private slots:
    void on_pensizebox_valueChanged(int pensize);

private:
    Ui::PenDialog *ui;
};

#endif // PENDIALOG_H
