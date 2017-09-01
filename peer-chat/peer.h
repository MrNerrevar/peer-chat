#ifndef PEER_H
#define PEER_H

#include <QtCore>
#include <QtNetwork>
#include <QString>

class Message;
class Identity;
class Peer : public QObject
{
    Q_OBJECT
public:
    explicit Peer(QObject *parent = 0, Identity* i = 0);

signals:
    void read(Message*);
public slots:
    bool write(Message*, Identity*);
    bool connectTo(Identity*);
private slots:
    void newConnection();
    void disconnected();
    void readyRead();

private:
    void allocateSocket(QTcpSocket*);    

    Identity* me;
    QTcpServer* server;
    QHash<QTcpSocket*, QByteArray*> buffers;
    QHash<QTcpSocket*, qint32*> sizes;
    QHash<QString, QTcpSocket*> participants;
};

#endif // PEER_H
