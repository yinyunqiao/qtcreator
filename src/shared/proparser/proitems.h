/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#ifndef PROITEMS_H
#define PROITEMS_H

#include <QtCore/QString>
#include <QtCore/QVector>

QT_BEGIN_NAMESPACE

#ifdef PROPARSER_THREAD_SAFE
typedef QAtomicInt ProItemRefCount;
#else
class ProItemRefCount {
public:
    ProItemRefCount(int cnt = 0) : m_cnt(cnt) {}
    bool ref() { return ++m_cnt != 0; }
    bool deref() { return --m_cnt != 0; }
    ProItemRefCount &operator=(int value) { m_cnt = value; return *this; }
private:
    int m_cnt;
};
#endif

namespace ProStringConstants {
enum OmitPreHashing { NoHash };
}

class ProStringList;

class ProString {
public:
    ProString();
    ProString(const ProString &other);
    ProString(const ProString &other, ProStringConstants::OmitPreHashing);
    explicit ProString(const QString &str);
    ProString(const QString &str, ProStringConstants::OmitPreHashing);
    explicit ProString(const char *str);
    ProString(const char *str, ProStringConstants::OmitPreHashing);
    ProString(const QString &str, int offset, int length);
    ProString(const QString &str, int offset, int length, uint hash);
    ProString(const QString &str, int offset, int length, ProStringConstants::OmitPreHashing);
    QString toQString() const;
    QString &toQString(QString &tmp) const;
    ProString &operator+=(const ProString &other);
    ProString &append(const ProString &other, bool *pending = 0);
    ProString &append(const ProStringList &other, bool *pending = 0, bool skipEmpty1st = false);
    bool operator==(const ProString &other) const;
    bool operator==(const QString &other) const;
    bool operator==(const QLatin1String &other) const;
    bool operator!=(const ProString &other) const { return !(*this == other); }
    bool operator!=(const QString &other) const { return !(*this == other); }
    bool operator!=(const QLatin1String &other) const { return !(*this == other); }
    bool isEmpty() const { return !m_length; }
    int size() const { return m_length; }
    const QChar *constData() const { return m_string.constData() + m_offset; }
    ProString mid(int off, int len = -1) const;
    ProString left(int len) const { return mid(0, len); }
    ProString right(int len) const { return mid(qMax(0, size() - len)); }
    ProString trimmed() const;
    void clear() { m_string.clear(); m_length = 0; }

    static uint hash(const QChar *p, int n);

private:
    QString m_string;
    int m_offset, m_length;
    mutable uint m_hash;
    QChar *prepareAppend(int extraLen);
    uint updatedHash() const;
    friend uint qHash(const ProString &str);
    friend QString operator+(const ProString &one, const ProString &two);
};
Q_DECLARE_TYPEINFO(ProString, Q_MOVABLE_TYPE);

uint qHash(const ProString &str);
QString operator+(const ProString &one, const ProString &two);
inline QString operator+(const ProString &one, const QString &two)
    { return one + ProString(two, ProStringConstants::NoHash); }
inline QString operator+(const QString &one, const ProString &two)
    { return ProString(one, ProStringConstants::NoHash) + two; }

class ProStringList : public QVector<ProString> {
public:
    ProStringList() {}
    ProStringList(const ProString &str) { *this << str; }
    QString join(const QString &sep) const;
    void removeDuplicates();
};

// These token definitions affect both ProFileEvaluator and ProWriter
enum ProToken {
    TokTerminator = 0,  // end of stream (possibly not included in length; must be zero)
    TokLine,            // line marker:
                        // - line (1)
    TokAssign,          // variable =
    TokAppend,          // variable +=
    TokAppendUnique,    // variable *=
    TokRemove,          // variable -=
    TokReplace,         // variable ~=
                        // previous literal/expansion is a variable manipulation
                        // - value expression + TokValueTerminator
    TokValueTerminator, // assignment value terminator
    TokLiteral,         // literal string (fully dequoted)
                        // - length (1)
                        // - string data (length; unterminated)
    TokHashLiteral,     // literal string with hash (fully dequoted)
                        // - hash (2)
                        // - length (1)
                        // - string data (length; unterminated)
    TokVariable,        // qmake variable expansion
                        // - hash (2)
                        // - name length (1)
                        // - name (name length; unterminated)
    TokProperty,        // qmake property expansion
                        // - name length (1)
                        // - name (name length; unterminated)
    TokEnvVar,          // environment variable expansion
                        // - name length (1)
                        // - name (name length; unterminated)
    TokFuncName,        // replace function expansion
                        // - hash (2)
                        // - name length (1)
                        // - name (name length; unterminated)
                        // - ((nested expansion + TokArgSeparator)* + nested expansion)?
                        // - TokFuncTerminator
    TokArgSeparator,    // function argument separator
    TokFuncTerminator,  // function argument list terminator
    TokCondition,       // previous literal/expansion is a conditional
    TokTestCall,        // previous literal/expansion is a test function call
                        // - ((nested expansion + TokArgSeparator)* + nested expansion)?
                        // - TokFuncTerminator
    TokNot,             // '!' operator
    TokAnd,             // ':' operator
    TokOr,              // '|' operator
    TokBranch,          // branch point:
                        // - then block length (2)
                        // - then block + TokTerminator (then block length)
                        // - else block length (2)
                        // - else block + TokTerminator (else block length)
    TokForLoop,         // for loop:
                        // - variable name: hash (2), length (1), chars (length)
                        // - expression: length (2), bytes + TokValueTerminator (length)
                        // - body length (2)
                        // - body + TokTerminator (body length)
    TokTestDef,         // test function definition:
    TokReplaceDef,      // replace function definition:
                        // - function name: hash (2), length (1), chars (length)
                        // - body length (2)
                        // - body + TokTerminator (body length)
    TokMask = 0xff,
    TokQuoted = 0x100,  // The expression is quoted => join expanded stringlist
    TokNewStr = 0x200   // Next stringlist element
};

class ProFile
{
public:
    explicit ProFile(const QString &fileName);
    ~ProFile();

    QString fileName() const { return m_fileName; }
    QString directoryName() const { return m_directoryName; }
    const QString &items() const { return m_proitems; }
    QString *itemsRef() { return &m_proitems; }
    const ushort *tokPtr() const { return (const ushort *)m_proitems.constData(); }

    void ref() { m_refCount.ref(); }
    void deref() { if (!m_refCount.deref()) delete this; }

    bool isOk() const { return m_ok; }
    void setOk(bool ok) { m_ok = ok; }

private:
    ProItemRefCount m_refCount;
    QString m_proitems;
    QString m_fileName;
    QString m_directoryName;
    bool m_ok;
};

QT_END_NAMESPACE

#endif // PROITEMS_H
