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

const QSizeF &Settings::sizeOfDropped() const
{
    return m_droppedSize;
}

const QSizeF &Settings::sizeOfExtended() const
{
    return m_extendedSize;
}

qreal Settings::widthOfShape() const
{
    return m_shapeWidth;
}

Settings::Settings()
    : m_gripSize(6, 6),
      m_droppedSize(6, 6),
      m_extendedSize(6, 6),
      m_shapeWidth(6),
      m_textPadding {5, 5, 5, 5}
{

}
