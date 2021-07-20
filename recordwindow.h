#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QLineEdit>
#include <QSlider>

namespace Ui {
class recordWindow;
}

/**
* @class recordWindow
* @brief 录制窗口
*/
class recordWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit recordWindow(QWidget *parent = nullptr);
    ~recordWindow();
    Ui::recordWindow* ui;

signals:
    /** 
    * @brief 录像开启信号，从recordwindow传到mainwindow    \n
    * 携带参数为摄像机移动速度、录像是否存储，不存储则为预览模式 
    * @note
    * bool RecordOrPreview:true表示存入视频数据；false表示只能预览  预览不能开启视频保存功能 \n
    * 若之前完成过一次record，没有save，此时点击save可以保存上次record而没有保存的视频
    * @attention
    * 预览到效果满意后，一定要record后，才能save!!!
    */
    void onRecordVideo2MainWindow(float speed,bool RecordOrPreview);
    /**
    * @brief 视频保存信号，从recordwindow传到mainwindow
    */
    void onSaveVideo2MainWindow();

private slots:
    /** @brief 接收滑动条改变信号，设置文本框的文本 */
    void setLineEditValue(int value);
    /** @brief 接收录像结束信号，开启视频保存功能 */
    void offRecordVideo(bool RecordOrPreview);
    /** @brief 接收保存视频结束信号，关闭视频保存功能 */
    void offSaveVideo();
    /** @brief 接收文本框编辑信号，改变滑动条数值 */
    void on_lineEdit_textEdited(const QString &arg1);
    /** @brief 接收录像开始的信号 */
    void on_pushButton_recordBegin_clicked();
    /** @brief 接收视频保存开始的信号 */
    void on_pushButton_recordSave_clicked();
    /** @brief 接收预览开始的信号 */
    void on_pushButton_recordPreview_clicked();
private:
    //滑动条 
    float speed = 50;
};

#endif // RECORDWINDOW_H
