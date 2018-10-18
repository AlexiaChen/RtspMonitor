#ifndef ADD_RTSP_URL_H
#define ADD_RTSP_URL_H

#include <QDialog>
#include <QWidget>
#include "ui_add_rtsp_url.h"

namespace Ui {
class add_Rtsp_Url;
}

class add_Rtsp_Url : public QDialog
{
    Q_OBJECT

public:
    explicit add_Rtsp_Url(QWidget *parent = 0);
    ~add_Rtsp_Url();

signals:
    void add_a_device(QString& rtsp, QString& deviceName);

private slots:
    void on_pB_Cancel_clicked();

    void on_pB_OK_clicked();

private:
    Ui::add_Rtsp_Url *ui;
};

#endif // ADD_RTSP_URL_H
