#include "signin.h"
#include "ui_signin.h"
#include <QDebug>

SignIn::SignIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignIn)
{
    ui->setupUi(this);
    nameToChange = new SignInDetails;
    setWindowTitle("Sign In");
}

SignIn::~SignIn()
{
    delete ui;
}


void SignIn::on_pushButton_clicked()
{
    auto text = ui->lineEdit->text();
    if(text.isEmpty())
        return;
    nameToChange->Name = text;
    nameToChange->Port = ui->lineEdit_2->text();
    hide();
    emit newSignIn(nameToChange);
}
