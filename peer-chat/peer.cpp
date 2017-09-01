#include "peer.h"
#include "message.h"
#include "identity.h"

static inline qint32 ArrayToInt(QByteArray source);
static inline QByteArray IntToArray(qint32 source);

Peer::Peer(QObject *parent, Identity* i) : QObject(parent)
{
    server = new QTcpServer(this);
    me = i;
    connect(server, SIGNAL(newConnection()), SLOT(newConnection()));
    this->server->listen(QHostAddress::Any, i->Port.toInt());
}

void Peer::allocateSocket(QTcpSocket* socket)
{
    connect(socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));
    auto buffer = new QByteArray();
    auto s = new qint32(0);
    buffers.insert(socket, buffer);
    sizes.insert(socket, s);
}

bool Peer::connectTo(Identity* p)
{
    if (participants.contains(p->ToAddress()))
        return true;//participants[p->ToAddress()]->state() == QAbstractSocket::ConnectedState;

    auto socket = new QTcpSocket(this);
    socket->connectToHost(p->IpAddress, p->Port.toInt());
    auto connected = socket->waitForConnected();
    allocateSocket(socket);
    participants.insert(p->ToAddress(), socket);
    return connected;
}

void Peer::newConnection()
{
    while (server->hasPendingConnections())
    {
        auto socket = server->nextPendingConnection();
        allocateSocket(socket);
        auto i = Identity::ToAddress(socket->peerAddress().toString(), QString(socket->peerPort()));
        participants.insert(i, socket);
    }
}

void Peer::disconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    qint32 *s = sizes.value(socket);
    socket->deleteLater();
    buffers.remove(socket);
    sizes.remove(socket);
    delete buffer;
    delete s;
}

bool Peer::write(Message* m, Identity* i)
{
    if(m == NULL)
        return false;
    auto socketIter = participants.find(i->ToAddress());
    if(socketIter == participants.end())
        return false;
    auto socket = participants.value(i->ToAddress());
    if(socket == nullptr)
        return false;
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        m->Sender = this->me->Uuid();
        auto data = m->ToString().toUtf8();
        socket->write(IntToArray(data.size()));
        socket->write(data);
        qDebug() << QString(data);
        return socket->waitForBytesWritten();
    }
    return false;
}

void Peer::readyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    qint32 *s = sizes.value(socket);
    qint32 size = *s;
    while (socket->bytesAvailable() > 0)
    {
        buffer->append(socket->readAll());
        while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size))
        {            
            if (size == 0 && buffer->size() >= 4)
            {
                *s = size = ArrayToInt(buffer->mid(0, 4));
                buffer->remove(0, 4);
            }
            if (size > 0 && buffer->size() >= size)
            {
                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);                
                *s = size = 0;
                auto m = Message::FromString(QString(data));
                qDebug() << QString(data);
                emit read(m);
            }
        }
    }
}

qint32 ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

QByteArray IntToArray(qint32 source)
{
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
