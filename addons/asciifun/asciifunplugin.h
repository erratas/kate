/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT
*/
#pragma once

#include <KTextEditor/Plugin>
#include <KXMLGUIClient>

#include <QPointer>

#include <functional>

namespace KTextEditor
{
class MainWindow;
class View;
}
class MatrixRain;
class QImage;

class AsciiFunPlugin final : public KTextEditor::Plugin
{
public:
    explicit AsciiFunPlugin(QObject *parent);
    QObject *createView(KTextEditor::MainWindow *mainWindow) override;
};

class AsciiFunPluginView final : public QObject, public KXMLGUIClient
{
public:
    explicit AsciiFunPluginView(KTextEditor::MainWindow *mainWindow);
    ~AsciiFunPluginView() override;

private:
    void chartSelection();
    void bannerSelection();
    void tableSelection();
    void insertImage();
    void insertBrailleImage();
    void insertImageAs(const std::function<QString(const QImage &)> &convert);
    void startMatrixRain();

    /// Active view with a non-empty selection, or nullptr after telling the user what to select.
    KTextEditor::View *viewWithSelection(const QString &hint);

    KTextEditor::MainWindow *const m_mainWindow;
    QPointer<MatrixRain> m_rain;
    QString m_bannerFont;
};
