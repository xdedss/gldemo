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
    ui->slider->setMaximum(175);   // 设置滑动条的最大值 


    ui->slider->setValue(175); // 设置滑动条初始值 
	 
    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(setLineEditValue(int)));
    // 点击录像事件   
    connect(ui->pushButton_recordBegin, SIGNAL(click()), this, SLOT(on_pushButton_recordBegin_clicked()));
    // 点击保存事件
    connect(ui->pushButton_recordSave, SIGNAL(click()), this, SLOT(on_pushButton_recordSave_clicked()));
    // 点击预览事件    
    connect(ui->pushButton_recordPreview, SIGNAL(click()), this, SLOT(on_pushButton_recordPreview_clicked()));
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
    qDebug() << "record begin";
    emit(onRecordVideo2MainWindow(speed,true));
}

void recordWindow::on_pushButton_recordPreview_clicked()
{
    qDebug() << "preview begin";
    emit(onRecordVideo2MainWindow(speed, false));
}

void recordWindow::offRecordVideo(bool RecordOrPreview)
{
    if (RecordOrPreview) {
        qDebug() << "record finish";
        ui->pushButton_recordSave->setEnabled(true);
    }
    else
        qDebug() << "preview finish";
}





void recordWindow::on_pushButton_recordSave_clicked()
{
    qDebug() << "save begin";
    emit(onSaveVideo2MainWindow());
}

void recordWindow::offSaveVideo()
{
    qDebug() << "save finish";
    ui->pushButton_recordSave->setEnabled(false);
}