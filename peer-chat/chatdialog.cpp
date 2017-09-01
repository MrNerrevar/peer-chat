#include <QtWidgets>

#include "chatdialog.h"
#include "message.h"
#include "signin.h"
#include "addfriend.h"
#include "identity.h"
#include "chathouse.h"
#include "chatroom.h"
#include "addchatroom.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    MessageLineEdit->setFocusPolicy(Qt::StrongFocus);
    ChatRoomTextEdit->setFocusPolicy(Qt::NoFocus);
    ChatRoomTextEdit->setReadOnly(true);
    FriendListWidget->setFocusPolicy(Qt::NoFocus);
    PendingFriendListWidget->setFocusPolicy(Qt::NoFocus);
    AddFriendButton->setFocusPolicy(Qt::NoFocus);
    AddChatRoomButton->setFocusPolicy(Qt::NoFocus);
    ChatRoomComboBox->setFocusPolicy(Qt::NoFocus);

    connect(MessageLineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    tableFormat.setBorder(0);

    signIn = new SignIn(this);
    connect(signIn, SIGNAL(newSignIn(SignInDetails*)), this, SLOT(OnNewSignIn(SignInDetails*)));

    addFriend = new AddFriend(this);
    connect(addFriend, SIGNAL(newFriend(AddFriendDetails*)), this, SLOT(OnNewFriend(AddFriendDetails*)));

    addChatRoom = new AddChatRoom(this);
    connect(addChatRoom, SIGNAL(newChatRoom(AddChatRoomDetails*)), this, SLOT(OnNewChatRoom(AddChatRoomDetails*)));
}

void ChatDialog::start()
{
    signIn->exec();
}

void ChatDialog::OnNewSignIn(SignInDetails* s)
{
    setWindowTitle("Chat: " + s->Name + ":" + s->Port);
    show();
    chatHouse = ChatHouse::LoadValidChatHouse(s->Name, s->Port);
    connect(chatHouse, SIGNAL(MessageEvent()), this, SLOT(OnMessage()));
    connect(chatHouse, SIGNAL(NewFriendEvent()), this, SLOT(OnNewFriend()));
    connect(chatHouse, SIGNAL(NewPendingFriendEvent()), this, SLOT(OnPendingFriend()));
    connect(chatHouse, SIGNAL(NewChatRoomEvent()), this, SLOT(OnNewChatRoom()));
    OnNewFriend();
    OnNewChatRoom();
    OnPendingFriend();
    OnMessage();
}

void ChatDialog::OnNewFriend(AddFriendDetails* f)
{
    chatHouse->writeFriendRequest(f->Address, f->Port);
}

void ChatDialog::OnNewChatRoom(AddChatRoomDetails * c)
{
    chatHouse->writeCreateChatRoom(c->ChatName, c->Friends);
}

void ChatDialog::OnMessage()
{
    on_ChatRoomComboBox_currentIndexChanged(ChatRoomComboBox->currentText());
}

void ChatDialog::returnPressed()
{
    QString text = MessageLineEdit->text();
    if (text.isEmpty())
        return;

    auto currentChat = ChatRoomComboBox->currentText();
    if(currentChat.isEmpty())
        return;

    MessageLineEdit->clear();
    chatHouse->writeMessageChatRoom(text, currentChat);
    this->on_ChatRoomComboBox_currentIndexChanged(currentChat);
}

void ChatDialog::newParticipant(const QString &nick)
{
    if (nick.isEmpty())
        return;

    QColor color = ChatRoomTextEdit->textColor();
    ChatRoomTextEdit->setTextColor(Qt::gray);
    ChatRoomTextEdit->append(tr("* %1 has joined").arg(nick));
    ChatRoomTextEdit->setTextColor(color);
    FriendListWidget->addItem(nick);
}

void ChatDialog::OnNewFriend()
{
    FriendListWidget->clear();
    for(auto i : chatHouse->Friends)
        if(!i->Name.isEmpty())
            FriendListWidget->addItem(i->Name);
}

void ChatDialog::OnPendingFriend()
{
    PendingFriendListWidget->clear();
    for(auto i : chatHouse->PendingFriends)        
        PendingFriendListWidget->addItem(i->IpAddress + ":" + i->Port);
}

void ChatDialog::OnNewChatRoom()
{
    ChatRoomComboBox->clear();
    for(auto i : chatHouse->ChatRooms)
        ChatRoomComboBox->addItem(i->ChatName);
}

void ChatDialog::participantLeft(const QString &nick)
{
    if (nick.isEmpty())
        return;

    QList<QListWidgetItem *> items = FriendListWidget->findItems(nick, Qt::MatchExactly);
    if (items.isEmpty())
        return;

    delete items.at(0);
    QColor color = ChatRoomTextEdit->textColor();
    ChatRoomTextEdit->setTextColor(Qt::gray);
    ChatRoomTextEdit->append(tr("* %1 has left").arg(nick));
    ChatRoomTextEdit->setTextColor(color);
}

void ChatDialog::on_AddFriendButton_clicked()
{
    addFriend->exec();
}

void ChatDialog::on_AddChatRoomButton_clicked()
{
    auto friendNames = std::vector<QString>();
    for(auto n : chatHouse->Friends)
        friendNames.push_back(n->Name);
    addChatRoom->updateFriendList(friendNames);
    addChatRoom->exec();
}

void ChatDialog::on_ChatRoomComboBox_currentIndexChanged(const QString &chatName)
{
    if(chatName.isNull() || chatName.isEmpty())
        return;

    auto chatRoom = chatHouse->GetChatRoom(chatName);
    if(chatRoom == nullptr)
        return;

    ChatRoomTextEdit->clear();
    QTextCursor cursor(ChatRoomTextEdit->textCursor());
    cursor.movePosition(QTextCursor::End);
    QTextTable *table = cursor.insertTable(chatRoom->Messages.size(), 2, tableFormat);
    auto i = 0;
    for(auto mc : chatRoom->Messages)
    {
        table->cellAt(i, 0).firstCursorPosition().insertText("<" + chatHouse->GetName(mc->Sender.toString()) + ">");
        table->cellAt(i, 1).firstCursorPosition().insertText(mc->Content);
        i++;
    }
    QScrollBar *bar = ChatRoomTextEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}
