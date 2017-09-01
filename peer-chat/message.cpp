#include "message.h"
#include <QDebug>

QString Message::IntroductionA = "0A";
QString Message::IntroductionB = "0B";
QString Message::ReconnectA = "00";
QString Message::ReconnectB = "01";
QString Message::CreateChatRoom = "0C";
QString Message::MessageChatRoom = "0D";
QString Message::LeaveChatRoom = "0E";

int Message::UuidLength = QString("{00000000-0000-0000-0000-000000000000}").length();
int Message::ISODateLength = QString("1997-07-16T19:20:30+01:00").length();

Message::Message()
{

}

QString Message::ToString()
{
    return "**" + Sender.toString() +
           "[" + QString::number(Time.toString(Qt::ISODate).length()) + "]" +
           "[" + Time.toString(Qt::ISODate) + "]" +
            "(" + Type + ")" +
            ":" + Content;
}

Message* Message::FromString(QString s)
{
    //**{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}[25][1997-07-16T19:20:30+01:00](0A):Soren Sorenson
    auto m = new Message();
    auto index = 2;
    m->Sender = QUuid(s.mid(index, UuidLength));

    index = 2 + UuidLength + 1;
    auto timeLength = s.mid(index, 2).toInt();

    index = 2 + UuidLength + 5;
    m->Time   = QDateTime::fromString(s.mid(index, timeLength), Qt::ISODate);

    index = 2 + UuidLength + 5 + timeLength + 2;
    m->Type   = s.mid(index, 2);

    index = 2 + UuidLength + 5 + timeLength + 6;
    m->Content = s.mid(index, s.length() - index);
    return m;
}

QDataStream& operator<<(QDataStream& out, Message& m)
{
    out << m.ToString();
    return out;
}

QDataStream& operator>>(QDataStream& in, Message& m)
{
    QString mString;
    in >> mString;
    m = *Message::FromString(mString);
    return in;
}
