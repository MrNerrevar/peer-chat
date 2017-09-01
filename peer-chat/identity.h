#ifndef IDENTITY_H
#define IDENTITY_H
#include <QUuid>
#include <QString>
#include <QDataStream>

class Identity
{
private:
    QUuid _id;
public:
    Identity(QString, QString);
    Identity(QString, QString, QString);
    Identity(QUuid, QString, QString, QString);
    Identity(QString, QString, QString, QString);
    Identity();

    QString Name;
    QString IpAddress;
    QString Port;
    QString Uuid();
    void setUuid(QString);
    void setUuid(QUuid);
    QString ToAddress();
    static QString ToAddress(Identity*);
    static QString ToAddress(QString, QString);
};
QDataStream& operator<<(QDataStream&, Identity&);
QDataStream& operator>>(QDataStream&, Identity&);

#endif // IDENTITY_H
