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


signals:
    void onRecordVideo2MainWindow(float speed);
public slots:
    void setLineEditValue(int value);


private slots:
    void on_lineEdit_textEdited(const QString &arg1);

    void on_pushButton_recordBegin_clicked();

private:

    //滑动条 

    float speed = 50;
    Ui::recordWindow *ui;
};

#endif // RECORDWINDOW_H
