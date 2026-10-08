/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT
*/
#pragma once

#include <QList>
#include <QString>
#include <QStringList>

class QImage;

/**
 * Pure text generators behind the ASCII Fun plugin. They only depend on Qt
 * so they can be unit tested without a running editor.
 */
namespace AsciiArt
{
/// Every number found in @p text, in order. Anything that does not parse is skipped.
QList<double> parseNumbers(const QString &text);

/// Series to chart from @p text: when more than one line holds two or more numbers,
/// each such line is its own series; otherwise all numbers form a single series.
QList<QList<double>> parseSeries(const QString &text);

/// Line chart of one or more @p series sharing one y axis, @p height rows tall
/// (port of asciichart's plot()). Returns an empty string when there is nothing to plot.
QString plot(const QList<QList<double>> &series, int height);

/// Names of the bundled banner fonts (from the ascii-art project), "banner" first.
QStringList bannerFonts();

/// @p text in letters of banner font @p font. Lowercase falls back to uppercase when the
/// font has no lowercase glyph; other unknown characters render as blanks.
/// Returns an empty string for an unknown font.
QString banner(const QString &text, const QString &font);

/// Box-drawn table from comma- or tab-separated @p text; the first row is the header.
QString table(const QString &text);

/// @p image as ASCII, @p columns characters wide (brightness ramp from ascii-view).
QString imageToAscii(const QImage &image, int columns);

/// @p image as Unicode braille, @p columns characters wide; each character holds a
/// 2x4 dot grid and a dot is set where brightness >= @p threshold (0-255), as in
/// ascii-image-converter.
QString imageToBraille(const QImage &image, int columns, int threshold = 128);
}
