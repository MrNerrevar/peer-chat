#ifndef SIGNINDETAILS_H
#define SIGNINDETAILS_H
#include <QString>

class SignInDetails
{
public:
    SignInDetails();
    SignInDetails(QString);

    QString Name;
    QString Port;
};

#endif // SIGNINDETAILS_H
