/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT
*/
#include "asciiart.h"

#include <QImage>
#include <QObject>
#include <QTest>

class AsciiArtTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void parseNumbers()
    {
        QCOMPARE(AsciiArt::parseNumbers(QStringLiteral("1, 2.5;x -3\n4e1 nan")), QList<double>({1, 2.5, -3, 40}));
        QVERIFY(AsciiArt::parseNumbers(QStringLiteral("no numbers")).isEmpty());
    }

    // Expected outputs are the examples documented in asciichart's plot() docstring
    void plotMatchesAsciichart()
    {
        QCOMPARE(AsciiArt::plot({10, 20, 30, 40, 50, 40, 30, 20, 10}, 4),
                 QStringLiteral("   50.00  ┤   ╭╮\n"
                                "   40.00  ┤  ╭╯╰╮\n"
                                "   30.00  ┤ ╭╯  ╰╮\n"
                                "   20.00  ┤╭╯    ╰╮\n"
                                "   10.00  ┼╯      ╰"));
        QCOMPARE(AsciiArt::plot({1, 2, 3, 4, 3, 2, 1}, 3),
                 QStringLiteral("    4.00  ┤  ╭╮\n"
                                "    3.00  ┤ ╭╯╰╮\n"
                                "    2.00  ┤╭╯  ╰╮\n"
                                "    1.00  ┼╯    ╰"));
    }

    void plotEdgeCases()
    {
        QVERIFY(AsciiArt::plot({}, 5).isEmpty());
        QCOMPARE(AsciiArt::plot({7}, 5), QStringLiteral("    7.00  ┼"));
        QCOMPARE(AsciiArt::plot({2, 2, 2}, 5), QStringLiteral("    2.00  ┼──"));
    }

    void banner()
    {
        QCOMPARE(AsciiArt::banner(QStringLiteral("hi")),
                 QStringLiteral("#     #   ###\n"
                                "#     #    #\n"
                                "#     #    #\n"
                                "#######    #\n"
                                "#     #    #\n"
                                "#     #    #\n"
                                "#     #   ###"));
        // two input lines -> two 7-row blocks separated by an empty line
        QCOMPARE(AsciiArt::banner(QStringLiteral("a\nb")).split(QLatin1Char('\n')).size(), 15);
        QCOMPARE(AsciiArt::banner(QStringLiteral("?")), QStringLiteral("\n\n\n\n\n\n"));
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
};

QTEST_GUILESS_MAIN(AsciiArtTest)

#include "asciiarttest.moc"
