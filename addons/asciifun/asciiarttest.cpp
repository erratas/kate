/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT
*/
#include "asciiart.h"

#include <QImage>
#include <QObject>
#include <QTest>

using Series = QList<QList<double>>;

class AsciiArtTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void parseNumbers()
    {
        QCOMPARE(AsciiArt::parseNumbers(QStringLiteral("1, 2.5;x -3\n4e1 nan")), QList<double>({1, 2.5, -3, 40}));
        QVERIFY(AsciiArt::parseNumbers(QStringLiteral("no numbers")).isEmpty());
    }

    void parseSeries()
    {
        // one number per line is a single series
        QCOMPARE(AsciiArt::parseSeries(QStringLiteral("1\n2\n3")), Series({{1, 2, 3}}));
        // several lines with two or more numbers each are separate series
        QCOMPARE(AsciiArt::parseSeries(QStringLiteral("cpu 1 2 3\nram 4 5\n")), Series({{1, 2, 3}, {4, 5}}));
        QVERIFY(AsciiArt::parseSeries(QStringLiteral("nothing here")).isEmpty());
    }

    // Expected outputs are the examples documented in asciichart's plot() docstring
    void plotMatchesAsciichart()
    {
        QCOMPARE(AsciiArt::plot({{10, 20, 30, 40, 50, 40, 30, 20, 10}}, 4),
                 QStringLiteral("   50.00  ┤   ╭╮\n"
                                "   40.00  ┤  ╭╯╰╮\n"
                                "   30.00  ┤ ╭╯  ╰╮\n"
                                "   20.00  ┤╭╯    ╰╮\n"
                                "   10.00  ┼╯      ╰"));
        QCOMPARE(AsciiArt::plot({{1, 2, 3, 4, 3, 2, 1}}, 3),
                 QStringLiteral("    4.00  ┤  ╭╮\n"
                                "    3.00  ┤ ╭╯╰╮\n"
                                "    2.00  ┤╭╯  ╰╮\n"
                                "    1.00  ┼╯    ╰"));
        QCOMPARE(AsciiArt::plot({{10, 20, 30, 40, 30, 20, 10}, {40, 30, 20, 10, 20, 30, 40}}, 3),
                 QStringLiteral("   40.00  ┤╮ ╭╮ ╭\n"
                                "   30.00  ┤╰╮╯╰╭╯\n"
                                "   20.00  ┤╭╰╮╭╯╮\n"
                                "   10.00  ┼╯ ╰╯ ╰"));
    }

    void plotEdgeCases()
    {
        QVERIFY(AsciiArt::plot({}, 5).isEmpty());
        QVERIFY(AsciiArt::plot({{}}, 5).isEmpty());
        QCOMPARE(AsciiArt::plot({{7}}, 5), QStringLiteral("    7.00  ┼"));
        QCOMPARE(AsciiArt::plot({{2, 2, 2}}, 5), QStringLiteral("    2.00  ┼──"));
    }

    void bannerFonts()
    {
        const QStringList fonts = AsciiArt::bannerFonts();
        QCOMPARE(fonts.size(), 10);
        QCOMPARE(fonts.first(), QStringLiteral("banner"));
        // every bundled font loads and renders something
        for (const QString &font : fonts) {
            QVERIFY2(AsciiArt::banner(QStringLiteral("Aa1"), font).contains(QLatin1Char('\n')), qPrintable(font));
        }
        QVERIFY(AsciiArt::banner(QStringLiteral("x"), QStringLiteral("no-such-font")).isEmpty());
    }

    // Expected outputs are what the ascii-art library itself prints, trailing spaces removed
    void bannerMatchesAsciiArt()
    {
        QCOMPARE(AsciiArt::banner(QStringLiteral("Hi"), QStringLiteral("banner")),
                 QStringLiteral("#     #\n"
                                "#     #   #\n"
                                "#     #\n"
                                "#######  ##\n"
                                "#     #   #\n"
                                "#     #   #\n"
                                "#     #  ###"));
        QCOMPARE(AsciiArt::banner(QStringLiteral("aB"), QStringLiteral("starwar")),
                 QStringLiteral("     ___       .______\n"
                                "    /   \\      |   _  \\\n"
                                "   /  ^  \\     |  |_)  |\n"
                                "  /  /_\\  \\    |   _  <\n"
                                " /  _____  \\   |  |_)  |\n"
                                "/__/     \\__\\  |______/"));
    }

    void bannerFallbacks()
    {
        // starwar has no lowercase 'b': it falls back to the uppercase glyph
        QCOMPARE(AsciiArt::banner(QStringLiteral("b"), QStringLiteral("starwar")), AsciiArt::banner(QStringLiteral("B"), QStringLiteral("starwar")));
        // two input lines -> two blocks separated by an empty line
        const QString two = AsciiArt::banner(QStringLiteral("I\nI"), QStringLiteral("sevenstar"));
        QCOMPARE(two.split(QLatin1Char('\n')).size(), 7 + 1 + 7);
        // characters with no glyph render blank
        QCOMPARE(AsciiArt::banner(QStringLiteral("?"), QStringLiteral("banner")), QString());
    }

    void table()
    {
        QCOMPARE(AsciiArt::table(QStringLiteral("name,qty\napple,3\nkiwi\n")),
                 QStringLiteral("╔═══════╤═════╗\n"
                                "║ name  │ qty ║\n"
                                "╠═══════╪═════╣\n"
                                "║ apple │ 3   ║\n"
                                "║ kiwi  │     ║\n"
                                "╚═══════╧═════╝"));
        QCOMPARE(AsciiArt::table(QStringLiteral("a\tb")),
                 QStringLiteral("╔═══╤═══╗\n"
                                "║ a │ b ║\n"
                                "╚═══╧═══╝"));
        QVERIFY(AsciiArt::table(QStringLiteral("\n  \n")).isEmpty());
    }

    void imageToAscii()
    {
        QImage image(8, 4, QImage::Format_RGB32);
        image.fill(Qt::black);
        for (int y = 0; y < 4; ++y) {
            for (int x = 4; x < 8; ++x) {
                image.setPixel(x, y, qRgb(255, 255, 255));
            }
        }
        // 8 columns of an 8x4 image -> 2 rows; left half dark (trimmed), right half brightest
        QCOMPARE(AsciiArt::imageToAscii(image, 8), QStringLiteral("    @@@@\n    @@@@"));
        QVERIFY(AsciiArt::imageToAscii(QImage(), 10).isEmpty());
    }

    void imageToBraille()
    {
        // 4 columns of a 4x2 image -> 1 row of 4 characters, each 2x4 dots
        QImage image(4, 2, QImage::Format_RGB32);
        image.fill(Qt::black);
        image.setPixel(2, 0, qRgb(255, 255, 255));
        image.setPixel(3, 0, qRgb(255, 255, 255));
        image.setPixel(2, 1, qRgb(255, 255, 255));
        image.setPixel(3, 1, qRgb(255, 255, 255));
        // dark half -> empty braille cells, bright half -> all 8 dots set
        QCOMPARE(AsciiArt::imageToBraille(image, 4), QStringLiteral("⠀⠀⣿⣿"));
        // a white image sets nothing when the threshold is above full brightness
        QImage white(4, 2, QImage::Format_RGB32);
        white.fill(Qt::white);
        QCOMPARE(AsciiArt::imageToBraille(white, 2, 256), QStringLiteral("⠀⠀"));
        QVERIFY(AsciiArt::imageToBraille(QImage(), 10).isEmpty());
    }
};

QTEST_GUILESS_MAIN(AsciiArtTest)

#include "asciiarttest.moc"
