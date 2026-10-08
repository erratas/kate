/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT

    Algorithms ported from MIT-licensed projects:
      plot()         asciichart, Copyright (c) 2016 Igor Kroitor
      banner glyphs  ascii-art "banner" font, Copyright (c) 2024 codewithnick
      ramp           ascii-view, Copyright (c) 2025 Xander Gouws
*/
#include "asciiart.h"

#include <QHash>
#include <QImage>
#include <QRegularExpression>
#include <QStringList>

#include <algorithm>
#include <cmath>

namespace AsciiArt
{
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

QString plot(const QList<double> &series, int height)
{
    if (series.isEmpty() || height < 1) {
        return {};
    }

    const auto [minIt, maxIt] = std::minmax_element(series.cbegin(), series.cend());
    const double minimum = *minIt;
    const double maximum = *maxIt;
    const double interval = maximum - minimum;
    const int offset = 3;
    const double ratio = interval > 0 ? height / interval : 1;

    const int min2 = int(std::floor(minimum * ratio));
    const int max2 = int(std::ceil(maximum * ratio));
    const int rows = max2 - min2;
    const int width = int(series.size()) + offset;

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
    grid[rows - scaled(series.first())][offset - 1] = QStringLiteral("┼");

    for (int x = 0; x + 1 < series.size(); ++x) {
        const int y0 = scaled(series[x]);
        const int y1 = scaled(series[x + 1]);
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

    QStringList lines;
    for (const QStringList &row : std::as_const(grid)) {
        static const QRegularExpression trailingSpace(QStringLiteral("\\s+$"));
        lines.append(row.join(QString()).remove(trailingSpace));
    }
    return lines.join(QLatin1Char('\n'));
}

static const QHash<QChar, QStringList> &bannerFont()
{
    static const QHash<QChar, QStringList> font = {
    {QChar(u'A'), {QStringLiteral("   #   "), QStringLiteral("  # #  "), QStringLiteral(" #   # "), QStringLiteral("#     #"), QStringLiteral("#######"), QStringLiteral("#     #"), QStringLiteral("#     #")}},
    {QChar(u'B'), {QStringLiteral("#######"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#### # "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#######")}},
    {QChar(u'C'), {QStringLiteral(" ##### "), QStringLiteral("#     #"), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#     #"), QStringLiteral(" ##### ")}},
    {QChar(u'D'), {QStringLiteral("###### "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("###### ")}},
    {QChar(u'E'), {QStringLiteral("#######"), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#####  "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#######")}},
    {QChar(u'F'), {QStringLiteral("#######"), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#####  "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#      ")}},
    {QChar(u'G'), {QStringLiteral(" ##### "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#  ####"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral(" # ### ")}},
    {QChar(u'H'), {QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#######"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #")}},
    {QChar(u'I'), {QStringLiteral("  ###  "), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("  ###  ")}},
    {QChar(u'J'), {QStringLiteral("      #"), QStringLiteral("      #"), QStringLiteral("      #"), QStringLiteral("      #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral(" ##### ")}},
    {QChar(u'K'), {QStringLiteral("#    # "), QStringLiteral("#   #  "), QStringLiteral("#  #   "), QStringLiteral("###    "), QStringLiteral("#  #   "), QStringLiteral("#   #  "), QStringLiteral("#    # ")}},
    {QChar(u'L'), {QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#######")}},
    {QChar(u'M'), {QStringLiteral("#     #"), QStringLiteral("##   ##"), QStringLiteral("# # # #"), QStringLiteral("#  #  #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #")}},
    {QChar(u'N'), {QStringLiteral("#     #"), QStringLiteral("##    #"), QStringLiteral("# #   #"), QStringLiteral("#  #  #"), QStringLiteral("#   # #"), QStringLiteral("#    ##"), QStringLiteral("#     #")}},
    {QChar(u'O'), {QStringLiteral("#######"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#######")}},
    {QChar(u'P'), {QStringLiteral("###### "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("###### "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#      ")}},
    {QChar(u'Q'), {QStringLiteral(" ##### "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#   # #"), QStringLiteral("#    # "), QStringLiteral(" #### #")}},
    {QChar(u'R'), {QStringLiteral("###### "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("###### "), QStringLiteral("#   #  "), QStringLiteral("#    # "), QStringLiteral("#     #")}},
    {QChar(u'S'), {QStringLiteral(" ##### "), QStringLiteral("#     #"), QStringLiteral("#      "), QStringLiteral(" ##### "), QStringLiteral("      #"), QStringLiteral("#     #"), QStringLiteral(" ##### ")}},
    {QChar(u'T'), {QStringLiteral("#######"), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("   #   ")}},
    {QChar(u'U'), {QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral(" ##### ")}},
    {QChar(u'V'), {QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral(" #   # "), QStringLiteral("  # #  "), QStringLiteral("   #   ")}},
    {QChar(u'W'), {QStringLiteral("#     #"), QStringLiteral("#  #  #"), QStringLiteral("#  #  #"), QStringLiteral("#  #  #"), QStringLiteral("#  #  #"), QStringLiteral("#  #  #"), QStringLiteral(" ## ## ")}},
    {QChar(u'X'), {QStringLiteral("#     #"), QStringLiteral(" #   # "), QStringLiteral("  # #  "), QStringLiteral("   #   "), QStringLiteral("  # #  "), QStringLiteral(" #   # "), QStringLiteral("#     #")}},
    {QChar(u'Y'), {QStringLiteral("#     #"), QStringLiteral(" #   # "), QStringLiteral("  # #  "), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("   #   "), QStringLiteral("   #   ")}},
    {QChar(u'Z'), {QStringLiteral("#######"), QStringLiteral("     # "), QStringLiteral("    #  "), QStringLiteral("   #   "), QStringLiteral("  #    "), QStringLiteral(" #     "), QStringLiteral("#######")}},
    {QChar(u'0'), {QStringLiteral("  ###  "), QStringLiteral(" #   # "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral(" #   # "), QStringLiteral("  ###  ")}},
    {QChar(u'1'), {QStringLiteral("  #    "), QStringLiteral(" ##    "), QStringLiteral("# #    "), QStringLiteral("  #    "), QStringLiteral("  #    "), QStringLiteral("  #    "), QStringLiteral("#####  ")}},
    {QChar(u'2'), {QStringLiteral(" ##### "), QStringLiteral("#     #"), QStringLiteral("      #"), QStringLiteral(" ##### "), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("#######")}},
    {QChar(u'3'), {QStringLiteral(" ##### "), QStringLiteral("#     #"), QStringLiteral("      #"), QStringLiteral(" ##### "), QStringLiteral("      #"), QStringLiteral("#     #"), QStringLiteral(" ##### ")}},
    {QChar(u'4'), {QStringLiteral("#      "), QStringLiteral("#    # "), QStringLiteral("#    # "), QStringLiteral("#    # "), QStringLiteral(" ######"), QStringLiteral("     # "), QStringLiteral("     # ")}},
    {QChar(u'5'), {QStringLiteral("#######"), QStringLiteral("#      "), QStringLiteral("#      "), QStringLiteral("###### "), QStringLiteral("      #"), QStringLiteral("#     #"), QStringLiteral(" ##### ")}},
    {QChar(u'6'), {QStringLiteral(" ##### "), QStringLiteral("#     #"), QStringLiteral("#      "), QStringLiteral("###### "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral(" ##### ")}},
    {QChar(u'7'), {QStringLiteral("#######"), QStringLiteral("#    # "), QStringLiteral("    #  "), QStringLiteral("   #   "), QStringLiteral("  #    "), QStringLiteral("  #    "), QStringLiteral("  #    ")}},
    {QChar(u'8'), {QStringLiteral(" ##### "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral(" ##### "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral(" ##### ")}},
    {QChar(u'9'), {QStringLiteral(" ##### "), QStringLiteral("#     #"), QStringLiteral("#     #"), QStringLiteral(" ######"), QStringLiteral("      #"), QStringLiteral("#     #"), QStringLiteral(" #### #")}},
    };
    return font;
}

QString banner(const QString &text)
{
    constexpr int glyphRows = 7;
    static const QStringList blank(glyphRows, QString(7, QLatin1Char(' ')));
    static const QRegularExpression trailingSpace(QStringLiteral("\\s+$"));

    QStringList out;
    const QStringList inputLines = text.split(QLatin1Char('\n'));
    for (const QString &inputLine : inputLines) {
        if (!out.isEmpty()) {
            out.append(QString());
        }
        QStringList rows(glyphRows);
        const QString upper = inputLine.toUpper();
        for (const QChar c : upper) {
            const QStringList &glyph = bannerFont().contains(c) ? bannerFont()[c] : blank;
            for (int r = 0; r < glyphRows; ++r) {
                rows[r] += glyph[r] + QLatin1Char(' ');
            }
        }
        for (QString &row : rows) {
            out.append(row.remove(trailingSpace));
        }
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

QString imageToAscii(const QImage &image, int columns)
{
    if (image.isNull() || columns < 1) {
        return {};
    }
    static const QString ramp = QStringLiteral(" .-=+*x#$&X@");

    // Characters are roughly twice as tall as they are wide
    const int rows = std::max(1, int(std::lround(double(image.height()) * columns / image.width() / 2.0)));
    const QImage small = image.convertToFormat(QImage::Format_RGB32).scaled(columns, rows, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    static const QRegularExpression trailingSpace(QStringLiteral("\\s+$"));
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
        lines.append(line.remove(trailingSpace));
    }
    return lines.join(QLatin1Char('\n'));
}
}
