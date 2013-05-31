#include "animationviewmodel.h"
#include <QPixmap>
#include <QBuffer>
#include <QMovie>

AnimationViewModel::AnimationViewModel(QByteArray bytes, QUrl url, QObject *parent) : QThread(parent)
{
    buffer.open(QBuffer::ReadWrite);
    buffer.write(bytes);
    buffer.seek(0);
    movie = new QMovie(&buffer);
    movieUrl = url;
    connect(movie, SIGNAL(frameChanged(int)), this, SLOT(movieAnimated(int)));
    movie->start();
}

void AnimationViewModel::movieAnimated(int frame)
{
    QPixmap map = movie->currentPixmap();
    emit(movieAnimated(map, movieUrl));
}
