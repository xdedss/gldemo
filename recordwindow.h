#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QLineEdit>
#include <QSlider>

namespace Ui {
class recordWindow;
}

class recordWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit recordWindow(QWidget *parent = nullptr);
    ~recordWindow();
    Ui::recordWindow* ui;

signals:
    void onRecordVideo2MainWindow(float speed);
    void onSaveVideo2MainWindow();

public slots:
    void setLineEditValue(int value);
    void offRecordVideo();
    void offSaveVideo();

private slots:
    void on_lineEdit_textEdited(const QString &arg1);
    void on_pushButton_recordBegin_clicked();
    void on_pushButton_recordSave_clicked();
private:

    //滑动条
    float speed = 50;
};

#endif // RECORDWINDOW_H
