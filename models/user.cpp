#include "user.h"
#include "channel.h"
#include <stdlib.h>
#include <QSet>
#include <QBrush>
#include <QDebug>

User::User(QString inName, QChar inMode, QStandardItem *inMenuItem, Channel *parent) : QObject(parent)
{
    menuItem = inMenuItem;
    modes = QSet<QChar>();

    //we've got 16 colors to choose from
    int randomColorNumber = (rand() % 16) + 1;
    this->classColor = QString::number(randomColorNumber);

    this->setName(inName);
    this->addMode(inMode);
}

User::~User()
{

}

QString User::getName() {
    return name;
}

void User::setName(QString inName) {
    if(name != NULL && inName.compare(name, Qt::CaseInsensitive) != 0) {
        QString renameMessage = QString("%1 is now known as %2").arg(name, inName);
        Channel *channel = getChannel();
        channel->appendText(renameMessage);
    }
    name = inName;

    menuItem->setData(name, UserDataName);
    this->refreshUserDisplay();
}

void User::setColor(QString classColor) {
    this->classColor = classColor;
}

QSet<QChar> User::getModes()
{
    return modes;
}

void User::refreshUserDisplay()
{
    QString mode = this->getModeDisplayString();
    menuItem->setText(mode + name);

    QBrush brush = getUserColor();
    menuItem->setForeground(brush);

    QString sortString = getSortString();
    menuItem->setData(sortString, UserDataSort);
    Channel *channel = this->getChannel();
    channel->sortUsers();
}

QChar User::getModeDisplayString()
{
    QChar mode = char();
    if (modes.contains('q'))
        mode = USER_MODE_OWNER;
    else if (modes.contains('a'))
        mode = USER_MODE_ADMIN;
    else if (modes.contains('o'))
        mode = USER_MODE_OPERATOR;
    else if (modes.contains('h'))
        mode = USER_MODE_HALF_OP;
    else if (modes.contains('v'))
        mode = USER_MODE_VOICED;

    return mode;
}

void User::addMode(QChar mode)
{
    if (mode == USER_MODE_OWNER)
        mode = 'q';
    else if (mode == USER_MODE_ADMIN)
        mode = 'a';
    else if (mode == USER_MODE_OPERATOR)
        mode = 'o';
    else if (mode == USER_MODE_HALF_OP)
        mode = 'h';
    else if (mode == USER_MODE_VOICED)
        mode = 'v';

    modes.insert(mode);
    this->refreshUserDisplay();
}

void User::removeMode(QChar mode)
{
    modes.remove(mode);
    this->refreshUserDisplay();
}

QString User::getSortString()
{
    int sortNumberPrefix;
    QChar mode = this->getModeDisplayString();

    if (mode == USER_MODE_OWNER)
        sortNumberPrefix = 1;
    else if (mode == USER_MODE_ADMIN)
        sortNumberPrefix = 2;
    else if (mode == USER_MODE_OPERATOR)
        sortNumberPrefix = 3;
    else if (mode == USER_MODE_HALF_OP)
        sortNumberPrefix = 4;
    else if (mode == USER_MODE_VOICED)
        sortNumberPrefix = 5;
    else
        sortNumberPrefix = 6;

    return sortNumberPrefix + name.toLower();
}

Channel* User::getChannel() {
    return qobject_cast<Channel*>(this->parent());
}

QStandardItem* User::getMenuItem()
{
    return menuItem;
}

QBrush User::getUserColor()
{
    QColor colorChoice;

    if (this->classColor == "1") {
            colorChoice = QColor("#F7977A");
    }
    else if (this->classColor == "2") {
        colorChoice = QColor("#F9AD81");
    }
    else if (this->classColor == "3") {
        colorChoice = QColor("#FDC68A");
    }
    else if (this->classColor == "4") {
        colorChoice = QColor("#FFF79A");
    }
    else if (this->classColor == "5") {
        colorChoice = QColor("#C4DF9B");
    }
    else if (this->classColor == "6") {
        colorChoice = QColor("#A2D39C");
    }
    else if (this->classColor == "7") {
        colorChoice = QColor("#82CA9D");
    }
    else if (this->classColor == "8") {
        colorChoice = QColor("#7BCDC8");
    }
    else if (this->classColor == "9") {
        colorChoice = QColor("#6ECFF6");
    }
    else if (this->classColor == "10") {
        colorChoice = QColor("#7EA7D8");
    }
    else if (this->classColor == "11") {
        colorChoice = QColor("#8493CA");
    }
    else if (this->classColor == "12") {
        colorChoice = QColor("#8882BE");
    }
    else if (this->classColor == "13") {
        colorChoice = QColor("#A187BE");
    }
    else if (this->classColor == "14") {
        colorChoice = QColor("#BC8DBF");
    }
    else if (this->classColor == "15") {
        colorChoice = QColor("#F49AC2");
    }
    else if (this->classColor == "16") {
        colorChoice = QColor("#F6989D");
    }
    else {
        colorChoice = QColor("#FFFFFF");
    }

    QBrush brush = QBrush(colorChoice);
    return brush;
}
