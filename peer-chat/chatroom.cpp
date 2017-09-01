#include "chatroom.h"
#include "identity.h"
#include "message.h"

ChatRoom::ChatRoom()
{

}

ChatRoom::ChatRoom(QString n)
{
    ChatName = n;
    Participants = QVector<Identity*>();
    Messages = QVector<Message*>();
}

ChatRoom::ChatRoom(QString n, QVector<Identity*> p, QVector<Message*> m)
{
    ChatName = n;
    Participants = p;
    Messages = m;
}

QDataStream &operator<<(QDataStream &out, ChatRoom &c)
{
    out << c.ChatName << c.Participants.size();
    for(auto i : c.Participants)
        out << *i;
    out << c.Messages.size();
    for(auto m : c.Messages)
        out << *m;
    return out;
}

QDataStream &operator>>(QDataStream &in, ChatRoom &c)
{
    int participantsSize, messagesSize;
    QString chatName;
    auto participants = QVector<Identity*>();
    auto messages = QVector<Message*>();
    Identity participant;
    Message message;
    in >> chatName >> participantsSize;
    for(int i = 0; i < participantsSize; i++)
    {
        in >> participant;
        participants.push_back(new Identity(participant.Uuid(), participant.Name, participant.IpAddress, participant.Port));
    }
    in >> messagesSize;
    for(int i = 0; i < messagesSize; i++)
    {
        in >> message;
        auto newMessage = new Message();
        newMessage->Sender = message.Sender;
        newMessage->Time = message.Time;
        newMessage->Type = message.Type;
        newMessage->Content = message.Content;
        messages.push_back(newMessage);
    }
    c = ChatRoom(chatName, participants, messages);
    return in;
}
