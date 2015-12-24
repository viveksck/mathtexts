#include "model.h"
#include <QString>
#include <QTextCodec>
#include <QByteArray>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QList>
#include <QHash>
#include <QDebug>

QTextStream err(stdout);

Model::Model(const QString& hashTableFilename, QTextStream& out) {
    countTagsPair.clear();
    dict.clear();
    endsAndTags.clear();
    countEndsTags.clear();

    QFile fin(hashTableFilename);
    if (!fin.open(QIODevice::ReadOnly)) {
        out << "ERROR: hash table file not found" << endl;
    }
    QByteArray rawInput = fin.readAll();
    rawInput = qUncompress(rawInput);


    QTextStream htFile(rawInput, QIODevice::ReadOnly);
    htFile.setCodec("CP1251");

    QString tmp1, tmp2, tmp3;
    ulong longtmp;
    QString curTags;

    while (true) {
        htFile >> tmp1;
        if (tmp1 == "&") {
            htFile >> curTags;
            continue;
        }
        if (tmp1 == "----------") {
            break;
        }
        htFile >> tmp2;
        dict.insert(tmp2, StringPair(tmp1, curTags));
    }
    while (true) {
        htFile >> tmp1;
        if (tmp1 == "----------") {
            break;
        }
        htFile >> tmp2;
        endsAndTags[tmp1].insert(tmp2);
    }
    while (true) {
        htFile >> tmp1;
        if (tmp1 == "----------") {
            break;
        }
        htFile >> tmp2 >> longtmp;
        countEndsTags[StringPair(tmp1, tmp2)] = longtmp;
    }

    fin.close();
}

bool
Model::save(const QString& filename, QTextStream& out) {
    QFile fout(filename);
    if (!fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
        out << "ERROR: cannot open model file" << endl;
        return false;
    }
    QTextStream sfout(&fout);
    sfout.setCodec("CP1251");

    for (QHash<StringPair, ulong>::const_iterator itr = countTagsPair.begin(); itr != countTagsPair.end(); ++itr) {
        sfout << itr.key().first << " " << itr.key().second << " " << itr.value() << " ";
    }
    sfout << "----------";
    fout.flush();
    fout.close();
    return true;
}

bool
Model::load(const QString& filename, QTextStream& out) {
    QFile fin(filename);
    if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        out << "ERROR: model file not found" << endl;
        return false;
    }
    QTextStream sfin(&fin);
    sfin.setCodec("CP1251");

    countTagsPair.clear();
    QString tmp1, tmp2;
    int tmp3;
    while (true) {
        sfin >> tmp1;
        if (tmp1 == "----------") {
            break;
        }
        sfin >> tmp2 >> tmp3;
        countTagsPair.insert(StringPair(tmp1, tmp2), tmp3);
    }
    fin.close();
    return true;
}

bool
Model::train(const QString& filename, QTextStream& out) {
    QFile fin(filename);

    if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        out << "ERROR: input file not found" << endl;
        return false;
    }

    QString prevTag = "NONE";

    QTextStream sfin(&fin);
    sfin.setCodec("UTF-8");
    out.setCodec("UTF-8");
    while (!sfin.atEnd()) {
        QString line = sfin.readLine();
        if (line == "----------") {
            prevTag = "NONE";
            continue;
        }

        QStringList words = line.split(" ");
        ++countWords;
        ++countTagsPair[StringPair(prevTag, words[2])];
        prevTag = words[2];
    }
    return true;
}

double
Model::test(const QString& filename, QTextStream& out) {
    QFile fin(filename);

    if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        out << "ERROR: input file not found" << endl;
        return false;
    }

    QString curTag, prevTag = "NONE";
    ulong countAll = 0, countWrong = 0;

    QTextStream sfin(&fin);
    sfin.setCodec("UTF-8");
    out.setCodec("UTF-8");
    while (!sfin.atEnd()) {
        QString line = sfin.readLine();
        if (line == "----------") {
            prevTag = "NONE";
            continue;
        }

        QStringList words = line.split(" ");
        curTag = predict(prevTag, words[0]).second;
        if (curTag != "PNKT" && curTag != "NUMB" && curTag != "LATN" && curTag != "UNKN") {
            if (words[2] == "UNKN")
                continue;
            ++countAll;
            if (words[2] != "UNKN" && words[2] != curTag) {
                out << words[0] << " : " << words[2] << " != " << curTag << endl;
                ++countWrong;
            }
        }
        prevTag = curTag;
    }
    out << (countAll - countWrong) / (1. * countAll) << endl;
    return true;
}

void
Model::print(QTextStream& out) {
    out.setCodec("UTF-8");
    QHash<StringPair, ulong>::const_iterator i;
    QHash<QString, ulong> count;
    for (i = countTagsPair.begin(); i != countTagsPair.end(); ++i) {
        out << i.key().first << " before " << i.key().second << " : " << i.value() << endl;
        count[i.key().first] += i.value();
    }

    QHash<QString, ulong>::const_iterator j;
    for (j = count.begin(); j != count.end(); ++j) {
        out << j.key() << " " << j.value() << endl;
    }
}

StringPair
Model::predict(const QString& prevTag, const QString& curWord) {
    QList<ulong> probs;
    QList<StringPair> variants = getTags(curWord, probs);
    uint maxVariant = 0;
    if (variants[maxVariant].second == "PNKT" || variants[maxVariant].second == "NUMB" ||
            variants[maxVariant].second == "LATN" || variants[maxVariant].second == "UNKN") {
        return variants[maxVariant];
    }
    Q_ASSERT(probs.size() == variants.size());
    for (int i = 0; i < variants.size(); ++i) {
        if (probs[i] * countTagsPair[StringPair(prevTag, variants[i].second)] > probs[maxVariant] * countTagsPair[StringPair(prevTag, variants[maxVariant].second)]) {
            maxVariant = i;
        }
    }
    return variants[maxVariant];
}

QList<StringPair>
Model::getTags(const QString& word, QList<ulong>& probs) {
    err.setCodec("UTF-8");
    QList<StringPair> result;
    result.clear();
    probs.clear();
    QChar yo = QString::fromUtf8("Ё")[0];
    QChar ye = QString::fromUtf8("Е")[0];
    result = dict.values(word.toUpper().replace(yo, ye, Qt::CaseInsensitive));
    if (result.size() > 0) {
        for (QList<StringPair>::iterator i = result.begin(); i != result.end(); ++i) {
            probs.append(1);
        }
        return result;
    }
    if (word[0].isPunct()) {
        result.append(StringPair(word, "PNCT"));
        probs.append(1);
    } else if (word[0].isNumber()) {
        result.append(StringPair(word, "NUMB"));
        probs.append(1);
    } else if (word[0].toLatin1() != 0) {
        result.append(StringPair(word, "LATN"));
        probs.append(1);
    } else {
        QSet<QString> predictedTags = endsAndTags[word.toUpper().right(3)];
        if (predictedTags.size() == 0) {
            result.append(StringPair(word, "UNKN"));
            probs.append(1);
        }
        for (QSet<QString>::const_iterator itr = predictedTags.begin(); itr != predictedTags.end(); ++itr) {
            result.append(StringPair(word, *itr));
            probs.append(countEndsTags[StringPair(word.toUpper().right(3), *itr)]);
        }
    }
    return result;
}
