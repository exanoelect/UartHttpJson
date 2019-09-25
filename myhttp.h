#ifndef MYHTTP_H
#define MYHTTP_H

#include <QObject>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

struct http_data{
    
};

class myhttp : public QObject
{
    Q_OBJECT
public:
    explicit myhttp(QObject *parent = nullptr);
    void http_post_data(QString mUrl, QString datStr);
    void http_get(QString strUrl);
    QString strHttpGetReply;
    bool httpGetFinished;
    bool httpPostFinished;
    QNetworkReply *replyPost;
signals:

private slots:
    void serviceRequestFinished(QNetworkReply* reply);

public slots:
    void replyFinished(QNetworkReply *reply);
private:
    QNetworkAccessManager *nam;
};

#endif // HTTP_H
