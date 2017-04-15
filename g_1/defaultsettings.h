#ifndef DEFAULTSETTINGS_H
#define DEFAULTSETTINGS_H

#include "singleton.h"

struct TextPadding {
    qreal top;
    qreal left;
    qreal right;
    qreal bottom;
};

class DefaultSettings
{
    friend class Singleton<DefaultSettings>;
public:
    
private:
    DefaultSettings() {}
};

class Settings
{
    friend class Singleton<Settings>;

    QSizeF m_gripSize;
    TextPadding m_textPadding;

public:
    const QSizeF &sizeOfGrip() const;
    const TextPadding &textPadding() const;

private:
    Settings();

};

#endif // DEFAULTSETTINGS_H
