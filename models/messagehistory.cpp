#include "messagehistory.h"

MessageHistory::MessageHistory(QObject *parent) : QStandardItemModel(parent)
{
    messageIndex = 0;
    sentMessages = QStringList();
}

MessageHistory::~MessageHistory()
{

}

void MessageHistory::insertNewMessage(QString text)
{
    // We'll insert at the front of the list so the last message sent will be the most recent
    sentMessages.insert(0, text);
    this->resetMessagePosition();
}

QString MessageHistory::getLastSentMessage(bool cycleUp)
{
    if (cycleUp)
    {
        messageIndex++;
    }
    else if (!cycleUp)
    {
        messageIndex--;
    }

    if (sentMessages.length() == 0)
    {
        return "";
    }

    // We'll restart at the beginning of the list if we get to the end
    if (messageIndex >= sentMessages.length())
    {
        messageIndex = 0;
    }
    // Rolling backwards
    if (messageIndex < 0)
    {
        messageIndex = sentMessages.length() - 1;
    }

    QString lastMessage = sentMessages[messageIndex];
    return lastMessage;
}

void MessageHistory::resetMessagePosition()
{
    messageIndex = 0;
}
