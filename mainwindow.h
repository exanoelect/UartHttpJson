#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
//#include <ArduinoJson-6.x/ArduinoJson.h>
#include "json.h"
#include "myhttp.h"
#include "config.h"
#include "filemanager.h"

#define COM0 "COM18"
#define COM1 "COM19"
#define COM2 "COM20"
#define COM3 "COM21"
#define COM4 "COM22"
#define COM5 "COM23"

using namespace std;

namespace Ui {
class MainWindow;
}

struct davisData{
    //int bar_Trend;
    uint16_t id_data;
    QString timeStamps;
    bool packet_Type;
    //int16_t next_Record;
    double barometer;
    double inside_Temperature;
    double inside_Humidity;
    double outside_Temperature;
    double wind_Speed;
    double Ten_Min_Avg_wind_Speed;
    double wind_Direction;
    //float xtra_Temperature;
    //float soil_Temperature;
    //float leaf_Temperature;
    double outside_Humidity;
    //float extra_Humidity;
    uint16_t rain_Rate;
    uint8_t UV;
    uint16_t solar_Radiation;
    //float storm_Rain;
    //uint16_t start_Date_of_Current_Strom;
    //uint16_t day_Rain;
    //uint16_t month_Rain;
    //uint16_t year_Rain;
    //uint16_t day_ET;
    //uint16_t month_ET;
    //uint16_t year_ET;
    //Soil Moisture
    //leaf Wetnesses
    //Inside Alaram
    //Rain Alaram
    //Outside Alaram
    //Extra Time or Humidity Alaram
    //Soil & Leaf Alaram
    //---uint8_t transmitter_Battery_Status;
    double console_Battery_Voltage;
    //Forecast Icons
    //Forecast  Rule Number
    //uint16_t time_of_Sunrise;
    //uint16_t time_of_Sunset;
};

struct dataPost{
    uint16_t id_data;
    QString timeStamps;
    bool packet_Type;
    double barometer;
    double inside_Temperature;
    double inside_Humidity;
    double outside_Temperature;
    double wind_Speed;
    double Ten_Min_Avg_wind_Speed;
    int wind_Direction;
    double outside_Humidity;
    uint16_t rain_Rate;
    uint8_t UV;
    uint16_t solar_Radiation;
    double console_Battery_Voltage;
};

enum HTTP_STATE{
    START_UP,
    COM_SCAN,
    COM_OPEN,
    COM_WAIT_TEST_RESPONSE,
    COM_RECEIVE,
    COM_VALID,
    GET_DATA_SERIAL,
    WAITING_DATA_SERIAL_COMPLETE,
    GET_HTTP,
    WAITING_HTTP_GET,
    RECEIVE_HTTP_GET,
    SEND_POST,
    WAITING_HTTP_POST
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    uint16_t hex2dec(char hex[10]);
    bool init_port(QString portname);
    void writeData(const QByteArray &data);
    //void http_post_data();
    //double http_get(void);
    void getDateTime(void);
    void restartApps(void);
    void saveJsonData(void);

    void postJson(void);
    void getHttp(void);
    void parsingJson(void);
    void loadSettings(void);

public slots:
    //void replyFinished(QNetworkReply *reply);
private slots:
    void on_btn_SerialPort_clicked();
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void closeSerialPort();
    void on_btn_GetData_clicked();
    void callBackTimer();
    //QString getCOM();

    void on_btnExit_clicked();
    //void serviceRequestFinished(QNetworkReply* reply);

    void on_btnRestart_clicked();

    void on_checkBoxLock_stateChanged(int arg1);

    void on_btnTest_clicked();

    void on_btnPost_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *m_serial = nullptr;
    QTimer *timerCmd;
    myhttp *mhttp;
    //QTimer *timerComScan;
    //QNetworkAccessManager *namPost;
    //QNetworkAccessManager *nam;
    //QNetworkReply *replyPost;
    //QNetworkReply *replyGet;

    unsigned char charAt;

    //QString portname;
    uint8_t port_numerator;
    uint8_t retry;
    bool response_test_valid;
    QString portname[10];// = ["COM18","COM18","COM18","COM18","COM18];
    davisData myDavisData;
    dataPost myDataPost;
    uint8_t process_state;
    uint8_t counter_second;
    uint8_t counter_minutes;
    uint8_t counter_hours;
    uint8_t post_periodic;
    QString pathSave;
    int port_ctr;
    int citcat;

    //uint8_t com_scan_state;

    // Config
    Config config;
    filemanager fman;
};

#endif // MAINWINDOW_H
