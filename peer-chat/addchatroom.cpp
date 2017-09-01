#include "addchatroom.h"
#include "ui_addchatroom.h"
#include <QDebug>

AddChatRoom::AddChatRoom(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddChatRoom)
{
    ui->setupUi(this);
    chatRoomToAdd = new AddChatRoomDetails();
    setWindowTitle("Add Chat Room");

}

void AddChatRoom::updateFriendList(const std::vector<QString>& fl)
{
    ui->FriendListComboBox->clear();
    for(auto& f : fl)
        ui->FriendListComboBox->addItem(f);
}

AddChatRoom::~AddChatRoom()
{
    delete ui;
    delete chatRoomToAdd;
}

void AddChatRoom::on_AddButton_clicked()
{
    QString text = ui->FriendListComboBox->currentText();
    if (text.isEmpty())
        return;

    auto contains = false;
    for(int i = 0; i < ui->FriendListWidget->count(); i++)
    {
        if(ui->FriendListWidget->item(i)->text().compare(text) == 0)
        {
            contains = true;
            break;
        }
    }

    if(!contains)
        ui->FriendListWidget->addItem(text);
}

void AddChatRoom::on_RemoveButton_clicked()
{
    auto item = ui->FriendListWidget->currentItem();
    if(item != nullptr)
        ui->FriendListWidget->removeItemWidget(item);
}

void AddChatRoom::on_OkButton_clicked()
{
    auto text = ui->ChatNameLineEdit->text();
    if(text.isEmpty())
        return;

    chatRoomToAdd->ChatName = text;

    if(ui->FriendListWidget->count() == 0)
        return;

    chatRoomToAdd->Friends.clear();
    for(int i = 0; i < ui->FriendListWidget->count(); i++)
        chatRoomToAdd->Friends.push_back(ui->FriendListWidget->item(i)->text());

    hide();
    emit newChatRoom(chatRoomToAdd);
}

void AddChatRoom::on_CancelButton_clicked()
{
    hide();
}
