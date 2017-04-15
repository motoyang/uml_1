#include <QtWidgets>
#include "defaultsettings.h"


const QSizeF &Settings::sizeOfGrip() const
{
    return m_gripSize;
}

const TextPadding &Settings::textPadding() const
{
    return m_textPadding;
}

Settings::Settings()
    : m_gripSize(QSizeF(6, 6)),
      m_textPadding {5, 5, 5, 5}
{

}
