#include "chathouse.h"
#include "peer.h"
#include "message.h"
#include "identity.h"
#include "chatroom.h"
#include <algorithm>
#include <QString>
#include <stdio.h>
#include <random>

ChatHouse::ChatHouse(QObject *p) : QObject(p)
{

}

ChatHouse::ChatHouse(QString i, QString p)
{
    Friends = QVector<Identity*>();
    ChatRooms = QVector<ChatRoom*>();
    Requests = QVector<Message*>();
    PendingFriends = QVector<Identity*>();
    //TODO Load name if exists else create new
    house = new Identity(i, "localhost", p);
    communicator = new Peer(this, house);
    isLoadedFromSave = false;
    connect(communicator, SIGNAL(read(Message*)), this, SLOT(read(Message*)));
    connect(this, SIGNAL(MessageEvent()), this, SLOT(OnMessageEvent()));
    connect(this, SIGNAL(NewFriendEvent()), this, SLOT(OnMessageEvent()));
    connect(this, SIGNAL(NewChatRoomEvent()), this, SLOT(OnMessageEvent()));
}

ChatHouse::ChatHouse()
{
    Friends = QVector<Identity*>();
    ChatRooms = QVector<ChatRoom*>();
    Requests = QVector<Message*>();
    PendingFriends = QVector<Identity*>();
}

void ChatHouse::read(Message* m)
{
    //emit MessageEvent(m);
    if(m->Type == Message::ReconnectA || m->Type == Message::ReconnectB)
        readReconnectRequest(m);
    if(m->Type == Message::IntroductionA)
        readFriendRequest(m);
    if(m->Type == Message::IntroductionB)
        readFriendRequestReply(m);
    if(m->Type == Message::MessageChatRoom)
        readMessageChatRoom(m);
    if(m->Type == Message::CreateChatRoom)
        readCreateChatRoom(m);    
}

void ChatHouse::readFriendRequest(Message* m)
{
    auto i = readFriendRequestHelper(m);
    if(i == nullptr)
        return;

    auto f = std::find_if(Friends.begin(), Friends.end(), [i](Identity* i2){ return i->Uuid().compare(i2->Uuid()) == 0;});
    if(f != Friends.end())
    {
        writeFriendRequestReply(i->IpAddress, i->Port);
        return;
    }

    Friends.push_back(i);
    Requests.push_back(m);
    writeFriendRequestReply(i->IpAddress, i->Port);
    emit NewFriendEvent();
}

void ChatHouse::readFriendRequestReply(Message* m)
{
    auto i = readFriendRequestHelper(m);
    if(i == nullptr)
        return;

    auto f = std::find_if(PendingFriends.begin(), PendingFriends.end(), [i](Identity* i2){ return i->IpAddress == i2->IpAddress && i->Port == i2->Port; });
    PendingFriends.erase(std::remove(PendingFriends.begin(), PendingFriends.end(), *f), PendingFriends.end());
    Friends.push_back(i);
    emit NewFriendEvent();
    emit NewPendingFriendEvent();
}

void ChatHouse::readReconnectRequest(Message* m)
{
    auto uuid = m->Sender.toString();
    auto fpf = std::find_if(PendingFriends.begin(), PendingFriends.end(), [uuid](Identity* i){ return i->Uuid().compare(uuid) == 0; });
    auto ffi = std::find_if(Friends.begin(), Friends.end(), [uuid](Identity* i){ return i->Uuid().compare(uuid) == 0; });
    auto isInFriends = ffi != Friends.end();
    auto isInPending = fpf != PendingFriends.end();
    auto isReconnectA = m->Type == Message::ReconnectA;

    if(isInPending)
    {
        PendingFriends.erase(std::remove(PendingFriends.begin(), PendingFriends.end(), *fpf), PendingFriends.end());
        emit NewPendingFriendEvent();
    }
    if(!isReconnectA)
    {
        auto i = readFriendRequestHelper(m);
        writeReconnectRequest(i, Message::ReconnectB);
    }
    if((isInPending && !isInFriends) || (!isReconnectA && !isInFriends))
    {
        Friends.push_back(*fpf);
        emit NewFriendEvent();
    }
    emit OnMessageEvent();
}

Identity* ChatHouse::readFriendRequestHelper(Message* m)
{
    auto s = m->Content.split(";");
    if(s.length() != 2)
        return nullptr;
    auto ap = s[1].split(":");
    //Name;123.123.312.123:12313
    auto i = std::find_if(PendingFriends.begin(), PendingFriends.end(), [ap](Identity* i){ return i->IpAddress == ap[0] && i->Port == ap[1]; });
    if(i == PendingFriends.end())
        return new Identity(m->Sender, s[0], ap[0], ap[1]);
    (*i)->Name = s[0];
    (*i)->setUuid(m->Sender);
    return *i;
}

void ChatHouse::readCreateChatRoom(Message* m)
{
    auto s = m->Content.split(",");
    auto chatRoom = new ChatRoom(s[0]);
    for(int i = 0; i < s.length(); i++)
    {
        auto idString = i == 0 ? m->Sender.toString() : s[i];
        auto id = std::find_if(Friends.begin(), Friends.end(), [idString](Identity* pi){ return pi->Uuid().compare(idString) == 0; });
        //TODO Add Friends of Unknown Participants
        if(id == Friends.end())
            continue;
        chatRoom->Participants.push_back(*id);
    }
    chatRoom->Messages.push_back(m);
    ChatRooms.push_back(chatRoom);
    emit NewChatRoomEvent();
}

void ChatHouse::readMessageChatRoom(Message* m)
{
    auto s = m->Content.split("::");
    auto chatRoomName = s[0];
    auto chatRoom = std::find_if(ChatRooms.begin(), ChatRooms.end(), [chatRoomName](ChatRoom* r){return r->ChatName.compare(chatRoomName) == 0;});
    if(chatRoom == ChatRooms.end())
        Requests.push_back(m);
    else
    {
        (*chatRoom)->Messages.push_back(m);
        emit MessageEvent();
    }
}


QString ChatHouse::GetName(const QString& uuid)
{
    if(uuid == house->Uuid())
        return house->Name;

    auto fi = std::find_if(Friends.begin(), Friends.end(), [uuid](Identity* i){ return i->Uuid().compare(uuid) == 0;});
    if(fi == Friends.end())
        return "Unknown";
    return (*fi)->Name;
}

ChatRoom* ChatHouse::GetChatRoom(const QString & name)
{
    auto fi = std::find_if(ChatRooms.begin(), ChatRooms.end(), [name](ChatRoom* i){ return i->ChatName.compare(name) == 0;});
    if(fi == ChatRooms.end())
        return nullptr;
    return (*fi);
}

QString ChatHouse::GetHouseName()
{
    return house->Name;
}

QString ChatHouse::GetHousePort()
{
    return house->Port;
}

QString ChatHouse::GetHouseUuid()
{
    return house->Uuid();
}

void ChatHouse::SetHouseUuid(QString u)
{
    house->setUuid(u);
}

void ChatHouse::SetIsLoadedFromSave()
{
    isLoadedFromSave = true;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnTimeoutEvent()));

    timer->setInterval(rand() % 400 + 500);
    timer->start();
}

void ChatHouse::write(Message* m, Identity* i)
{
    communicator->connectTo(i);
    communicator->write(m, i);
    //emit MessageEvent(m);
}

void ChatHouse::OnMessageEvent()
{
    auto fileName = qApp->applicationDirPath() + "/" + this->GetHouseName() + ".ch";
    SaveChatHouse(fileName, this);
}

void ChatHouse::OnTimeoutEvent()
{
    for(auto f : PendingFriends)
        writeReconnectRequest(f, Message::ReconnectA);

    for(auto pf : PendingFriends)
        for(auto f : Friends)
        {
            if(f->ToAddress().compare(pf->ToAddress()))
            {
                PendingFriends.erase(std::remove(PendingFriends.begin(), PendingFriends.end(), pf), PendingFriends.end());
            }
        }

    if(PendingFriends.size() == 0)
    {
        timer->stop();
        OnMessageEvent();
    }
}

Identity* ChatHouse::writeFriendRequestHelper(QString a, QString p, QString messageType)
{
    auto i = new Identity();
    i->IpAddress = a;
    i->Port = p;

    auto message = new Message();
    message->Sender = house->Uuid();
    message->Time = QDateTime::currentDateTime();
    message->Type = messageType;
    //Name;123.123.312.123:12313
    message->Content = house->Name + ";" + house->IpAddress + ":" + house->Port;
    Requests.push_back(message);

    write(message, i);
    return i;
}

void ChatHouse::writeReconnectRequest(Identity* i, QString t)
{
    auto f = std::find_if(Friends.begin(), Friends.end(), [i](Identity* i2){ return i->Uuid().compare(i2->Uuid()) == 0; });
    if(f != Friends.end() && t != Message::ReconnectB)
        return;

    writeFriendRequestHelper(i->IpAddress, i->Port, t);
    emit NewPendingFriendEvent();
}

void ChatHouse::writeFriendRequest(QString a, QString p)
{    
    auto f = std::find_if(Friends.begin(), Friends.end(), [a, p](Identity* i){ return i->IpAddress == a && i->Port == p; });
    if(f != Friends.end())
        return;

    auto i = writeFriendRequestHelper(a, p, Message::IntroductionA);
    PendingFriends.push_back(i);
    emit NewPendingFriendEvent();
}

void ChatHouse::writeFriendRequestReply(QString a, QString p)
{
    writeFriendRequestHelper(a, p, Message::IntroductionB);
}

void ChatHouse::writeCreateChatRoom(QString chatName, const std::vector<QString>& friends)
{
    auto chatRoom = new ChatRoom(chatName);
    for(auto f : friends)
    {
        auto fi = std::find_if(Friends.begin(), Friends.end(), [f](Identity* r){ return r->Name == f; });
        if(*fi == nullptr)
            continue;
        chatRoom->Participants.push_back(*fi);
    }

    auto message = new Message();
    message->Sender = house->Uuid();
    message->Time = QDateTime::currentDateTime();
    message->Type = Message::CreateChatRoom;
    //09;NinLeters
    message->Content = chatName;
    for(auto i : chatRoom->Participants)
        message->Content += "," + i->Uuid();//addPrefixLength(i->Uuid());

    chatRoom->Messages.push_back(message);
    for(auto i : chatRoom->Participants)
        write(message, i);

    ChatRooms.push_back(chatRoom);
    emit NewChatRoomEvent();
}

void ChatHouse::writeMessageChatRoom(QString content, QString roomName)
{
    auto chatRoom = std::find_if(ChatRooms.begin(), ChatRooms.end(), [roomName](ChatRoom* r){ return r->ChatName == roomName; });
    if(*chatRoom == nullptr)
        return;

    auto message = new Message();
    message->Sender = house->Uuid();
    message->Time = QDateTime::currentDateTime();
    message->Type = Message::MessageChatRoom;
    message->Content = roomName + "::" + content;

    for(auto i : (*chatRoom)->Participants)
        write(message, i);
    (*chatRoom)->Messages.push_back(message);
    OnMessageEvent();
}

ChatHouse* ChatHouse::LoadChatHouse(QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QDataStream s(&file);
    int friendsSize, chatRoomsSize;
    QString houseName, housePort, houseUuid;
    Identity identity;
    ChatRoom chatRoom;
    auto friends = QVector<Identity*>();
    auto chatRooms = QVector<ChatRoom*>();

    s >> houseUuid;
    s >> houseName;
    s >> housePort;
    s >> friendsSize;
    for(auto i = 0; i < friendsSize; i++)
    {
        s >> identity;
        friends.push_back(new Identity(identity.Uuid(), identity.Name, identity.IpAddress, identity.Port));
    }
    s >> chatRoomsSize;
    for(auto i = 0; i < chatRoomsSize; i++)
    {
        s >> chatRoom;
        chatRooms.push_back(new ChatRoom(chatRoom.ChatName, chatRoom.Participants, chatRoom.Messages));
    }
    auto house = new ChatHouse(houseName, housePort);
    house->SetHouseUuid(houseUuid);
    house->SetIsLoadedFromSave();
    house->PendingFriends = friends;
    house->ChatRooms = chatRooms;
    return house;
}

ChatHouse* ChatHouse::LoadValidChatHouse(QString n, QString p)
{
    auto fileName = qApp->applicationDirPath() + "/" + n + ".ch";
    QFile file(fileName);
    if(file.exists())
        return LoadChatHouse(fileName);
    return new ChatHouse(n, p);
}

void ChatHouse::SaveChatHouse(QString fileName, ChatHouse* house)
{
    QFile file(fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QDataStream s(&file);
    s << house->GetHouseUuid();
    s << house->GetHouseName();
    s << house->GetHousePort();
    s << house->Friends.size();
    for(auto f : house->Friends)
        s << *f;
    s << house->ChatRooms.size();
    for(auto c : house->ChatRooms)
        s << *c;
}

QString ChatHouse::addPrefixLength(QString s)
{
    return (qMin(s.length(), 99) + ";" + s.mid(0, 99).trimmed());
}
