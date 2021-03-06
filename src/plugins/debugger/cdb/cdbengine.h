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

#ifndef DEBUGGER_CDBENGINE_H
#define DEBUGGER_CDBENGINE_H

#include "debuggerengine.h"
#include "breakpoint.h"

#include <QtCore/QSharedPointer>
#include <QtCore/QProcess>
#include <QtCore/QVariant>
#include <QtCore/QMap>
#include <QtCore/QTime>

namespace Utils {
class ConsoleProcess;
}
namespace Debugger {
namespace Internal {

class DisassemblerAgent;
struct CdbBuiltinCommand;
struct CdbExtensionCommand;
struct CdbOptions;
class ByteArrayInputStream;
class GdbMi;

class CdbEngine : public Debugger::DebuggerEngine
{
    Q_OBJECT

public:
    typedef QSharedPointer<CdbOptions> OptionsPtr;

    enum CommandFlags { QuietCommand = 0x1 };
    // Flag bits for a sequence of commands
    enum CommandSequenceFlags {
        CommandListStack = 0x1,
        CommandListThreads = 0x2,
        CommandListRegisters = 0x4,
        CommandListModules = 0x8,
        CommandListBreakPoints = 0x10
    };

    typedef QSharedPointer<CdbBuiltinCommand> CdbBuiltinCommandPtr;
    typedef QSharedPointer<CdbExtensionCommand> CdbExtensionCommandPtr;
    typedef void (CdbEngine::*BuiltinCommandHandler)(const CdbBuiltinCommandPtr &);
    typedef void (CdbEngine::*ExtensionCommandHandler)(const CdbExtensionCommandPtr &);

    explicit CdbEngine(const DebuggerStartParameters &sp,
        DebuggerEngine *masterEngine,
        const OptionsPtr &options);

    virtual ~CdbEngine();
    // Factory function that returns 0 if the debug engine library cannot be found.

    virtual bool setToolTipExpression(const QPoint &mousePos, TextEditor::ITextEditor *editor,
                                      const DebuggerToolTipContext &ctx);
    virtual void setupEngine();
    virtual void setupInferior();
    virtual void runEngine();
    virtual void shutdownInferior();
    virtual void shutdownEngine();
    virtual void detachDebugger();
    virtual void updateWatchData(const WatchData &data,
                                 const WatchUpdateFlags & flags = WatchUpdateFlags());
    virtual unsigned debuggerCapabilities() const;
    virtual void watchPoint(const QPoint &);
    virtual void setRegisterValue(int regnr, const QString &value);

    virtual void executeStep();
    virtual void executeStepOut();
    virtual void executeNext();
    virtual void executeStepI();
    virtual void executeNextI();

    virtual void continueInferior();
    virtual void interruptInferior();

    virtual void executeRunToLine(const ContextData &data);
    virtual void executeRunToFunction(const QString &functionName);
    virtual void executeJumpToLine(const ContextData &data);
    virtual void assignValueInDebugger(const WatchData *w, const QString &expr, const QVariant &value);
    virtual void executeDebuggerCommand(const QString &command);

    virtual void activateFrame(int index);
    virtual void selectThread(int index);

    virtual bool stateAcceptsBreakpointChanges() const;
    virtual bool acceptsBreakpoint(BreakpointId id) const;
    virtual void attemptBreakpointSynchronization();

    virtual void fetchDisassembler(DisassemblerAgent *agent);
    virtual void fetchMemory(MemoryAgent *, QObject *, quint64 addr, quint64 length);

    virtual void reloadModules();
    virtual void loadSymbols(const QString &moduleName);
    virtual void loadAllSymbols();
    virtual void requestModuleSymbols(const QString &moduleName);

    virtual void reloadRegisters();
    virtual void reloadSourceFiles();
    virtual void reloadFullStack();

    static QString extensionLibraryName(bool is64Bit);

private slots:
    void readyReadStandardOut();
    void readyReadStandardError();
    void processError();
    void processFinished();
    void postCommand(const QByteArray &cmd, unsigned flags);
    void postBuiltinCommand(const QByteArray &cmd,
                            unsigned flags,
                            BuiltinCommandHandler handler,
                            unsigned nextCommandFlag = 0,
                            const QVariant &cookie = QVariant());

    void postExtensionCommand(const QByteArray &cmd,
                              const QByteArray &arguments,
                              unsigned flags,
                              ExtensionCommandHandler handler,
                              unsigned nextCommandFlag = 0,
                              const QVariant &cookie = QVariant());

    void postCommandSequence(unsigned mask);
    void operateByInstructionTriggered(bool);

    void consoleStubMessage(const QString &, bool);
    void consoleStubProcessStarted();
    void consoleStubExited();

private:
    typedef QHash<BreakpointId, BreakpointResponse> PendingBreakPointMap;

    enum SpecialStopMode
    {
        NoSpecialStop,
        SpecialStopSynchronizeBreakpoints,
        SpecialStopGetWidgetAt
    };
    enum ParseStackResultFlags // Flags returned by parseStackTrace
    {
        ParseStackStepInto = 1 // Need to execute a step, hit on a call frame in "Step into"
    };


    bool startConsole(const DebuggerStartParameters &sp, QString *errorMessage);
    void init();
    unsigned examineStopReason(const GdbMi &stopReason, QString *message,
                               QString *exceptionBoxMessage);
    bool commandsPending() const;
    void handleExtensionMessage(char t, int token, const QByteArray &what, const QByteArray &message);
    bool doSetupEngine(QString *errorMessage);
    bool launchCDB(const DebuggerStartParameters &sp, QString *errorMessage);
    void handleSessionAccessible(unsigned long cdbExState);
    void handleSessionInaccessible(unsigned long cdbExState);
    void handleSessionIdle(const QByteArray &message);
    void doInterruptInferior(SpecialStopMode sm);
    void doContinueInferior();
    inline void parseOutputLine(QByteArray line);
    inline bool isCdbProcessRunning() const { return m_process.state() != QProcess::NotRunning; }
    bool canInterruptInferior() const;
    void syncOperateByInstruction(bool operateByInstruction);
    void postWidgetAtCommand();

    // Builtin commands
    void dummyHandler(const CdbBuiltinCommandPtr &);
    void handleStackTrace(const CdbExtensionCommandPtr &);
    void handleRegisters(const CdbBuiltinCommandPtr &);
    void handleDisassembler(const CdbBuiltinCommandPtr &);
    void handleJumpToLineAddressResolution(const CdbBuiltinCommandPtr &);
    // Extension commands
    void handleThreads(const CdbExtensionCommandPtr &);
    void handlePid(const CdbExtensionCommandPtr &reply);
    void handleLocals(const CdbExtensionCommandPtr &reply);
    void handleAddWatch(const CdbExtensionCommandPtr &reply);
    void handleExpandLocals(const CdbExtensionCommandPtr &reply);
    void handleRegisters(const CdbExtensionCommandPtr &reply);
    void handleModules(const CdbExtensionCommandPtr &reply);
    void handleMemory(const CdbExtensionCommandPtr &);
    void handleWidgetAt(const CdbExtensionCommandPtr &);
    void handleBreakPoints(const CdbExtensionCommandPtr &);
    void handleBreakPoints(const GdbMi &value);

    QString normalizeFileName(const QString &f);
    void updateLocalVariable(const QByteArray &iname);
    void updateLocals(bool forNewStackFrame = false);
    int elapsedLogTime() const;
    void addLocalsOptions(ByteArrayInputStream &s) const;
    unsigned parseStackTrace(const GdbMi &data, bool sourceStepInto);
    void parseThreads(const GdbMi &, int forceCurrentThreadId = -1);

    const QByteArray m_creatorExtPrefix;
    const QByteArray m_tokenPrefix;
    const OptionsPtr m_options;

    QProcess m_process;
    QScopedPointer<Utils::ConsoleProcess> m_consoleStub;
    DebuggerStartMode m_effectiveStartMode;
    QByteArray m_outputBuffer;
    unsigned long m_inferiorPid;
    //! Debugger accessible (expecting commands)
    bool m_accessible;
    SpecialStopMode m_specialStopMode;
    int m_nextCommandToken;
    QList<CdbBuiltinCommandPtr> m_builtinCommandQueue;
    int m_currentBuiltinCommandIndex; //!< Current command whose output is recorded.
    QList<CdbExtensionCommandPtr> m_extensionCommandQueue;
    QMap<QString, QString> m_normalizedFileCache;
    const QByteArray m_extensionCommandPrefixBA; //!< Library name used as prefix
    bool m_operateByInstructionPending; //!< Creator operate by instruction action changed.
    bool m_operateByInstruction;
    bool m_notifyEngineShutdownOnTermination;
    bool m_hasDebuggee;
    QTime m_logTime;
    mutable int m_elapsedLogTime;
    QByteArray m_extensionMessageBuffer;
    bool m_sourceStepInto;
    int m_watchPointX;
    int m_watchPointY;
    PendingBreakPointMap m_pendingBreakpointMap;
    QHash<QString, QString> m_fileNameModuleHash;
    bool m_ignoreCdbOutput;
};

} // namespace Internal
} // namespace Debugger

#endif // DEBUGGER_CDBENGINE_H
