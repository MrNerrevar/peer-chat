#ifndef ADDCHATROOMDETAILS_H
#define ADDCHATROOMDETAILS_H
#include <vector>
#include <QString>

class AddChatRoomDetails
{
public:
    AddChatRoomDetails();

    std::vector<QString> Friends;
    QString ChatName;
};

#endif // ADDCHATROOMDETAILS_H
