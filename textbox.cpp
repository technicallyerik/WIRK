#include "textbox.h"
#include <QKeyEvent>
#include <QDebug>
#include <QLineEdit>
#include "messagehistory.h"

TextBox::TextBox(QWidget *parent) : QLineEdit(parent)
{
    messageHistory = new MessageHistory(this);
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
