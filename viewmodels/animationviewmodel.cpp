#include "animationviewmodel.h"
#include <QPixmap>
#include <QBuffer>
#include <QMovie>
#include <QTextDocument>
#include <QDebug>

AnimationViewModel::AnimationViewModel(QByteArray bytes, QUrl url, QTextDocument *document, QObject *parent) : QThread(parent)
{
    movieUrl = url;
    textDocument = document;
    movieBits = bytes;
}

void AnimationViewModel::run()
{
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    buffer.write(movieBits);
    buffer.seek(0);

    movie = new QMovie(&buffer, QByteArray());
    QPixmap map;
    map.loadFromData(movieBits);
    if(!map.isNull()) {
        QSize size = map.size();
        movie->setScaledSize(QSize((150 * size.width()) / size.height(), 150));
    }
    connect(movie, SIGNAL(frameChanged(int)), this, SLOT(movieAnimated(int)), Qt::DirectConnection);
    movie->start();
    exec();
    movie->stop();
    movie->deleteLater();
}

void AnimationViewModel::movieAnimated(int frame)
{
    QPixmap map = movie->currentPixmap();
    textDocument->addResource(QTextDocument::ImageResource, movieUrl, map);
}
