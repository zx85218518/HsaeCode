#ifndef QTSTL_GLOBAL_H
#define QTSTL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTSTL_LIBRARY)
#  define QTSTLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTSTLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QTSTL_GLOBAL_H
