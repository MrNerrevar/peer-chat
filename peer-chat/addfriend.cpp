#include "addfriend.h"
#include "ui_addfriend.h"
#include <QDebug>

AddFriend::AddFriend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFriend)
{
    ui->setupUi(this);
    friendToAdd = new AddFriendDetails;
    setWindowTitle("Add Friend");
}

AddFriend::~AddFriend()
{
    delete ui;
}

void AddFriend::on_pushButton_2_clicked()
{
    hide();
}

void AddFriend::on_pushButton_clicked()
{
    friendToAdd->Address = ui->lineEdit->text();
    friendToAdd->Port = ui->lineEdit_2->text();
    hide();
    emit newFriend(friendToAdd);
}
