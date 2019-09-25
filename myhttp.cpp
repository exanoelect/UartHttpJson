#include "myhttp.h"
#include "mainwindow.h"

myhttp::myhttp(QObject *parent) : QObject(parent)
{

}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void myhttp::http_post_data(QString mUrl, QString datStr)
{
    qDebug() << "begin url" << mUrl;

    QNetworkRequest request;
    request.setUrl(mUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

    qDebug() << "begin json data :" << datStr;

    char charTemp[2000] = {'\0'};
    char *ptr = 0;

    //copy str to char array
    memcpy(charTemp,datStr.toStdString().c_str(),datStr.size());
    //assign ptr
    ptr = charTemp;

    nam = new QNetworkAccessManager(this);

    connect(nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));

    qDebug() << "begin post\n";

    try{
        httpPostFinished = false;
        replyPost = nam->post(request,ptr);
        //while (!replyPost->isFinished())
        //{
        //    qApp->processEvents();
        //}

        //qDebug()<< "get response\n";

        //QByteArray response_data = replyPost->readAll();

        //QJsonDocument jsonResponse = QJsonDocument::fromJson(response_data);
        //qDebug() << response_data;
        //replyPost->deleteLater();
    }catch (exception &error) {
        //if (error == QSerialPort::ResourceError) {
         //   QMessageBox::critical(this, tr("Critical Error"), qApp->);
        //restartApps();
    }
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void myhttp::replyFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    }
    else
    {
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

        disconnect(nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyFinished(QNetworkReply*)));

        /*QFile *file = new QFile("C:/Qt/Dummy/downloaded.txt");
        if(file->open(QFile::Append))
        {
            file->write(reply->readAll());
            file->flush();
            file->close();
        }
        delete file;
        */
    }

    reply->deleteLater();
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void myhttp::http_get(QString strUrl)
{
    qDebug() << "begin get\n";
    nam = new QNetworkAccessManager(this);

    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

    connect(nam, SIGNAL(finished(QNetworkReply*)),this,SLOT(serviceRequestFinished(QNetworkReply*)));

    //QUrl url("http://api.openweathermap.org/data/2.5/weather?lat=-6.9345&lon=107.605&units=metric&APPID=249534879c6a216ed7f97af0af2bc477");
    //QNetworkReply* reply =
    QUrl url(strUrl);
    nam->get(QNetworkRequest(url));
    httpGetFinished = false;
    //replyGet = namGet->get(QNetworkRequest(url));
}

/*****************************************************************************************************
**--------------------------------------------------------------------------------------------------**
**--------------------------------------------------------------------------------------------------**
******************************************************************************************************/
void myhttp::serviceRequestFinished(QNetworkReply* reply)
{
    if(reply->error() == QNetworkReply::NoError) {

        QStringList propertyNames;
        QStringList propertyKeys;

        QString strRply = (QString)reply->readAll();

        strHttpGetReply = strRply;

        //qDebug() << strRply;
        qDebug() << strHttpGetReply;

        qDebug() << "GET HTTP OK";

        bool ok;
        /*
        JsonObject result = QtJson::parse(strRply, ok).toMap();

        //https://github.com/qt-json/qt-json
        QtJson::JsonObject nested = result["main"].toMap();
        myDavisData.outside_Temperature = nested["temp"].toDouble();
        myDavisData.barometer = nested["pressure"].toDouble();

        qDebug() << "Temperature " << myDavisData.outside_Temperature;
        qDebug() << "Barometer " << myDavisData.barometer ;

        ui->labelTemperature->setText(QString::number(myDavisData.outside_Temperature));
        ui->lblBarometer->setText(QString::number(myDavisData.barometer));
*/
        disconnect(nam, SIGNAL(finished(QNetworkReply*)),this,SLOT(serviceRequestFinished(QNetworkReply*)));
        qDebug() << "DISCONNECT SIGNAL HTTP GET";
        httpGetFinished = true;
        //process_state = SEND_POST;
    } else {
        qDebug() << "ERROR";
        //process_state = GET_HTTP;
    }

    //delete reply;
    reply->deleteLater();
}
