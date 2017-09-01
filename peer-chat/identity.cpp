#include "identity.h"

Identity::Identity(QString n, QString a, QString p) : Identity(QUuid::createUuid(), n, a, p) { }

Identity::Identity(QString a, QString p)
{
    IpAddress = a;
    Port = p;
    Name = "Unknown";
}

Identity::Identity(QUuid i, QString n, QString a, QString p)
{
    _id = i;
    Name = n;
    IpAddress = a;
    Port = p;
}

Identity::Identity(QString i, QString n, QString a, QString p) : Identity(QUuid(i), n, a, p) { }

Identity::Identity()
{

}

QString Identity::Uuid()
{
    return _id.toString();
}

void  Identity::setUuid(QString s)
{
    _id = QUuid(s);
}

void Identity::setUuid(QUuid i)
{
    _id = i;
}

QString Identity::ToAddress()
{
    return ToAddress(this);
}

QString Identity::ToAddress(Identity* i)
{
    return ToAddress(i->IpAddress, i->Port);
}

QString Identity::ToAddress(QString a, QString p)
{
    if(a == "localhost")
        return "127.0.0.1:" + p;
    return a + ":" + p;
}

QDataStream& operator<<(QDataStream& out, Identity& i)
{
    out << i.Name << i.IpAddress << i.Port << i.Uuid();
    return out;
}

QDataStream& operator>>(QDataStream& in, Identity& i)
{
    QString name, ipAddress, port, uuid;
    in >> name >> ipAddress >> port >> uuid;
    i = Identity(uuid, name, ipAddress, port);
    return in;
}
