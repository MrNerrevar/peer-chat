#ifndef MESSAGE_H
#define MESSAGE_H
#include <QString>
#include <QUuid>
#include <QTime>

class Message
{
public:
    Message();

    static QString IntroductionA;
    static QString IntroductionB;
    static QString ReconnectA;
    static QString ReconnectB;
    static QString CreateChatRoom;
    static QString MessageChatRoom;
    static QString LeaveChatRoom;
    static int UuidLength;
    static int ISODateLength;

    QUuid Sender;
    QDateTime Time; //Qt::ISODate
    QString Type;
    QString Content;

    QString ToString();
    static Message* FromString(QString);
};
QDataStream& operator<<(QDataStream&, Message&);
QDataStream& operator>>(QDataStream&, Message&);

#endif // MESSAGE_H
