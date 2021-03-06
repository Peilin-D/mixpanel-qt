#include <QString>
#include <QtTest>

#include "mixpanel.h"

class MixpanelTest : public QObject
{
    Q_OBJECT
    
public:
    MixpanelTest();

private:
    Mixpanel * mixpanel;
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void mixpanelTrackCase();
    void mixpanelTrackCase_data();
    void mixpanelEngageCase();
    void mixpanelEngageCase_data();
};

MixpanelTest::MixpanelTest()
{

}

void MixpanelTest::initTestCase()
{
    mixpanel = new Mixpanel("cbaf4f8d9d5c467e9eda8b4d9cf7df0d");
}

void MixpanelTest::cleanupTestCase()
{
    delete mixpanel;
}

void MixpanelTest::mixpanelTrackCase()
{
    QFETCH(QString, event);
    QFETCH(QVariantMap, properties);
    QFETCH(QString, distinct_id);
    QFETCH(bool, verbose);

    mixpanel->setVerbose(verbose);
    // mixpanel->setDistinct_id(distinct_id);

    QSignalSpy spySended(mixpanel, SIGNAL(sended()));
    QSignalSpy spyError(mixpanel, SIGNAL(error()));
    QVERIFY(mixpanel->track(distinct_id, event, properties));

    QTest::qWait(1000);

    QCOMPARE(spySended.count(), 1);
    QCOMPARE(spyError.count(), 0);
}


void MixpanelTest::mixpanelTrackCase_data()
{
    QTest::addColumn<QString>("event");
    QTest::addColumn<QVariantMap>("properties");
    QTest::addColumn<QString>("distinct_id");
    QTest::addColumn<bool>("verbose");

    QTest::newRow("Simple event 1 verbose") << QString("TrackCase 1") << QVariantMap() << QString() << true;
    QTest::newRow("Simple event 2 verbose") << QString("TrackCase 2") << QVariantMap() << QString() << true;
    QVariantMap test1;
    test1.insert("time", QDateTime().toTime_t());
    QTest::newRow("Event with properties 1 verbose") << QString("TrackCase with properties 1") << test1 << QString() << true;
    QVariantMap test2;
    test2.insert("time", QDateTime().toTime_t());
    test2.insert("mp_name_tag", "Stream Name");
    QTest::newRow("Event with properties 2 verbose") << QString("TrackCase with properties 2") << test2 << QString() << true;


    QTest::newRow("Simple event 1") << QString("TrackCase 1") << QVariantMap() << QString() << false;
    QTest::newRow("Simple event 2") << QString("TrackCase 2") << QVariantMap() << QString() << false;
    QTest::newRow("Event with properties 1") << QString("TrackCase with properties 1") << test1 << QString() << false;
    QTest::newRow("Event with properties 2") << QString("TrackCase with properties 2") << test2 << QString() << false;

    QTest::newRow("John event 1") << QString("Signup") << QVariantMap() << QString("john_doe_1234") << false;
    QTest::newRow("John event 2") << QString("Signin") << QVariantMap() << QString("john_doe_1234") << false;
    QTest::newRow("John event 3") << QString("Buy somethings") << QVariantMap() << QString("john_doe_1234") << false;
    QVariantMap test3;
    test3.insert("mp_name_tag", "Buy stream");
    QTest::newRow("John event with properties 1") << QString("Buy another things") << test3 << QString("john_doe_1234") << false;
    QVariantMap test4;
    test4.insert("time", QDateTime().toTime_t());
    test4.insert("mp_name_tag", "Buy stream");
    QTest::newRow("John event with properties 2") << QString("Buy yet another things") << test4 << QString("john_doe_1234") << false;

    QTest::newRow("Jane event 1") << QString("Signup") << QVariantMap() << QString("jane_doe_12") << false;
    QTest::newRow("Jane event 2") << QString("Signin") << QVariantMap() << QString("jane_doe_12") << false;
    QTest::newRow("Jane event 3") << QString("Buy somethings") << QVariantMap() << QString("jane_doe_12") << false;

    QTest::newRow("{d5b65c21-38c2-46b2-b89a-ec7801e4e632} event 1") << QString("Signup") << QVariantMap() << QString("{d5b65c21-38c2-46b2-b89a-ec7801e4e632}") << false;
    QTest::newRow("{d5b65c21-38c2-46b2-b89a-ec7801e4e632} event 2") << QString("Signin") << QVariantMap() << QString("{d5b65c21-38c2-46b2-b89a-ec7801e4e632}") << false;

    QVariantMap test5;
    test5.insert("During in second", QVariant::fromValue(29999l));
    QTest::newRow("{d5b65c21-38c2-46b2-b89a-ec7801e4e632} event 3 with long") << QString("Event") << test5 << QString("{d5b65c21-38c2-46b2-b89a-ec7801e4e632}") << false;
    // Doesn't work, QJson 0.7.1 apparently don't support long

}

void MixpanelTest::mixpanelEngageCase()
{
    QFETCH(QVariantMap, properties);
    QFETCH(QString, distinct_id);
    QFETCH(QString, operation);
    QFETCH(bool, verbose);

    mixpanel->setVerbose(verbose);
    // mixpanel->setDistinct_id(distinct_id);

    QSignalSpy spySended(mixpanel, SIGNAL(sended()));
    QSignalSpy spyError(mixpanel, SIGNAL(error()));
    QVERIFY(mixpanel->engage(distinct_id, operation, properties));

    QTest::qWait(1000);

    QCOMPARE(spySended.count(), 1);
    QCOMPARE(spyError.count(), 0);
}


void MixpanelTest::mixpanelEngageCase_data()
{
    QTest::addColumn<QVariantMap>("properties");
    QTest::addColumn<QString>("distinct_id");
    QTest::addColumn<QString>("operation");
    QTest::addColumn<bool>("verbose");

    QVariantMap test1;
    test1.insert("Gender", "Male");
    test1.insert("Age", 15);
    QTest::newRow("Set john") << test1 << QString("john_doe_1234") << Mixpanel::SET << true;
    QVariantMap test2;
    test2.insert("Gender", "Female");
    test2.insert("birthday", "1987-09-20");
    QTest::newRow("Set jane") << test2 << QString("jane_doe_12") << Mixpanel::SET << false;

    QVariantList test3;
    test3 << "Stuff" << "More stuff";
    QVariantMap test4;
    test4.insert("Items Buy",test3);
    QTest::newRow("Union john 1") << test4 << QString("john_doe_1234") << Mixpanel::UNION << true;

    QVariantList test5;
    test5 << "Stuff" << "More More stuff";
    QVariantMap test6;
    test6.insert("Items Buy",test5);
    QTest::newRow("Union john 2") << test6 << QString("john_doe_1234") << Mixpanel::UNION << true;

    QVariantMap test7;
    test7.insert("Total Buy",1);
    test7.insert("Total Amount",209);
    QTest::newRow("Add john 1") << test7 << QString("john_doe_1234") << Mixpanel::ADD << true;

    QVariantMap test8;
    test8.insert("Total Buy",2);
    test8.insert("Total Amount",75);
    QTest::newRow("Add john 2") << test8 << QString("john_doe_1234") << Mixpanel::ADD << true;

    QVariantMap test9;
    test9.insert("Total Buy",3);
    test9.insert("Total Amount",70);
    QTest::newRow("Add {d5b65c21-38c2-46b2-b89a-ec7801e4e632}") << test8 << QString("{d5b65c21-38c2-46b2-b89a-ec7801e4e632}") << Mixpanel::ADD << true;
}


QTEST_MAIN(MixpanelTest)

#include "tst_mixpaneltest.moc"
