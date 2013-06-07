#include "textbox.h"
#include <QKeyEvent>
#include <QDebug>
#include <QLineEdit>
#include "messagehistory.h"




TextBox::TextBox(QWidget *parent) : QLineEdit(parent)
{
    messageHistory = new MessageHistory(this);
    searchingUsernames = QStringList();
    userSearchIndex = 0;
}

TextBox::~TextBox()
{

}

void TextBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
    {
        bool willCycleUp = event->key() == Qt::Key_Up;

        QString lastSent = messageHistory->getLastSentMessage(willCycleUp);
        this->setText(lastSent);
    }
    else {
        if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        {
            messageHistory->insertNewMessage(this->text());
        }

        QLineEdit::keyPressEvent(event);
    }
}

bool TextBox::event(QEvent *e)
{
    if(e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        if(ke->key() == Qt::Key_Tab) {
            getLastArgument();
            return true;
        }

    }

    return QLineEdit::event(e);
}

void TextBox::setUsernames(QStringList users)
{
    usernames = users;
}

QString TextBox::getLastArgument()
{
    QStringList messageList = this->text().split(QRegExp("\\s+"));

    QString lastWord = NULL;
    if(!messageList.isEmpty()) {
        lastWord = messageList.last();
        qDebug() << lastWord;
    }

    if(lastWord == NULL || lastWord.isEmpty()) {
        return this->text();
    }

    if(searchingUsernames.isEmpty()) {
        userSearchIndex = 0;
        for(QStringList::Iterator iter = usernames.begin(); iter != usernames.end(); iter++) {
            QString wordToCheck = *iter;
            if(wordToCheck.startsWith(lastWord, Qt::CaseInsensitive)) {
                searchingUsernames.append(wordToCheck);
            }
        }

    } else {
        userSearchIndex++;
    }
    if(searchingUsernames.isEmpty()) {
        QString foundName = searchingUsernames.at(userSearchIndex % searchingUsernames.length());
    }


    qDebug() << "Found name: " << foundName;


    return lastWord;

}
