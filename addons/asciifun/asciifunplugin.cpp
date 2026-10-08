/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT
*/
#include "asciifunplugin.h"

#include "asciiart.h"
#include "matrixrain.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KTextEditor/Document>
#include <KTextEditor/MainWindow>
#include <KTextEditor/Message>
#include <KTextEditor/View>
#include <KXMLGUIFactory>

#include <QAction>
#include <QFileDialog>
#include <QImage>
#include <QImageReader>
#include <QInputDialog>

K_PLUGIN_FACTORY_WITH_JSON(AsciiFunPluginFactory, "asciifunplugin.json", registerPlugin<AsciiFunPlugin>();)

namespace
{
constexpr int ChartHeight = 10;
constexpr int ImageColumns = 80;

/// Replace the selection with a multi-line @p block, starting it on a fresh line when the
/// selection begins mid-line so that every row of the block stays aligned.
void replaceSelectionWithBlock(KTextEditor::View *view, const QString &block)
{
    const KTextEditor::Range range = view->selectionRange();
    const QString prefix = range.start().column() > 0 ? QStringLiteral("\n") : QString();
    view->document()->replaceText(range, prefix + block);
}

void showMessage(KTextEditor::View *view, const QString &text, KTextEditor::Message::MessageType type)
{
    auto *message = new KTextEditor::Message(text, type);
    message->setAutoHide(4000);
    view->document()->postMessage(message);
}
}

AsciiFunPlugin::AsciiFunPlugin(QObject *parent)
    : KTextEditor::Plugin(parent)
{
}

QObject *AsciiFunPlugin::createView(KTextEditor::MainWindow *mainWindow)
{
    return new AsciiFunPluginView(mainWindow);
}

AsciiFunPluginView::AsciiFunPluginView(KTextEditor::MainWindow *mainWindow)
    : QObject(mainWindow)
    , m_mainWindow(mainWindow)
{
    KXMLGUIClient::setComponentName(QStringLiteral("asciifun"), i18n("ASCII Fun"));
    setXMLFile(QStringLiteral("ui.rc"));

    auto addAction = [this](const char *name, const QString &text, void (AsciiFunPluginView::*slot)()) {
        QAction *a = actionCollection()->addAction(QString::fromLatin1(name));
        a->setText(text);
        connect(a, &QAction::triggered, this, slot);
    };
    addAction("asciifun_chart", i18n("Chart Selected Numbers"), &AsciiFunPluginView::chartSelection);
    addAction("asciifun_banner", i18n("Selection to Banner Letters"), &AsciiFunPluginView::bannerSelection);
    addAction("asciifun_table", i18n("CSV Selection to Table"), &AsciiFunPluginView::tableSelection);
    addAction("asciifun_image", i18n("Insert Image as ASCII…"), &AsciiFunPluginView::insertImage);
    addAction("asciifun_braille", i18n("Insert Image as Braille…"), &AsciiFunPluginView::insertBrailleImage);

    // The easter egg stays out of the menus and the shortcut editor on purpose
    auto *rain = new QAction(this);
    rain->setShortcut(Qt::CTRL | Qt::ALT | Qt::SHIFT | Qt::Key_M);
    rain->setShortcutContext(Qt::WindowShortcut);
    connect(rain, &QAction::triggered, this, &AsciiFunPluginView::startMatrixRain);
    m_mainWindow->window()->addAction(rain);

    m_mainWindow->guiFactory()->addClient(this);
}

AsciiFunPluginView::~AsciiFunPluginView()
{
    m_mainWindow->guiFactory()->removeClient(this);
    delete m_rain;
}

KTextEditor::View *AsciiFunPluginView::viewWithSelection(const QString &hint)
{
    KTextEditor::View *view = m_mainWindow->activeView();
    if (!view) {
        return nullptr;
    }
    if (!view->selection() || view->selectionText().trimmed().isEmpty()) {
        showMessage(view, hint, KTextEditor::Message::Information);
        return nullptr;
    }
    return view;
}

void AsciiFunPluginView::chartSelection()
{
    KTextEditor::View *view = viewWithSelection(i18n("Select some numbers to chart."));
    if (!view) {
        return;
    }
    const QList<QList<double>> series = AsciiArt::parseSeries(view->selectionText());
    if (series.isEmpty()) {
        showMessage(view, i18n("No numbers found in the selection."), KTextEditor::Message::Warning);
        return;
    }
    // Keep the data: the chart goes on new lines after the selection
    const KTextEditor::Cursor end = view->selectionRange().end();
    view->document()->insertText(end, QLatin1Char('\n') + AsciiArt::plot(series, ChartHeight) + QLatin1Char('\n'));
}

void AsciiFunPluginView::bannerSelection()
{
    KTextEditor::View *view = viewWithSelection(i18n("Select some text to turn into banner letters."));
    if (!view) {
        return;
    }
    const QStringList fonts = AsciiArt::bannerFonts();
    bool ok = false;
    const QString font =
        QInputDialog::getItem(m_mainWindow->window(), i18n("Banner Letters"), i18n("Font:"), fonts, std::max(0, int(fonts.indexOf(m_bannerFont))), false, &ok);
    if (!ok) {
        return;
    }
    m_bannerFont = font;
    replaceSelectionWithBlock(view, AsciiArt::banner(view->selectionText(), font));
}

void AsciiFunPluginView::tableSelection()
{
    KTextEditor::View *view = viewWithSelection(i18n("Select comma- or tab-separated lines to turn into a table."));
    if (!view) {
        return;
    }
    replaceSelectionWithBlock(view, AsciiArt::table(view->selectionText()));
}

void AsciiFunPluginView::insertImage()
{
    insertImageAs([](const QImage &image) {
        return AsciiArt::imageToAscii(image, ImageColumns);
    });
}

void AsciiFunPluginView::insertBrailleImage()
{
    insertImageAs([](const QImage &image) {
        return AsciiArt::imageToBraille(image, ImageColumns);
    });
}

void AsciiFunPluginView::insertImageAs(const std::function<QString(const QImage &)> &convert)
{
    KTextEditor::View *view = m_mainWindow->activeView();
    if (!view) {
        return;
    }

    QStringList patterns;
    const QList<QByteArray> formats = QImageReader::supportedImageFormats();
    for (const QByteArray &format : formats) {
        patterns.append(QStringLiteral("*.") + QString::fromLatin1(format));
    }
    const QString path =
        QFileDialog::getOpenFileName(m_mainWindow->window(), i18n("Insert Image"), QString(), i18n("Images (%1)", patterns.join(QLatin1Char(' '))));
    if (path.isEmpty()) {
        return;
    }

    QImageReader reader(path);
    const QImage image = reader.read();
    if (image.isNull()) {
        showMessage(view, i18n("Could not read image %1: %2", path, reader.errorString()), KTextEditor::Message::Error);
        return;
    }
    view->document()->insertText(view->cursorPosition(), convert(image) + QLatin1Char('\n'));
}

void AsciiFunPluginView::startMatrixRain()
{
    if (m_rain) {
        return;
    }
    KTextEditor::View *view = m_mainWindow->activeView();
    m_rain = new MatrixRain(view ? static_cast<QWidget *>(view) : m_mainWindow->window());
}

#include "asciifunplugin.moc"
