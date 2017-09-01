#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QDialog>
#include "addfrienddetails.h"

namespace Ui {
class AddFriend;
}

class AddFriend : public QDialog
{
    Q_OBJECT

public:
    explicit AddFriend(QWidget *parent = 0);
    ~AddFriend();

signals:
    void newFriend(AddFriendDetails*);

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::AddFriend *ui;
    AddFriendDetails* friendToAdd;
};

#endif // ADDFRIEND_H
