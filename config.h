#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QSettings>

class Config
{
public:
    Config();

    QString getURLServer();
    QString getPeriodic();
    QString getPathSave();
    //QString getUser();
    //QString getPassword();
    //QString getRecipant();
    // QString getDbPassword();
    // QString getComport();
    // QString getBaudrate();
    // QString getPictures();
    // QString getIpCamUrl();
    // QString getIpCamVidUrl();
    // QString getLokasi();
    // QString getAlamat();

    void setFile(QString fileName);
    // void server(QString hostName, QString database, QString user, QString password);
    // void hardware(QString comport, QString baudrate);
    // void setHostName(QString hostName);
    // void setDatabase(QString dbName);
    // void setDbUser(QString userName);
    // void setDbPassword(QString password);
    // void setComport(QString comport);
    // void setBaudrate(QString baudrate);
    // void setPictures(QString url);

private:
    QString fileConfig;
    //QString strHost, strDb, strDbUser, strDbPwd, strComport, strCamera, strPictures;
	QString strSmtpServer, strPort, strUser, strPwd, strRecipant;//, strCamera, strPictures;
    QSettings *settings;
};

#endif // CONFIG_H
