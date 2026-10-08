/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT
*/
#pragma once

#include <QList>
#include <QString>

class QImage;

/**
 * Pure text generators behind the ASCII Fun plugin. They only depend on Qt
 * so they can be unit tested without a running editor.
 */
namespace AsciiArt
{
/// Every number found in @p text, in order. Anything that does not parse is skipped.
QList<double> parseNumbers(const QString &text);

/// Line chart of @p series, @p height rows tall (port of asciichart's plot()).
/// Returns an empty string when @p series is empty.
QString plot(const QList<double> &series, int height);

/// @p text in 7-row banner letters (glyphs from the ascii-art "banner" font).
/// Letters are upper-cased; characters without a glyph render as blanks.
QString banner(const QString &text);

/// Box-drawn table from comma- or tab-separated @p text; the first row is the header.
QString table(const QString &text);

/// @p image as ASCII, @p columns characters wide (brightness ramp from ascii-view).
QString imageToAscii(const QImage &image, int columns);
}
