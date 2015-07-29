#include "DlgValidatorDateTime.h"
#include "FormatDateTime.h"
#include "Logger.h"

DlgValidatorDateTime::DlgValidatorDateTime(CoordScale coordScale,
                                           CoordUnitsDate coordUnitsDate,
                                           CoordUnitsTime coordUnitsTime,
                                           QObject *parent) :
  DlgValidatorAbstract(parent),
  m_coordScale (coordScale),
  m_coordUnitsDate (coordUnitsDate),
  m_coordUnitsTime (coordUnitsTime)
{
  LOG4CPP_INFO_S ((*mainCat)) << "DlgValidatorDateTime::DlgValidatorDateTime";
}

QValidator::State DlgValidatorDateTime::validate (QString &input,
                                                  int &pos) const
{
  FormatDateTime formatDateTime;
  QDateTime parsedValue = formatDateTime.parse (m_coordUnitsDate,
                                                m_coordUnitsTime,
                                                input);

  QValidator::State state = (parsedValue.isValid() ?
                               QValidator::Acceptable :
                               QValidator::Invalid);

  if (state != QValidator::Acceptable) {

    pos = 0; // Would be nice to set to a value that meant something

  }

  return state;
}