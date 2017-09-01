#ifndef ADDCHATROOM_H
#define ADDCHATROOM_H

#include <QDialog>
#include <vector>
#include "addchatroomdetails.h"

namespace Ui {
class AddChatRoom;
}

class AddChatRoom : public QDialog
{
    Q_OBJECT

public:
    explicit AddChatRoom(QWidget *parent = 0);
    ~AddChatRoom();

    void updateFriendList(const std::vector<QString>& fl);
signals:
    void newChatRoom(AddChatRoomDetails*);

private slots:
    void on_AddButton_clicked();
    void on_RemoveButton_clicked();
    void on_OkButton_clicked();
    void on_CancelButton_clicked();

private:
    Ui::AddChatRoom *ui;
    AddChatRoomDetails* chatRoomToAdd;
};

#endif // ADDCHATROOM_H
