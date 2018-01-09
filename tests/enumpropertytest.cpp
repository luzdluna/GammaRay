/*
  enumpropertytest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <core/enumutil.h>
#include <core/enumrepositoryserver.h>

#include <QtTest/qtest.h>
#include <QFrame>
#include <QObject>

Q_DECLARE_METATYPE(const QMetaObject *)
Q_DECLARE_METATYPE(QSizePolicy::ControlTypes)

using namespace GammaRay;

class EnumHolder
{
public:
    enum MyEnum {
        Value0 = 0,
        Value1 = 1,
        Value2 = 2,
        Value3 = 4
    };
    Q_DECLARE_FLAGS(MyFlags, MyEnum)
};

Q_DECLARE_METATYPE(EnumHolder::MyEnum)
Q_DECLARE_METATYPE(EnumHolder::MyFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(EnumHolder::MyFlags)

static const MetaEnum::Value<EnumHolder::MyEnum> my_enum_table[] = {
    { EnumHolder::Value0, "Value0" },
    { EnumHolder::Value1, "Value1" },
    { EnumHolder::Value2, "Value2" }
    // Value3 intentionally missing
};

class EnumPropertyTest : public QObject
{
    Q_OBJECT
public:
    explicit EnumPropertyTest(QObject *parent = nullptr) :
        QObject(parent)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        qRegisterMetaType<QFrame*>();
        qRegisterMetaType<QFrame::Shadow>();
#endif

        EnumRepositoryServer::create(this);
        ER_REGISTER_ENUM(EnumHolder, MyEnum, my_enum_table);
        ER_REGISTER_FLAGS(EnumHolder, MyFlags, my_enum_table);
    }

private slots:
    void testEnumToString_data()
    {
        QTest::addColumn<QVariant>("variant", nullptr);
        QTest::addColumn<QByteArray>("name", nullptr);
        QTest::addColumn<const QMetaObject*>("mo", nullptr);
        QTest::addColumn<QString>("result", nullptr);

        const QMetaObject *nullObj = nullptr;
        QTest::newRow("null") << QVariant() << QByteArray() << nullObj << QString();
        QTest::newRow("int") << QVariant(42) << QByteArray() << &staticMetaObject << QString();

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        // global enum
        QTest::newRow("global enum as int, QMO/name") << QVariant::fromValue<int>(Qt::LeftToRight) << QByteArray("Qt::LayoutDirection") << &QFrame::staticMetaObject << QStringLiteral("LeftToRight");
        QTest::newRow("global enum as int, name") << QVariant::fromValue<int>(Qt::LeftToRight) << QByteArray("Qt::LayoutDirection") << nullObj << QStringLiteral("LeftToRight");
        QTest::newRow("global enum, QMO/name") << QVariant::fromValue<Qt::LayoutDirection>(Qt::LeftToRight) << QByteArray("Qt::LayoutDirection") << &QFrame::staticMetaObject << QStringLiteral("LeftToRight");
        QTest::newRow("global enum, QMO") << QVariant::fromValue<Qt::LayoutDirection>(Qt::LeftToRight) << QByteArray() << &QFrame::staticMetaObject << QStringLiteral("LeftToRight");
        QTest::newRow("global enum, name") << QVariant::fromValue<Qt::LayoutDirection>(Qt::LeftToRight) << QByteArray("Qt::LayoutDirection") << nullObj << QStringLiteral("LeftToRight");
        QTest::newRow("global enum") << QVariant::fromValue<Qt::LayoutDirection>(Qt::LeftToRight) << QByteArray() << nullObj << QStringLiteral("LeftToRight");

        // global flag
        QTest::newRow("global flag as int, name") << QVariant::fromValue<int>(Qt::AlignHCenter|Qt::AlignTop) << QByteArray("Qt::Alignment") << nullObj << QStringLiteral("AlignHCenter|AlignTop");
        QTest::newRow("global flag, name") << QVariant::fromValue<Qt::Alignment>(Qt::AlignHCenter|Qt::AlignTop) << QByteArray("Qt::Alignment") << nullObj << QStringLiteral("AlignHCenter|AlignTop");
        QTest::newRow("global flag") << QVariant::fromValue<Qt::Alignment>(Qt::AlignHCenter|Qt::AlignTop) << QByteArray() << nullObj << QStringLiteral("AlignHCenter|AlignTop");

        // object-local enum
        QTest::newRow("local enum as int, QMO/name") << QVariant::fromValue<int>(QFrame::Sunken) << QByteArray("QFrame::Shadow") << &QFrame::staticMetaObject << QStringLiteral("Sunken");
        QTest::newRow("local enum as int, name, registered") << QVariant::fromValue<int>(QFrame::Sunken) << QByteArray("QFrame::Shadow") << nullObj << QStringLiteral("Sunken");
        QTest::newRow("local enum as int, name, parent registered") << QVariant::fromValue<int>(QFrame::Box) << QByteArray("QFrame::Shape") << nullObj << QStringLiteral("Box");
        QTest::newRow("local enum, QMO/name") << QVariant::fromValue<QFrame::Shadow>(QFrame::Sunken) << QByteArray("QFrame::Shadow") << &QFrame::staticMetaObject << QStringLiteral("Sunken");
        QTest::newRow("local enum, QMO") << QVariant::fromValue<QFrame::Shadow>(QFrame::Sunken) << QByteArray() << &QFrame::staticMetaObject << QStringLiteral("Sunken");
        QTest::newRow("local enum, name") << QVariant::fromValue<QFrame::Shadow>(QFrame::Sunken) << QByteArray("QFrame::Shadow") << nullObj << QStringLiteral("Sunken");
        QTest::newRow("local enum") << QVariant::fromValue<QFrame::Shadow>(QFrame::Sunken) << QByteArray() << nullObj << QStringLiteral("Sunken");

        // gadget-local enum
        QTest::newRow("gadget enum as int, QMO/name") << QVariant::fromValue<int>(QSizePolicy::Maximum) << QByteArray("QSizePolicy::Policy") << &QSizePolicy::staticMetaObject << QStringLiteral("Maximum");
        QTest::newRow("gadget enum as int, name") << QVariant::fromValue<int>(QSizePolicy::Maximum) << QByteArray("QSizePolicy::Policy") << nullObj << QStringLiteral("Maximum");
        QTest::newRow("gadget enum, QMO/name") << QVariant::fromValue<QSizePolicy::Policy>(QSizePolicy::Maximum) << QByteArray("QSizePolicy::Policy") << &QSizePolicy::staticMetaObject << QStringLiteral("Maximum");
        QTest::newRow("gadget enum, QMO") << QVariant::fromValue<QSizePolicy::Policy>(QSizePolicy::Maximum) << QByteArray() << &QSizePolicy::staticMetaObject << QStringLiteral("Maximum");
        QTest::newRow("gadget enum, name") << QVariant::fromValue<QSizePolicy::Policy>(QSizePolicy::Maximum) << QByteArray("QSizePolicy::Policy") << nullObj << QStringLiteral("Maximum");
        QTest::newRow("gadget enum") << QVariant::fromValue<QSizePolicy::Policy>(QSizePolicy::Maximum) << QByteArray() << nullObj << QStringLiteral("Maximum");

        // gadget-local flag
        QTest::newRow("gadget flag as int, QMO/name") << QVariant::fromValue<int>(QSizePolicy::Frame|QSizePolicy::Label) << QByteArray("QSizePolicy::ControlTypes") << &QSizePolicy::staticMetaObject << QStringLiteral("Frame|Label");
        QTest::newRow("gadget flag as int, name") << QVariant::fromValue<int>(QSizePolicy::Frame|QSizePolicy::Label) << QByteArray("QSizePolicy::ControlTypes") << nullObj << QStringLiteral("Frame|Label");
        QTest::newRow("gadget flag, QMO/name") << QVariant::fromValue<QSizePolicy::ControlTypes>(QSizePolicy::Frame|QSizePolicy::Label) << QByteArray("QSizePolicy::ControlTypes") << &QSizePolicy::staticMetaObject << QStringLiteral("Frame|Label");
        QTest::newRow("gadget flag, QMO") << QVariant::fromValue<QSizePolicy::ControlTypes>(QSizePolicy::Frame|QSizePolicy::Label) << QByteArray() << &QSizePolicy::staticMetaObject << QStringLiteral("Frame|Label");
        QTest::newRow("gadget flag, name") << QVariant::fromValue<QSizePolicy::ControlTypes>(QSizePolicy::Frame|QSizePolicy::Label) << QByteArray("QSizePolicy::ControlTypes") << nullObj << QStringLiteral("Frame|Label");
        QTest::newRow("gadget flag") << QVariant::fromValue<QSizePolicy::ControlTypes>(QSizePolicy::Frame|QSizePolicy::Label) << QByteArray() << nullObj << QStringLiteral("Frame|Label");

        // non-Qt enum
        QTest::newRow("plain enum, in map") << QVariant::fromValue(EnumHolder::Value2) << QByteArray() << nullObj << QStringLiteral("Value2");
        QTest::newRow("plain enum, not in map") << QVariant::fromValue(EnumHolder::Value3) << QByteArray() << nullObj << QStringLiteral("unknown (4)");

        // non-Qt flags
        QTest::newRow("plain flag, single, in map") << QVariant::fromValue<EnumHolder::MyFlags>(EnumHolder::Value2) << QByteArray() << nullObj << QStringLiteral("Value2");
        QTest::newRow("plain flag, double, in map") << QVariant::fromValue<EnumHolder::MyFlags>(EnumHolder::Value2 | EnumHolder::Value1) << QByteArray() << nullObj << QStringLiteral("Value1|Value2");
        QTest::newRow("plain flag, single, not in map") << QVariant::fromValue<EnumHolder::MyFlags>(EnumHolder::Value3) << QByteArray() << nullObj << QStringLiteral("flag 0x4");
        QTest::newRow("plain flag, double, mixed") << QVariant::fromValue<EnumHolder::MyFlags>(EnumHolder::Value2|EnumHolder::Value3) << QByteArray() << nullObj << QStringLiteral("Value2|flag 0x4");
        QTest::newRow("plain flag, empty") << QVariant::fromValue(EnumHolder::MyFlags()) << QByteArray() << nullObj << QStringLiteral("Value0");
#endif
    }

    void testEnumToString()
    {
        QFETCH(QVariant, variant);
        QFETCH(QByteArray, name);
        QFETCH(const QMetaObject *, mo);
        QFETCH(QString, result);

        const auto str = EnumUtil::enumToString(variant, name, mo);
        QCOMPARE(str, result);
    }

};

QTEST_MAIN(EnumPropertyTest)

#include "enumpropertytest.moc"
