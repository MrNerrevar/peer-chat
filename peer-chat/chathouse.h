#ifndef CHATHOUSE_H
#define CHATHOUSE_H
#include <vector>
#include <QtCore>
#include <QtNetwork>
#include <QString>
#include <QTimer>

class Identity;
class ChatRoom;
class Message;
class Peer;
class ChatHouse : public QObject
{
    Q_OBJECT
public:
    explicit ChatHouse(QObject *p = 0);
    ChatHouse(QString, QString);
    ChatHouse();

    QVector<Identity*> Friends;
    QVector<ChatRoom*> ChatRooms;
    QVector<Message*> Requests;
    QVector<Identity*> PendingFriends;

    void writeMessageChatRoom(QString, QString);
    void writeFriendRequest(QString, QString);
    void writeReconnectRequest(Identity*, QString);
    void writeFriendRequestReply(QString, QString);
    void writeCreateChatRoom(QString, const std::vector<QString>&);

    QString GetName(const QString&);
    ChatRoom* GetChatRoom(const QString &);
    QString GetHouseName();
    QString GetHousePort();
    QString GetHouseUuid();
    void SetHouseUuid(QString);
    void SetIsLoadedFromSave();

    static ChatHouse* LoadChatHouse(QString);
    static ChatHouse* LoadValidChatHouse(QString, QString);
    static void SaveChatHouse(QString, ChatHouse*);

signals:
    void MessageEvent();
    void NewFriendEvent();
    void NewPendingFriendEvent();
    void NewChatRoomEvent();

private slots:
    void read(Message*);
    void write(Message*, Identity*);
    void OnMessageEvent();
    void OnTimeoutEvent();

private:
    Peer* communicator;
    Identity* house;
    QTimer* timer;
    bool isLoadedFromSave;

    Identity* writeFriendRequestHelper(QString, QString, QString);
    QString addPrefixLength(QString);

    void readFriendRequest(Message*);
    void readFriendRequestReply(Message*);
    void readReconnectRequest(Message*);
    Identity* readFriendRequestHelper(Message*);
    void readMessageChatRoom(Message*);
    void readCreateChatRoom(Message*);
};

#endif // CHATHOUSE_H
