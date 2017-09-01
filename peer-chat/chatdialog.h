#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include "ui_chatdialog.h"

class Identity;
class Message;
class SignIn;
class SignInDetails;
class AddFriend;
class AddFriendDetails;
class AddChatRoom;
class AddChatRoomDetails;
class ChatHouse;
class ChatRoom;
class ChatDialog : public QDialog, private Ui::ChatDialog
{
    Q_OBJECT

public:
    ChatDialog(QWidget *parent = 0);

    void start();
public slots:
    void OnMessage();

private slots:
    void returnPressed();
    void participantLeft(const QString &nick);
    void newParticipant(const QString &nick);

    void OnPendingFriend();
    void OnNewFriend();
    void OnNewFriend(AddFriendDetails*);
    void OnNewSignIn(SignInDetails*);
    void OnNewChatRoom();
    void OnNewChatRoom(AddChatRoomDetails*);

    void on_AddFriendButton_clicked();

    void on_AddChatRoomButton_clicked();

    void on_ChatRoomComboBox_currentIndexChanged(const QString &arg1);

private:
    ChatHouse* chatHouse;
    QTextTableFormat tableFormat;
    Identity* user;
    SignIn* signIn;
    AddFriend* addFriend;
    AddChatRoom* addChatRoom;
};

#endif
