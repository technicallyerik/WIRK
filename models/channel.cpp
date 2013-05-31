#include "channel.h"
#include "server.h"
#include "user.h"
#include "session.h"

Channel::Channel(QString inName, QStandardItem *inMenuItem, Server *parent) : QObject(parent)
{
    text = "<body>";
    users = new QStandardItemModel(this);
    menuItem = inMenuItem;
    this->setName(inName);
}

QString Channel::getName() {
    return name;
}

void Channel::setName(QString inName) {
    name = inName;
    menuItem->setText(inName);
}

QString Channel::getText() {
    return text;
}

void Channel::appendText(QString inText) {
    this->appendText("", inText, Channel::Info);
}


void Channel::appendText(QString sender, QString inText, MessageType type) {
    QString currentUser = this->getServer()->getNickname();
    bool textContainsUser = inText.contains(currentUser, Qt::CaseInsensitive);
    QDateTime currentTime = QDateTime::currentDateTime();
    QString currentTimeStr = currentTime.toString("h:mmap");

    QString tableRow = "";
    if (type == Channel::Emote)
    {
        tableRow = "<table class=\"msg-emote\" width=\"100%\"><tr>";
    }
    else if (type == Channel::Topic)
    {
        tableRow = "<table class=\"msg-topic\" width=\"100%\"><tr>";
    }
    else if (type == Channel::Info)
    {
        tableRow = "<table class=\"msg-info\" width=\"100%\"><tr>";
    }
    else if (textContainsUser)
    {
        tableRow = "<table class=\"msg-mentioned\" width=\"100%\"><tr>";
    }
    else
    {
        tableRow = "<table width=\"100%\"><tr>";
    }
        tableRow += "<th class=\"col-name\" width=\"140\" align=\"right\"><span class=\"user\">" + sender + "</span></th>";
        tableRow += "<td class=\"col-message\"><p class=\"message\">" + inText + "</p></td>";
        tableRow += "<td class=\"col-meta\" width=\"50\"><h6 class=\"metainfo\">" + currentTimeStr +"</h6></td>";
        tableRow += "</tr></table>";
    text += tableRow;
    Server *server = this->getServer();
    Session *session = server->getSession();
    session->emitMessageReceived(this->getServer(), this, tableRow);
}

QStandardItemModel* Channel::getUsers() {
    return users;
}

void Channel::addUsers(QStringList inUsers) {
    for (int i = 0; i < inUsers.count(); i++) {
        QString newUserName = inUsers[i];
        this->addUser(newUserName);
    }
}

void Channel::addUser(QString inUser) {
    QStandardItem *newMenuItem = new QStandardItem();
    User *newUser = new User(inUser.toLower(), newMenuItem, this);
    newMenuItem->setData(QVariant::fromValue<User*>(newUser), Qt::UserRole);
    users->appendRow(newMenuItem);
    users->sort(0);
}

void Channel::removeUser(QString inUser) {
    QStandardItem *menuItem = getUserMenuItem(inUser);
    if(menuItem != NULL) {
        int row = menuItem->row();
        users->removeRow(row);
    }
}

QStandardItem* Channel::getUserMenuItem(QString inUser)
{
    QList<QStandardItem*> foundUsers = users->findItems(inUser.toLower());
    if(foundUsers.count() == 1) {
        QStandardItem *user = foundUsers[0];
        return user;
    }
    return NULL;
}

User* Channel::getUser(QString inUser)
{
    QStandardItem *user = getUserMenuItem(inUser);
    if(user != NULL) {
        QVariant data = user->data(Qt::UserRole);
        return data.value<User*>();
    }
    return NULL;
}

Server* Channel::getServer() {
    return qobject_cast<Server *>(this->parent());
}

QStandardItem* Channel::getMenuItem()
{
    return menuItem;
}
