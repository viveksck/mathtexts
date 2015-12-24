#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QStringList>
#include <QMultiHash>
#include <QHash>
#include <QSet>
#include <QVector>

typedef QPair<QString, QString> StringPair;

int
main(int argc, char *argv[])
{

    //QMultiHash<QString, StringPair> hashTable;
    QHash<QString, QSet<QString> > predictTable;
    QHash<StringPair, ulong> countTable;
    QVector<QString> tagsV;
    QHash<ulong, QVector<StringPair> > hashTb;
    tagsV.clear();
    QFile fin(argv[1]);
    QTextStream out(stdout);
    QTextStream err(stderr);

    if (argc != 3) {
        out << "Usage: genhashtable dictfile.txt hashtablefile.txt" << endl;
        return 0;
    }

    if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        err << "ERROR: input file not found" << endl;
        return 1;
    }

    QTextStream sfin(&fin);
    sfin.setCodec("UTF-8");
    out.setCodec("UTF-8");
    QString line = sfin.readLine();
    QStringList lineParts;

    bool isfirst = false;

    QString form, normalForm, tags;
    while (!line.isNull()) {
        lineParts = line.split(QRegExp("[\t ]"), QString::SkipEmptyParts);
        if (isfirst) {
            if (!(lineParts.size() < 2 || lineParts[1].startsWith("VERB,") || lineParts[1].startsWith("PRTF,")
                    || lineParts[1].startsWith("PRTS,") || lineParts[1].startsWith("GRND,"))) {
                normalForm = lineParts[0];
            }
            isfirst = false;
        }

        if (lineParts.size() > 2 && lineParts[1].startsWith("INFN,")) {
            normalForm = lineParts[0];
        }

        if (lineParts.size() < 1) {
            line = sfin.readLine();
            continue;
        }

        if (lineParts.size() == 1) {
            isfirst = true;
            line = sfin.readLine();
            continue;
        }
        form = lineParts[0]; 
        QChar yo = QString::fromUtf8("Ё")[0];
        QChar ye = QString::fromUtf8("Е")[0];
        form.replace(yo, ye, Qt::CaseInsensitive);
        tags = lineParts[1];
        
        if (lineParts.size() == 3) {
            tags += ("," + lineParts[2]);
        }

        if (tagsV.indexOf(tags) == -1) {
            tagsV.append(tags);
        }
        hashTb[tagsV.indexOf(tags)].append(StringPair(normalForm, form));

        //hashTable.insert(form, StringPair(normalForm, tags));
        predictTable[form.right(3)].insert(tags);
        ++countTable[StringPair(form.right(3), tags)];

        line = sfin.readLine();
    }
    fin.close();

    //out << "Table size: " << hashTable.size() << endl;
    QString result("");
    for (int i = 0; i < tagsV.size(); ++i) {
        result += ("& " + tagsV[i] + " ");
        for (int j = 0; j < hashTb[i].size(); ++j) {
            result += (hashTb[i][j].first + " " + hashTb[i][j].second + " ");
        }
    }
    
    result += "\n----------";
    for (QHash<QString, QSet<QString> >::const_iterator itr = predictTable.begin(); itr != predictTable.end(); ++itr) {
        for (QSet<QString>::const_iterator jtr = itr.value().begin(); jtr != itr.value().end(); ++jtr) {
            result += (" " + itr.key() + " " + *jtr);
        }
    }
    result += "\n----------";
    for (QHash<StringPair, ulong>::const_iterator itr = countTable.begin(); itr != countTable.end(); ++itr) {
        result += (" " + itr.key().first + " " + itr.key().second + " " + QString::number(itr.value()));
    }
    result += "\n----------";


    QFile hashFile(argv[2]);
    hashFile.open(QIODevice::WriteOnly);
    QTextCodec *cp1251 = QTextCodec::codecForName("CP1251");
    hashFile.write(qCompress(cp1251->fromUnicode(result)));
    hashFile.flush();
    hashFile.close();

    return 0;
}
