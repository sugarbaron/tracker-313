#include "sqlcorpuscle.h"

SqlCorpuscle::SqlCorpuscle(const QString& placeholder, const QVariant& valueToBind):
_placeholder(placeholder),
_valueToBind(valueToBind)
{}

SqlCorpuscle::~SqlCorpuscle(){}

QString SqlCorpuscle::getPlaceholder() const
{
  return _placeholder;
}

QVariant SqlCorpuscle::getValueToBind() const
{
  return _valueToBind;
}
