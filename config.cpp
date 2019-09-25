#include "config.h"

Config::Config()
{

}

void Config::setFile(QString fileName)
{
    settings = new QSettings(fileName, QSettings::IniFormat);
    settings->beginGroup("SERVER");
    settings->setValue("URL", settings->value("URL","").toString());
    settings->setValue("PERIODIC", settings->value("PERIODIC","").toString());
    settings->setValue("PATH", settings->value("PATH","").toString());
    //settings->setValue("USER", settings->value("USER","").toString());
    //settings->setValue("PASSWORD", settings->value("PASSWORD","").toString());
    //settings->setValue("RECIPANT", settings->value("RECIPANT","").toString());
    settings->endGroup();
}

// void Config::server(QString hostName, QString database, QString user, QString password)
// {
    // settings->beginGroup("SERVER");
    // settings->setValue("HOSTNAME", hostName);
    // settings->setValue("DATABASE", database);
    // settings->setValue("USER", user);
    // settings->setValue("PASSWORD", password);
    // settings->endGroup();
// }

// void Config::hardware(QString comport, QString baudrate)
// {
    // settings->beginGroup("HARDWARE");
    // settings->setValue("COMPORT", comport);
    // settings->setValue("BAUDRATE", baudrate);
    // settings->endGroup();
// }

// void Config::setHostName(QString hostName)
// {
    // settings->beginGroup("SERVER");
    // settings->setValue("HOSTNAME", hostName);
    // settings->endGroup();
// }

// void Config::setDatabase(QString dbName)
// {
    // settings->beginGroup("SERVER");
    // settings->setValue("DATABASE", dbName);
    // settings->endGroup();
// }

// void Config::setDbUser(QString userName)
// {
    // settings->beginGroup("SERVER");
    // settings->setValue("USER", userName);
    // settings->endGroup();
// }

// void Config::setDbPassword(QString password)
// {
    // settings->beginGroup("SERVER");
    // settings->setValue("PASSWORD", password);
    // settings->endGroup();
// }

// void Config::setComport(QString comport)
// {
    // settings->beginGroup("HARDWARE");
    // settings->setValue("COMPORT", comport);
    // settings->endGroup();
// }

// void Config::setBaudrate(QString baudrate)
// {
    // settings->beginGroup("HARDWARE");
    // settings->setValue("BAUDRATE", baudrate);
    // settings->endGroup();
// }

// void Config::setPictures(QString url)
// {
    // settings->beginGroup("SERVER");
    // settings->setValue("PICTURES", url);
    // settings->endGroup();
// }

// QString Config::getHostName()
// {
    // settings->beginGroup("SERVER");
    // QString read = settings->value("HOSTNAME","").toString();
    // settings->endGroup();
    // return read;
// }

QString Config::getURLServer()
{
    settings->beginGroup("SERVER");
    QString read = settings->value("URL","").toString();
    settings->endGroup();
    return read;
}

QString Config::getPeriodic()
{
    settings->beginGroup("SERVER");
    QString read = settings->value("PERIODIC","").toString();
    settings->endGroup();
    return read;
}

QString Config::getPathSave()
{
    settings->beginGroup("SERVER");
    QString read = settings->value("PATH","").toString();
    settings->endGroup();
    return read;
}

// QString Config::getDatabase()
// {
    // settings->beginGroup("SERVER");
    // QString read = settings->value("DATABASE","").toString();
    // settings->endGroup();
    // return read;
// }

// QString Config::getDbUser()
// {
    // settings->beginGroup("SERVER");
    // QString read = settings->value("USER","").toString();
    // settings->endGroup();
    // return read;
// }

// QString Config::getDbPassword()
// {
    // settings->beginGroup("SERVER");
    // QString read = settings->value("PASSWORD","").toString();
    // settings->endGroup();
    // return read;
// }

// QString Config::getComport()
// {
    // settings->beginGroup("HARDWARE");
    // QString read = settings->value("COMPORT","").toString();
    // settings->endGroup();
    // return read;
// }

// QString Config::getBaudrate()
// {
    // settings->beginGroup("HARDWARE");
    // QString read = settings->value("BAUDRATE","").toString();
    // settings->endGroup();
    // return read;
// }

// QString Config::getPictures()
// {
    // settings->beginGroup("SERVER");
    // QString read = settings->value("PICTURES","").toString();
    // settings->endGroup();
    // return read;
// }

// QString Config::getIpCamUrl()
// {
    // settings->beginGroup("HARDWARE");
    // QString read = settings->value("CAMERAURL","").toString();
    // settings->endGroup();
    // return read;
// }

// QString Config::getLokasi()
// {
    // settings->beginGroup("PARKIR");
    // QString read = settings->value("LOKASI","").toString();
    // settings->endGroup();
    // return read;
// }

// QString Config::getAlamat()
// {
    // settings->beginGroup("PARKIR");
    // QString read = settings->value("ALAMAT","").toString();
    // settings->endGroup();
    // return read;
// }

// QString Config::getIpCamVidUrl()
// {
    // settings->beginGroup("HARDWARE");
    // QString read = settings->value("IPCAMURL","").toString();
    // settings->endGroup();
    // return read;
// }

