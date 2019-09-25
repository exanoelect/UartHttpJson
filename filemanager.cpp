#include "filemanager.h"
#include <QDir>
#include <qdir.h>
#include <QtDebug>
#include <QDebug>
//#include "mainwindow.h"
//#include "ui_mainwindow.h"

//extern Ui::MainWindow *ui;

filemanager::filemanager(QObject *parent) : QObject(parent)
{

}

QString filemanager::fileFirstFound(QString dir)
{
    //QString path = "D:/ZIO";
    QDir dir_path(dir);
    QString str;

     foreach(QFileInfo item, dir_path.entryInfoList())
     {
    //QFileInfo item;
         if(item.isDir())
             qDebug() << "Dir: " << item.absoluteFilePath();
         if(item.isFile()){
             //qDebug() << "File: " << item.fileName();
             str = item.fileName();
             qDebug() << "File: " << str;

             qDebug() << "File: " << item.size();
             //qDebug() << "File: " << item.;
             //qDebug() << "File: " << item.;
             return str;
         }
         //if(item.)
         //qDebug() << "File: " << item.absoluteFilePath();
     }
         return "NULL";
}

bool filemanager::fileWrite(QString path, QString data)
{
    //bool openok = false;
    QFile file(path); //"D://ZIO/out.txt");
    //QFile file(path); //"D://ZIO/out.txt");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        qDebug() << "open fail";
        return false;
    }

    filemanager fman;
    QString fileStr;
    QTextStream out(&file);

    //    int i = 0;

/*    for(i=0;i<ui->lw_data->count();i++){
        //qDebug() << list_result->item(i)->text().toFloat();
        fileStr = ui->lw_data->item(i)->text();//ui->lw_data->item(i)->text();
        out << fileStr;
        qDebug() << fileStr;
    }
*/

          //QTextStream out(&file);
          //out << "The magic number is: " << 49 << "\n";
          out << data;
          //out << data;
          //out << data;
          //out << data;
          //qDebug() << "open OK";
          return true;

      //}else{

      //}
}

QString filemanager::fileRead(QString path)
{
    QFile file(path); //("D://ZIO/out.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "NULL";

    QTextStream in(&file);
    //!in.readAll()) {
        QString line = in.readLine();
        qDebug() << "line.." << line;
        //process_line(line);
        return line;
    //}
}

bool filemanager::fileDelete(QString path)
{
    QFile file(path); //"D://ZIO/out.txt");
    file.remove();
    return true;
}

