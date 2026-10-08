/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT

    Algorithms ported from:
      plot()           asciichart, MIT, Copyright (c) 2016 Igor Kroitor
      banner fonts     ascii-art, MIT, Copyright (c) 2024 codewithnick
      ASCII ramp       ascii-view, MIT, Copyright (c) 2025 Xander Gouws
      braille dots     ascii-image-converter, Apache-2.0, Copyright (c) Zoraiz Hassan
*/
#include "asciiart.h"

#include <QFile>
#include <QHash>
#include <QImage>
#include <QRegularExpression>

#include <algorithm>
#include <cmath>

// Q_INIT_RESOURCE must be used outside of any namespace
static void initFontResources()
{
    Q_INIT_RESOURCE(asciifonts);
}

namespace AsciiArt
{
static QString trimRight(QString s)
{
    static const QRegularExpression trailingSpace(QStringLiteral("\\s+$"));
    return s.remove(trailingSpace);
}

QList<double> parseNumbers(const QString &text)
{
    static const QRegularExpression separators(QStringLiteral("[\\s,;]+"));
    QList<double> numbers;
    const QStringList tokens = text.split(separators, Qt::SkipEmptyParts);
    for (const QString &token : tokens) {
        bool ok = false;
        const double value = token.toDouble(&ok);
        if (ok && std::isfinite(value)) {
            numbers.append(value);
        }
    }
    return numbers;
}

QList<QList<double>> parseSeries(const QString &text)
{
    QList<QList<double>> lines;
    const QStringList textLines = text.split(QLatin1Char('\n'));
    for (const QString &line : textLines) {
        QList<double> numbers = parseNumbers(line);
        if (numbers.size() >= 2) {
            lines.append(numbers);
        }
    }
    if (lines.size() > 1) {
        return lines;
    }
    const QList<double> all = parseNumbers(text);
    return all.isEmpty() ? QList<QList<double>>{} : QList<QList<double>>{all};
}

QString plot(const QList<QList<double>> &series, int height)
{
    if (series.isEmpty() || series.first().isEmpty() || height < 1) {
        return {};
    }

    double minimum = series.first().first();
    double maximum = minimum;
    qsizetype longest = 0;
    for (const QList<double> &s : series) {
        for (double v : s) {
            minimum = std::min(minimum, v);
            maximum = std::max(maximum, v);
        }
        longest = std::max(longest, s.size());
    }
    const double interval = maximum - minimum;
    const int offset = 3;
    const double ratio = interval > 0 ? height / interval : 1;

    const int min2 = int(std::floor(minimum * ratio));
    const int max2 = int(std::ceil(maximum * ratio));
    const int rows = max2 - min2;
    const int width = int(longest) + offset;

    // std::nearbyint rounds half to even, matching Python's round() in asciichart
    auto scaled = [&](double y) {
        return int(std::nearbyint(std::clamp(y, minimum, maximum) * ratio) - min2);
    };

    // One cell per column; a label occupies a single (wide) cell like in asciichart.
    QList<QStringList> grid(rows + 1, QStringList(width, QStringLiteral(" ")));

    for (int y = min2; y <= max2; ++y) {
        const double magnitude = maximum - ((y - min2) * interval / (rows ? rows : 1));
        const QString label = QString::asprintf("%8.2f ", magnitude);
        grid[y - min2][std::max(offset - int(label.size()), 0)] = label;
        grid[y - min2][offset - 1] = y == 0 ? QStringLiteral("┼") : QStringLiteral("┤");
    }
    grid[rows - scaled(series.first().first())][offset - 1] = QStringLiteral("┼");

    for (const QList<double> &s : series) {
        for (int x = 0; x + 1 < s.size(); ++x) {
            const int y0 = scaled(s[x]);
            const int y1 = scaled(s[x + 1]);
            if (y0 == y1) {
                grid[rows - y0][x + offset] = QStringLiteral("─");
                continue;
            }
            grid[rows - y1][x + offset] = y0 > y1 ? QStringLiteral("╰") : QStringLiteral("╭");
            grid[rows - y0][x + offset] = y0 > y1 ? QStringLiteral("╮") : QStringLiteral("╯");
            for (int y = std::min(y0, y1) + 1; y < std::max(y0, y1); ++y) {
                grid[rows - y][x + offset] = QStringLiteral("│");
            }
        }
    }

    QStringList lines;
    for (const QStringList &row : std::as_const(grid)) {
        lines.append(trimRight(row.join(QString())));
    }
    return lines.join(QLatin1Char('\n'));
}

namespace
{
struct BannerFont {
    int height = 0;
    QHash<QChar, QStringList> glyphs; // every row padded to the glyph's width
};

/// Font file format: "height N", then per glyph "@<char> <width>" followed by N rows.
BannerFont loadFont(const QString &name)
{
    initFontResources();
    QFile file(QStringLiteral(":/asciifun/fonts/%1.txt").arg(name));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    const QStringList lines = QString::fromUtf8(file.readAll()).split(QLatin1Char('\n'));
    BannerFont font;
    font.height = lines.value(0).section(QLatin1Char(' '), 1).toInt();
    for (qsizetype i = 1; i < lines.size(); ++i) {
        const QString &header = lines[i];
        if (!header.startsWith(QLatin1Char('@')) || header.size() < 4) {
            continue;
        }
        const QChar c = header[1];
        const int width = header.mid(3).toInt();
        QStringList rows;
        for (int r = 0; r < font.height; ++r) {
            rows.append(lines.value(i + 1 + r).leftJustified(width, QLatin1Char(' '), true));
        }
        font.glyphs.insert(c, rows);
        i += font.height;
    }
    return font;
}

const BannerFont *font(const QString &name)
{
    static QHash<QString, BannerFont> cache;
    if (!bannerFonts().contains(name)) {
        return nullptr;
    }
    auto it = cache.find(name);
    if (it == cache.end()) {
        it = cache.insert(name, loadFont(name));
    }
    return &it.value();
}
}

QStringList bannerFonts()
{
    return {QStringLiteral("banner"),
            QStringLiteral("amongus"),
            QStringLiteral("block"),
            QStringLiteral("boomer"),
            QStringLiteral("carlos"),
            QStringLiteral("drpepper"),
            QStringLiteral("sevenstar"),
            QStringLiteral("small"),
            QStringLiteral("starwar"),
            QStringLiteral("straight")};
}

QString banner(const QString &text, const QString &fontName)
{
    const BannerFont *f = font(fontName);
    if (!f || f->height == 0) {
        return {};
    }
    const QStringList blank(f->height, QString(f->glyphs.value(QLatin1Char(' ')).value(0).size(), QLatin1Char(' ')));

    QStringList out;
    const QStringList inputLines = text.split(QLatin1Char('\n'));
    for (const QString &inputLine : inputLines) {
        if (!out.isEmpty()) {
            out.append(QString());
        }
        QStringList rows(f->height);
        for (const QChar c : inputLine) {
            auto it = f->glyphs.constFind(c);
            if (it == f->glyphs.cend()) {
                it = f->glyphs.constFind(c.toUpper());
            }
            const QStringList &glyph = it != f->glyphs.cend() ? it.value() : blank;
            // ascii-art leaves two blank columns between glyphs
            for (int r = 0; r < f->height; ++r) {
                rows[r] += glyph[r] + QStringLiteral("  ");
            }
        }
        // ascii-art only grows a line for glyphs taller than the font's base height
        for (QString &row : rows) {
            row = trimRight(row);
        }
        while (rows.size() > 1 && rows.last().isEmpty()) {
            rows.removeLast();
        }
        out.append(rows);
    }
    return out.join(QLatin1Char('\n'));
}

QString table(const QString &text)
{
    const QChar separator = text.contains(QLatin1Char('\t')) ? QLatin1Char('\t') : QLatin1Char(',');

    QList<QStringList> rows;
    const QStringList lines = text.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        if (line.trimmed().isEmpty()) {
            continue;
        }
        QStringList cells = line.split(separator);
        for (QString &cell : cells) {
            cell = cell.trimmed();
        }
        rows.append(cells);
    }
    if (rows.isEmpty()) {
        return {};
    }

    qsizetype columns = 0;
    for (const QStringList &row : std::as_const(rows)) {
        columns = std::max(columns, row.size());
    }
    QList<qsizetype> widths(columns, 0);
    for (QStringList &row : rows) {
        while (row.size() < columns) {
            row.append(QString());
        }
        for (qsizetype c = 0; c < columns; ++c) {
            widths[c] = std::max(widths[c], row[c].size());
        }
    }

    // Box characters follow ASCII-Data's UTF8TableFormat
    auto rule = [&](QChar left, QChar fill, QChar cross, QChar right) {
        QString s(left);
        for (qsizetype c = 0; c < columns; ++c) {
            s += QString(widths[c] + 2, fill);
            s += c + 1 < columns ? cross : right;
        }
        return s;
    };
    auto content = [&](const QStringList &row) {
        QString s(QStringLiteral("║"));
        for (qsizetype c = 0; c < columns; ++c) {
            s += QLatin1Char(' ') + row[c].leftJustified(widths[c]) + QLatin1Char(' ');
            s += c + 1 < columns ? QStringLiteral("│") : QStringLiteral("║");
        }
        return s;
    };

    QStringList out;
    out.append(rule(u'╔', u'═', u'╤', u'╗'));
    out.append(content(rows.first()));
    if (rows.size() > 1) {
        out.append(rule(u'╠', u'═', u'╪', u'╣'));
        for (qsizetype r = 1; r < rows.size(); ++r) {
            out.append(content(rows[r]));
        }
    }
    out.append(rule(u'╚', u'═', u'╧', u'╝'));
    return out.join(QLatin1Char('\n'));
}

/// @p image scaled so that @p columns characters of @p dotsWide x @p dotsHigh dots each
/// keep the picture's proportions; characters are about twice as tall as they are wide.
static QImage scaledForText(const QImage &image, int columns, int dotsWide, int dotsHigh)
{
    const int rows = std::max(1, int(std::lround(double(image.height()) * columns / image.width() / 2.0)));
    return image.convertToFormat(QImage::Format_RGB32).scaled(columns * dotsWide, rows * dotsHigh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QString imageToAscii(const QImage &image, int columns)
{
    if (image.isNull() || columns < 1) {
        return {};
    }
    static const QString ramp = QStringLiteral(" .-=+*x#$&X@");
    const QImage small = scaledForText(image, columns, 1, 1);

    QStringList lines;
    for (int y = 0; y < small.height(); ++y) {
        QString line;
        for (int x = 0; x < small.width(); ++x) {
            const QRgb px = small.pixel(x, y);
            const double value = std::max({qRed(px), qGreen(px), qBlue(px)}) / 255.0;
            // value squared for contrast, as ascii-view does
            const int index = std::min(int(value * value * ramp.size()), int(ramp.size()) - 1);
            line += ramp[index];
        }
        lines.append(trimRight(line));
    }
    return lines.join(QLatin1Char('\n'));
}

QString imageToBraille(const QImage &image, int columns, int threshold)
{
    if (image.isNull() || columns < 1) {
        return {};
    }
    // Unicode braille dot bits, indexed [row][column] within a character
    static constexpr int dotBits[4][2] = {{0x1, 0x8}, {0x2, 0x10}, {0x4, 0x20}, {0x40, 0x80}};
    const QImage dots = scaledForText(image, columns, 2, 4);

    QStringList lines;
    for (int y = 0; y < dots.height(); y += 4) {
        QString line;
        for (int x = 0; x < dots.width(); x += 2) {
            char16_t c = 0x2800;
            for (int r = 0; r < 4; ++r) {
                for (int col = 0; col < 2; ++col) {
                    if (qGray(dots.pixel(x + col, y + r)) >= threshold) {
                        c |= dotBits[r][col];
                    }
                }
            }
            line += QChar(c);
        }
        lines.append(line);
    }
    return lines.join(QLatin1Char('\n'));
}
}
