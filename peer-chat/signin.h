#ifndef SIGNIN_H
#define SIGNIN_H

#include <QDialog>
#include "signindetails.h"

namespace Ui {
class SignIn;
}

class SignIn : public QDialog
{
    Q_OBJECT

public:
    explicit SignIn(QWidget *parent = 0);
    ~SignIn();
signals:
    void newSignIn(SignInDetails*);

private slots:
    void on_pushButton_clicked();

private:
    Ui::SignIn *ui;
    SignInDetails* nameToChange;
};

#endif // SIGNIN_H
