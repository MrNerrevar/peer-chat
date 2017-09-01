#ifndef CHATROOM_H
#define CHATROOM_H
#include <QVector>
#include <QString>

class Identity;
class Message;
class ChatRoom
{
public:
    ChatRoom();
    ChatRoom(QString);
    ChatRoom(QString, QVector<Identity*>, QVector<Message*>);

    QString ChatName;
    QVector<Identity*> Participants;
    QVector<Message*> Messages;
};
QDataStream &operator<<(QDataStream &, ChatRoom &);
QDataStream &operator>>(QDataStream &, ChatRoom &);

#endif // CHATROOM_H
