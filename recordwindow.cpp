#include "recordwindow.h"
#include "ui_recordwindow.h"
#include <QHBoxLayout>
#include <qdebug.h>
recordWindow::recordWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::recordWindow)
{
    ui->setupUi(this);
    ui->lineEdit_slider->setText("50");
    
    ui->slider->setMinimum(25);      // 设置滑动条的最小值 
    ui->slider->setMaximum(75);   // 设置滑动条的最大值 

    ui->slider->setValue(50); // 设置滑动条初始值 
	 
    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(setLineEditValue(int)));

    connect(ui->pushButton_recordBegin, SIGNAL(click()), this, SLOT(on_pushButton_recordBegin_clicked()));
}

void recordWindow::setLineEditValue(int value)
{
    int pos = ui->slider->value();
    QString str = QString("%1").arg(pos);
    ui->lineEdit_slider->setText(str);
    speed = (float)pos;
}

recordWindow::~recordWindow()
{
    delete ui;
}

void recordWindow::on_lineEdit_textEdited(const QString &arg1)
{
    speed = QString(ui->lineEdit_slider->text()).toFloat();
    ui->slider->setValue(speed);
}

void recordWindow::on_pushButton_recordBegin_clicked()
{
    qDebug() << "11";
    emit(onRecordVideo2MainWindow(speed));
}
