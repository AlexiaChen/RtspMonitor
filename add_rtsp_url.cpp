#include "add_rtsp_url.h"
#include "ui_add_rtsp_url.h"

add_Rtsp_Url::add_Rtsp_Url(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_Rtsp_Url)
{
    ui->setupUi(this);
}

add_Rtsp_Url::~add_Rtsp_Url()
{
    delete ui;
}

void add_Rtsp_Url::on_pB_Cancel_clicked()
{
    this->close();
}

void add_Rtsp_Url::on_pB_OK_clicked()
{
    QString sRtsp_Url = ui->m_rtspUrl->text();
    QString sDevice_Name = ui->m_deviceName->text();

    emit add_a_device(sRtsp_Url, sDevice_Name);
    this->close();
}
