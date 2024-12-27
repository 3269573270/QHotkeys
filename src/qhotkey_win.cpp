#ifndef QHOTKEY_WIN_H
#define QHOTKEY_WIN_H
#include <QtCore>

#ifdef Q_OS_WIN
#include "qhotkey.h"
#include "helper.h"
#include <Windows.h>

int Qt::QHotkey::_ghkid = 0;
struct Qt::QHotkey::PlatformData
{
    int thrId;
    UINT wmId;
};

Qt::QHotkey::QHotkey(const QVector<int>& modifiers, const int key)
    : _registered(false),
    _modifiers(modifiers),_key_int(key),
    _hkid(_ghkid++),
    _loop(&Qt::QHotkey::registerHotkey, this),
    _pData(new PlatformData)
// ,
// registrationStatus(false)
{}
// Qt::QHotkey::QHotkey(const QVector<Qt::ModifierKey>& modifiers, const Qt::Key key)
//     : _modifiers(modifiers), _key(key),
//     _hkid(_ghkid++), _registered(false),
//     _loop(&Qt::QHotkey::registerHotkey, this),
//     _pData(new PlatformData),
//     registrationStatus(false)
// {}

Qt::QHotkey::~QHotkey()
{
    // Send WM_QHOTKEY_UNHOOK message to messageLoop()
    PostThreadMessage(_pData->thrId, _pData->wmId, NULL, NULL);
    _loop.join();
    UnregisterHotKey(NULL, _hkid);
    delete _pData;
}

void Qt::QHotkey::registerHotkey()
{
    // registrationStatus = true;
    _pData->wmId = RegisterWindowMessage(L"WM_QHOTKEY_UNHOOK");
    _pData->thrId = GetCurrentThreadId();

    // TODO: 改为发送信号
    if (_registered)
        // registrationStatus = false;
        qCritical() << "有bug  This QHotkey instance is already registered!";
    //throw std::runtime_error("This QHotkey instance is already registered!");

    // auto result = RegisterHotKey(NULL, _hkid, getMod(_modifiers), getKey(_key));
    auto result = RegisterHotKey(NULL, _hkid, getMod(_modifiers), _key_int);
    if (result == FALSE)
        // registrationStatus = false;
        qCritical() << "有bug  Could not register hotkey! #";
    // throw std::runtime_error("Could not register hotkey! #");//GetLastError()

    _registered = result == 0;
    messageLoop();
}

void Qt::QHotkey::messageLoop() const
{
    MSG msg;
    while (GetMessage(&msg, NULL, NULL, NULL)) {
        if (msg.message == _pData->wmId) return;
        if (msg.message == WM_HOTKEY &&
            msg.wParam == _hkid) {
            if (msg.wParam == _hkid) {
                emit pressed(*this); // Qt callback slot
            }
        }
    }
}
#endif // Q_OS_WIN

#endif // QHOTKEY_WIN_H
