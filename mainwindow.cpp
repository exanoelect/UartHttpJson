#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <iostream>
#include <QDateTime>
#include <QtDebug>
#include "qdebug.h"
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QProcess>
#include <QSerialPortInfo>
#include <QFileDialog>
#include <QtCore>
#include <QDir>
#include <QDate>
#include <QDialog>
#include <QFile>
#include <QFileInfo>
#include <QBuffer>
#include <QImageWriter>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
//#include <ArduinoJson-6.x/ArduinoJson.h>
#include <stddef.h>
#include "myhttp.h"
#include "config.h"
#include "filemanager.h"
#include "json.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_serial(new QSerialPort(this))
{
    ui->setupUi(this);
    loadSettings();
    //initialize COM Port String
    //portname[0] = COM0;
    //portname[1] = COM1;
    //portname[2] = COM2;
    //portname[3] = COM3;
    //portname[4] = COM4;

    //init variable
    port_numerator = 0;
    retry = 0;
    counter_second = 58;
    counter_minutes = 58;
    myDavisData.id_data = 0;
    myDavisData.outside_Temperature = 23.00;
    myDavisData.inside_Humidity = 0;
    myDavisData.outside_Humidity = 0;
    myDavisData.outside_Temperature = 0;
    myDavisData.wind_Direction = 0;
    myDavisData.wind_Speed = 0;
    myDavisData.Ten_Min_Avg_wind_Speed = 0;
    myDavisData.rain_Rate = 0;
    myDavisData.barometer = 1000.00;

    process_state = COM_OPEN;
    response_test_valid = false;
    //process_state = START_UP;

    timerCmd = new QTimer(this);
    mhttp = new myhttp(this);

    //myhttp mhttp;
    //timerComScan = new QTimer(this);
    //nam = new QNetworkAccessManager(this);
    //replyGet = new QNetworkReply(this);

    // connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(timerCmd, SIGNAL(timeout()), this, SLOT(callBackTimer()));
    //connect(nam, SIGNAL(finished(QNetworkReply*)),
    //        this, SLOT(replyFinished(QNetworkReply*)));

    //connect(timerComScan, SIGNAL(timeout()), this, SLOT(getCom()));
    //int n = 0;
/*
    port_ctr = 0;
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos){
        QString s = info.portName();
        //auto label = new QLabel(s);
        qDebug() << "COM PORT = " << s;
        portname[port_ctr] = s;
        port_ctr++;
    }
*/
    timerCmd->start(1000);

    //Test procedur here:
    //saveJsonData();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
MainWindow::~MainWindow()
{
    delete ui;
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::on_btn_SerialPort_clicked()
{
    init_port("COM18");
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
bool MainWindow::init_port(QString portname)
{
    //QTextStream out(stdout);
    //QString portname = "COM18";
    m_serial = new QSerialPort(portname);

    m_serial->setPortName(portname);//("COM12");
    m_serial->setBaudRate(QSerialPort::Baud19200);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setStopBits(QSerialPort::OneStop);

    if (!m_serial->open(QIODevice::ReadWrite)){
        qDebug() << "Open Port error";
        ui->lblStatus->setText("OPEN PORT ERROR");
        return false;
    }else{
        qDebug() << "Open Port OK";
        connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
        connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
        //connect(timerCmd, SIGNAL(timeout()), this, SLOT(callBackTimer()));
        //ui->btn_GetData->setEnabled(true);
        ui->lblStatus->setText("OPEN" + portname + "OK");
        return true;
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::on_btn_GetData_clicked()
{
    //writeData("LOOP 1\r");
    //timerCmd->start(5000);

    mhttp->http_get("http://api.openweathermap.org/data/2.5/weather?lat=-6.9345&lon=107.605&units=metric&APPID=249534879c6a216ed7f97af0af2bc477");
    //writeData("LOOP 1\r");
    qDebug() << "Get HTTP ";
    parsingJson();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::readData()
{
    //const QByteArray myData = m_serial->readAll();
    QByteArray myData = m_serial->readAll();
    int n = myData.size();
    qDebug() << "Serial received:" << myData;
   // QByteArray temp;
    QString recStr(myData); //= QTextCodec::codecForMib(1015)->toUnicode(myData);
    qDebug() << "String received:" << recStr;

    char temp[10];
    uint16_t temp16;
    //char tempBarometer[10];
    double tempDouble;
    char hex[10] = "\0";

    if(myData.isEmpty()){

    }else{
        switch(process_state){
            case COM_WAIT_TEST_RESPONSE:
                qDebug() << "COM_WAIT_TEST_RESPONSE";
                if(recStr.contains("TEST")){
                    qDebug() << "TEST OK";
                    response_test_valid = true;
                    process_state = GET_DATA_SERIAL;
                    //process_state = COM_VALID;
                }else{
                    response_test_valid = false;
                    qDebug() << "Not yet Bro...";
                }
                m_serial->flush();
                m_serial->clear();
                myData.chop(n);
                break;
            case WAITING_DATA_SERIAL_COMPLETE:
                if(myData.length() >= 100){
                    qDebug() << "Serial received:" << myData;
                    //parsing
                    //Test funct hex2dec
                    /* verified
                    hex[0] = 0x30;
                    hex[1] = 0x31;
                    qDebug() << "hex0:" <<  hex[0];
                    qDebug() << "hex0:" <<  hex[1];
                    snprintf(temp,10,"%X%X", hex[0], hex[1]);
                    qDebug() << "awal" << temp;
                    temp16 = hex2dec(temp);
                    qDebug() << "Hex2Dec" << temp16;
                    temp16 = atoi(temp);
                    qDebug() << "atoi hex2dec" << temp16;
                    */

                    //Packet Type
                    myDavisData.packet_Type = myData.at(5);
                    qDebug() << "Packet Type" << myDavisData.packet_Type;

                    //Wind Speed
                    myDavisData.wind_Speed = myData.at(15)*0.44704;
                    qDebug() << "Wind Speed" << myDavisData.wind_Speed;
                    ui->labelWindSpeed->setText(QString::number(myDavisData.wind_Speed));

                    //Avg Wind Speed
                    myDavisData.Ten_Min_Avg_wind_Speed = myData.at(16)*0.44704;
                    qDebug() << "Avg Wind Speed" << myDavisData.wind_Speed;
                    ui->labelWindSpeedAvg->setText(QString::number(myDavisData.Ten_Min_Avg_wind_Speed));

                    //Wind Direction
                    hex[0] = myData.at(17);
                    qDebug() << "hex[0]" << hex[0];
                    snprintf(temp,10,"%X", hex[0]);
                    qDebug() << "hex[0] temp" << temp;

                    hex[1] = myData.at(18);
                    qDebug() << "hex[1]" << hex[1];
                    snprintf(temp,10,"%X", hex[1]);
                    int tempHex = atoi(temp);
                    if((tempHex <= 68) && (tempHex >= 0)){
                        qDebug() << "hex[1] temp" << temp;
                        snprintf(temp,10,"%X%X", hex[0], hex[1]);
                        qDebug() << "WindDirectionTemp" << temp;
                        myDavisData.wind_Direction = atoi(temp);
                        if((myDavisData.wind_Direction <= 360) && (myDavisData.wind_Direction >= 0)){
                            qDebug() << "WindDirection" << myDavisData.wind_Direction;
                            ui->labelWindDir->setText(QString::number(myDavisData.wind_Direction));
                        }else{
                            qDebug() << "No Wind Dir Data";
                            ui->labelWindDir->setText("0");
                        }
                        //myDavisData.wind_Direction = hex2dec(temp);
                    }else{
                        qDebug() << "No Wind Dir Data";
                    }

                    //Rain Rate
                    hex[0] = myData.at(42);
                    hex[1] = myData.at(43);
                    snprintf(temp,10,"%X%X", hex[0], hex[1]);
                    temp16 = atoi(temp);
                    tempDouble = temp16/100;
                    snprintf(temp,10,"%f", tempDouble);
                    qDebug() << "Rain rate" << temp;
                    ui->labelRainRate->setText(QString::number(temp16));

                    //UV
                    qDebug() << "UV...";
                    uint8_t rUV = myData.at(44);
                    myDavisData.UV =  rUV;
                    qDebug() << "UV" << myDavisData.UV;

                    //Solar Radiation
                    hex[0] = myData.at(45);
                    hex[1] = myData.at(46);
                    snprintf(temp,10,"%X%X", hex[0], hex[1]);
                    temp16 = atoi(temp);
                    qDebug() << "Solar Radiation " << temp16;

                    //Barometer
                    /*
                    hex[0] = myData.at(8);
                    hex[1] = myData.at(9);
                    snprintf(tempBarometer,10,"%X%X", hex[0], hex[1]);
                    qDebug() << "Barometer rec" << tempBarometer;
                    double tempBar = hex2dec(tempBarometer);
                    tempBar = tempBar/10;
                    // myDavisData.barometer =
                    if((tempBar >= 508) && (tempBar <= 812.8)){ //inside range
                        myDavisData.barometer = tempBar;
                        ui->lblBarometer->setText(QString::number(myDavisData.barometer));
                        qDebug() << "Barometer" << tempBar;
                    }else{ //out of range
                        qDebug() << "Barometer Out of Range " << tempBar;
                        ui->lblBarometer->setText("N/A");
                    }
                    */

                    /*
                    //Temperature inside
                    hex[0] = myData.at(10);
                    hex[1] = myData.at(11);

                    snprintf(temp,10,"%X%X", hex[0], hex[1]);
                    qDebug() << "Temperature inside F temp " << temp;
                    int tempFahrenheitIn = atoi(temp)/10;
                    if(tempFahrenheitIn >= 32){
                        qDebug() << "Temperature inside F " << tempFahrenheitIn;
                        tempDouble = (((tempFahrenheitIn/1)-32)*5)/9;
                        myDavisData.inside_Temperature = tempDouble;
                        ui->labelTemperature->setText(QString::number(myDavisData.inside_Temperature));
                        qDebug() << "Temperature Inside" << tempDouble; //myDavisData.inside_Temperature;
                    }else{
                        ui->labelTemperature->setText("N/A");
                    }
                    */


                    //Temperature Outside
                    /*
                    hex[0] = myData.at(12);
                    snprintf(temp,10,"%X", hex[0]);
                    qDebug() << "temp out hex0" << hex[0];

                    hex[1] = myData.at(13);
                    snprintf(temp,10,"%X", hex[1]);
                    qDebug() << "temp out hex1" << hex[1];

                    //QString(hex);

                    snprintf(temp,10,"%X%X", hex[0], hex[1]);
                    int tempFahrenheitOut = atoi(temp)/10;
                    qDebug() << "Temperature outside F " << tempFahrenheitOut;
                    tempDouble = (((tempFahrenheitOut/1)-32)*5)/9;
                    qDebug() << "Temperature Outside" << tempDouble; //myDavisData.inside_Temperature;

                    */

                    //Humidty Inside
                    char humidity = myData.at(12);
                    myDavisData.inside_Humidity = humidity;
                    ui->labelHumidity->setText(QString::number(myDavisData.inside_Humidity));
                    qDebug() << "Humidity Inside" << myDavisData.inside_Humidity;

                    //Humidty Outside
                    humidity = myData.at(34);
                    myDavisData.outside_Humidity = humidity;
                    ui->labelHumidityOut->setText(QString::number(myDavisData.outside_Humidity));
                    qDebug() << "Humidity Outside" << myDavisData.outside_Humidity ;

                    //console_Battery_Voltage
                    hex[0] = myData.at(88);
                    hex[1] = myData.at(89);
                    snprintf(temp,10,"%X%X", hex[0], hex[1]);
                    //qDebug() << "Barometer rec" << temp;
                    temp16 = hex2dec(temp);
                    tempDouble = (((temp16*300)/512)/100);
                    // myDavisData.barometer =
                    //if((temp16 >= 20) && (temp16 <= 32.5)){ //inside range
                        //myDavisData.barometer = tempBar/100;
                        //ui->lblBarometer->setText(QString::number(myDavisData.barometer));
                    myDavisData.console_Battery_Voltage = tempDouble;
                    qDebug() << "console_Battery_Voltage" << myDavisData.console_Battery_Voltage;
                    //}else{ //out of range
                    //qDebug() << "Barometer Out of Range " << tempBar/100;
                    //ui->lblBarometer->setText("N/A");
                    //}
                    //process_state = GET_HTTP; //Serial data completed, now get http data
                    process_state = GET_HTTP;
                    m_serial->flush();
                    m_serial->clear();
                    myData.chop(n);
                }
                break;
            default:
                break;
        }
	}

}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::callBackTimer()
{
    //Get Date time and use it as filename
    QDateTime local(QDateTime::currentDateTime());
    qDebug() << "clock time is:" << local;
    //Convert to String
    ui->labelClock->setText(local.toString());

    //3 hours occurs, then restart
    if(counter_hours >= 180){
        restartApps();
    }

    counter_minutes++;
    if(counter_minutes >= post_periodic){ //1 minutes elapsed
        counter_minutes = 0;
        counter_hours++;
        port_ctr = 0;
        const auto infos = QSerialPortInfo::availablePorts();
        for(const QSerialPortInfo &info : infos){
            QString s = info.portName();
            //auto label = new QLabel(s);
            qDebug() << "COM PORT = " << s;
            portname[port_ctr] = s;
            port_ctr++;
        }
        //http_get();
    }else{
		counter_second++;
        switch (process_state){
            case COM_SCAN:
                break;
            case COM_OPEN: //test com port
                qDebug() << "Case COM OPEN";
                if(init_port(portname[port_numerator])){ //klo sukses
                    writeData("TEST\r");
                    retry = 0;
                    process_state = COM_WAIT_TEST_RESPONSE;
                    ui->lblStatus->setText("OPEN" + portname[port_numerator]);
                    qDebug() << "Case COM OPEN successfully";
                }else{ //gagal
                    retry++;
                    process_state = COM_OPEN;
                    ui->lblStatus->setText("OPEN" + portname[port_numerator] + "FAIL");
                    if(retry >= 3){ //gagal buka port 3x
                       port_numerator++; //pindah port selanjutnya
                       process_state = COM_OPEN;
                       retry = 0;
                       if(port_numerator > port_ctr){
                           port_numerator = 0;
                       }
                    }
                    qDebug() << "Case COM OPEN fail";
                }
                break;
            case COM_WAIT_TEST_RESPONSE: //periksa string TEST
                qDebug() << "Case COM_WAIT_TEST_RESPONSE";
                if(response_test_valid){ //TEST received, so com port is OK
                    process_state = GET_DATA_SERIAL;
                    qDebug() << "Case Receive TEST successfully";
                }else{
                    process_state = COM_WAIT_TEST_RESPONSE;
                    qDebug() << "Loop waiting TEST Response";
                    retry++;
                    if(retry >= 3){
                       retry = 0;
                       process_state = COM_OPEN;
                       port_numerator++;
                    }
                }
                break;
            case GET_DATA_SERIAL:
                if(counter_second >= 59){
                    counter_second = 0;
                    qDebug() << "case GET_DATA_SERIAL";
                    process_state = WAITING_DATA_SERIAL_COMPLETE;
                    retry = 0;
                    writeData("LOOP 1\r");
                }
                break;
             case WAITING_DATA_SERIAL_COMPLETE:
                qDebug() << "case WAITING_DATA_SERIAL_COMPLETE";
                retry++;
                if(retry >= 3){
                   retry = 0;
                   if(process_state == WAITING_DATA_SERIAL_COMPLETE){
                      process_state = GET_DATA_SERIAL;
                   }
                }
                break;
             case GET_HTTP:
                qDebug() << "case GET_HTTP";
                process_state = WAITING_HTTP_GET;
                citcat = 0;
                mhttp->http_get("http://api.openweathermap.org/data/2.5/weather?lat=-6.9345&lon=107.605&units=metric&APPID=249534879c6a216ed7f97af0af2bc477");
                ui->lblStatus->setText("POSTING");
                break;
             case WAITING_HTTP_GET:
                if(mhttp->httpGetFinished){
                    citcat++;
                    if(citcat > 3){
                        parsingJson();
                        process_state = SEND_POST;
                        citcat = 0;
                    }
                }else{
                    process_state = WAITING_HTTP_GET;
                }
                break;
             case SEND_POST:
                qDebug() << "case SEND_POST, parsing JSON HTTP GET";
                //parsingJson();
                ui->lblStatus->setText("POSTING");
                getDateTime();
                //timerCmd->stop();
                //QString mUrl = ui->lineEditUrl->text();
                //mhttp->http_post_data(mUrl,"");
                postJson(); //Post to Thingsboard HTTP server
                saveJsonData(); //Save to text file
                qDebug() << "LOOP 1\r";
                //process_state = GET_DATA_SERIAL;
                process_state = WAITING_HTTP_POST;
                ui->lblStatus->setText("FLY");
                break;
            case WAITING_HTTP_POST:
                if(mhttp->httpPostFinished == true){
                    process_state = GET_DATA_SERIAL;
                    qDebug()<< "get response\n";

                    QByteArray response_data = mhttp->replyPost->readAll();

                    QJsonDocument jsonResponse = QJsonDocument::fromJson(response_data);
                    qDebug() << response_data;
                    mhttp->replyPost->deleteLater();
                }else{
                    if(!mhttp->replyPost->isFinished()){ //not finished
                        qApp->processEvents(); //waiting
                    }else{
                        mhttp->httpPostFinished = true;
                    }
                }
                break;
            default:
                qDebug() << "case default";
                break;
        }
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::getDateTime(){
    //Get Date time and use it as filename
    QDateTime local(QDateTime::currentDateTime());
    qDebug() << "Local time is:" << local;
    //Convert to String
    myDavisData.timeStamps = local.toString();
    ui->labelIdData->setText(QString::number(myDavisData.id_data));
    ui->labelTimeStamps->setText(myDavisData.timeStamps);
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::restartApps()
{
    QStringList args = QApplication::arguments();
    args.removeFirst();
    QProcess::startDetached(QApplication::applicationFilePath(), args);
    QCoreApplication::quit();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::saveJsonData()
{
    QString path;
    QString jsondata;
    QString folderDay;

    //Check Directory
    QDate date(QDate::currentDate());
    folderDay = date.toString();
    qDebug() << "Date now:" << folderDay;

    //Check if directory exist
    QDir dir1(pathSave + folderDay);
    //if not exist, create it
    if(dir1.exists()){
        qDebug() << "Already exist" << dir1;
    }else{
        //dir1.mkdir(folderDay);
        QDir().mkdir(pathSave + folderDay);
    }

    //Get Date time and use it as filename
    QDateTime local(QDateTime::currentDateTime());
    qDebug() << "Path local time is:" << local;
    //Convert to String
    path = local.toString();
    path.remove(QChar(':'), Qt::CaseInsensitive);

    path = pathSave + folderDay + "/" + path + '\0' + ".txt";

    qDebug() << "path is " << path;
    jsondata = ui->lineJsonData->text();

    qDebug() << "start save";
    fman.fileWrite(path, jsondata);
    qDebug() << "end save";
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::postJson()
{
    /*QJsonObject json;
    json.insert("id_data",myDavisData.id_data);
    json.insert("time_stamps",myDavisData.timeStamps);
    json.insert("barometer",myDavisData.barometer);
    json.insert("humidity", myDavisData.inside_Humidity);
    json.insert("humidity_out", myDavisData.outside_Humidity);
    json.insert("lat", "-6.885156");
    json.insert("long", "107.619142");
    json.insert("temperature", myDavisData.outside_Temperature);
    json.insert("wind_dir",myDavisData.wind_Direction);
    json.insert("wind_speed",myDavisData.wind_Speed);
    json.insert("wind_speed_avg",myDavisData.Ten_Min_Avg_wind_Speed);
    json.insert("rain_rate",myDavisData.rain_Rate);
    */

   // QString strId = QString::number(myDavisData.id_data);

   // QString strJson = "{\"id_data\"strId}";

    //strJson.("{\"id_data\:\"}",)
    char jsonChar[2000];
    /*snprintf(jsonChar,300,"{\"id_data\":s,\"time_stamps\":%s,\"barometer\":%s,\"humidity\":%s,\"humidity_out\":%s,\"lat\":6.885156,\"long\":107.619142,\"temperature\":%s,\"wind_dir\":%s,\"wind_speed\":%s,\"wind_speed_avg\":%s,\"rain_rate\":%s\"}",
                            myDavisData.id_data,
                            //myDavisData.timeStamps,
                            myDavisData.barometer,
                            myDavisData.inside_Humidity,
                            myDavisData.outside_Humidity,
                            myDavisData.outside_Temperature,
                            myDavisData.wind_Direction,
                            myDavisData.wind_Speed,
                            myDavisData.Ten_Min_Avg_wind_Speed,
                            myDavisData.rain_Rate);
    */

    qDebug() << "jtime " ;
    unsigned char timeStampChar[50];
    myDavisData.timeStamps = '"' + myDavisData.timeStamps + '"' + '\0';
    memcpy( timeStampChar, myDavisData.timeStamps.toStdString().c_str() ,myDavisData.timeStamps.size());

    qDebug() << "jtime " ;

    snprintf(jsonChar,2000,"{"
                          "\"id_data\":%d,"
                           "\"time_stamps\":%s,"
                           "\"barometer\":%.2f,"
                           "\"humidity\":%f.2,"
                           "\"humidity_out\":%.2f,"
                           "\"lat\":6.885156,"
                           "\"long\":107.619142,"
                           "\"temperature\":%.2f,"
                           "\"wind_dir\":%d,"
                           "\"wind_speed\":%.2f,"
                           "\"wind_speed_avg\":%.2f,"
                          "\"rain_rate\":%.2f"
                          "}",
                            myDavisData.id_data,
                            timeStampChar,
                            myDavisData.barometer,
                            myDavisData.inside_Humidity,
                            myDavisData.outside_Humidity,
                            myDavisData.outside_Temperature,
                            myDavisData.wind_Direction,
                            myDavisData.wind_Speed,
                            myDavisData.Ten_Min_Avg_wind_Speed,
                            myDavisData.rain_Rate);

    qDebug() << "json data " << jsonChar;

    QString strData(jsonChar);

    qDebug() << "json data string " << jsonChar;
    qDebug() << "Json data is " << strData;

    ui->lineJsonData->setText(strData);

    myDavisData.id_data++;
    ui->labelIdData->setText(QString::number(myDavisData.id_data));

    QString strUrl = ui->lineEditUrl->text();
    //mhttp->http_post_data(strUrl,QString(QJsonDocument(json).toJson()));
    mhttp->http_post_data(strUrl,strData);
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::getHttp()
{

}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::parsingJson()
{
    qDebug() << "Start Parsing...";

    bool ok;
    JsonObject result = QtJson::parse(mhttp->strHttpGetReply, ok).toMap();

    //https://github.com/qt-json/qt-json
    QtJson::JsonObject nested = result["main"].toMap();
    myDavisData.outside_Temperature = nested["temp"].toDouble();
    myDavisData.barometer = nested["pressure"].toDouble();

    qDebug() << "Temperature " << myDavisData.outside_Temperature;
    qDebug() << "Barometer " << myDavisData.barometer ;

    ui->labelTemperature->setText(QString::number(myDavisData.outside_Temperature));
    ui->lblBarometer->setText(QString::number(myDavisData.barometer));

    //disconnect(nam, SIGNAL(finished(QNetworkReply*)),this,SLOT(serviceRequestFinished(QNetworkReply*)));
    qDebug() << "End Parsing...";
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        //QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
         process_state = COM_OPEN;
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen()){
        m_serial->close();
        qDebug() << "Close OK\n";
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
uint16_t MainWindow::hex2dec(char hex[10]){
    long long decimal, place;
    int i = 0, val, len;

    decimal = 0;
    place = 1;

    /* Input hexadecimal number from user */
    // printf("Enter any hexadecimal number: ");
    //gets(hex);

    /* Find the length of total number of hex digit */
    len = strlen(hex);
    len--;

    /*
     * Iterate over each hex digit
     */
     for(i=0; hex[i]!='\0'; i++)
     {

            /* Find the decimal representation of hex[i] */
            if(hex[i]>='0' && hex[i]<='9')
            {
                val = hex[i] - 48;
            }
            else if(hex[i]>='a' && hex[i]<='f')
            {
                val = hex[i] - 97 + 10;
            }
            else if(hex[i]>='A' && hex[i]<='F')
            {
                val = hex[i] - 65 + 10;
            }

            decimal += val * pow(16, len);
            len--;
        }

     return decimal;
        //printf("Hexadecimal number = %s\n", hex);
        //printf("Decimal number = %lld", decimal);
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::on_btnExit_clicked()
{
    QApplication::quit();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
/*double MainWindow::http_get(void)
{
    qDebug() << "begin get\n";
    nam = new QNetworkAccessManager(this);

    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

    connect(nam, SIGNAL(finished(QNetworkReply*)),this,SLOT(serviceRequestFinished(QNetworkReply*)));

    QUrl url("http://api.openweathermap.org/data/2.5/weather?lat=-6.9345&lon=107.605&units=metric&APPID=249534879c6a216ed7f97af0af2bc477");
    QNetworkReply* reply = nam->get(QNetworkRequest(url));
    //replyGet = namGet->get(QNetworkRequest(url));
}
*/

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
/*void MainWindow::serviceRequestFinished(QNetworkReply* reply)
{
    if(reply->error() == QNetworkReply::NoError) {

        QStringList propertyNames;
        QStringList propertyKeys;

        QString strRply = (QString)reply->readAll();

        qDebug() << strRply;

        qDebug() << "Start Parsing...";

        bool ok;
        JsonObject result = QtJson::parse(strRply, ok).toMap();

        //https://github.com/qt-json/qt-json
        QtJson::JsonObject nested = result["main"].toMap();
        myDavisData.outside_Temperature = nested["temp"].toDouble();
        myDavisData.barometer = nested["pressure"].toDouble();

        qDebug() << "Temperature " << myDavisData.outside_Temperature;
        qDebug() << "Barometer " << myDavisData.barometer ;

        ui->labelTemperature->setText(QString::number(myDavisData.outside_Temperature));
        ui->lblBarometer->setText(QString::number(myDavisData.barometer));

        disconnect(nam, SIGNAL(finished(QNetworkReply*)),this,SLOT(serviceRequestFinished(QNetworkReply*)));
        qDebug() << "End Parsing...";
        //process_state = SEND_POST;
    } else {
        qDebug() << "ERROR";
        //process_state = GET_HTTP;
    }

    delete reply;
}
*/

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::on_btnRestart_clicked()
{
    //QProcess::startDetached(QApplication::applicationFilePath());
    //exit(12);
    QStringList args = QApplication::arguments();
    args.removeFirst();
    QProcess::startDetached(QApplication::applicationFilePath(), args);
    QCoreApplication::quit();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::on_checkBoxLock_stateChanged(int arg1)
{
    if(ui->checkBoxLock->checkState()){
        ui->lineEditUrl->setEnabled(true);
    }else{
        ui->lineEditUrl->setEnabled(false);
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::on_btnTest_clicked()
{
    parsingJson();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void MainWindow::on_btnPost_clicked()
{
    //mhttp->http_post_data("https://cloud.thingsboard.io/api/v1/3A99ym139IT0jboYovSr/telemetry",
    //                      "{\"timestamp\":  \"Wed Aug 20 09:13:12 2019\",\"humidity\": 43.00,\"temperature\":37.00,\"light\":3.00}");
    getDateTime();
    postJson();
}

/****************************************************************
 ****************************************************************
 ****************************************************************/
void MainWindow::loadSettings()
{
    // Init config
    config.setFile("config.ini");

    qDebug() << "server url = " << config.getURLServer();
    ui->lineEditUrl->setText(config.getURLServer());

    //qDebug() << "periodic = " << config.getPeriodic();
    post_periodic = config.getPeriodic().toInt();
    qDebug() << "periodic = " << post_periodic;

    ui->labelPeriod->setText(config.getPeriodic());

    pathSave = config.getPathSave();
    qDebug() << "pathSave = " << pathSave;
    ui->linePathSave->setText(pathSave);
}
