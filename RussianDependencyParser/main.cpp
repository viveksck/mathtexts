#include "model.h"
#include <QTextStream>
#include <QFile>
#include <QXmlStreamReader>
#include <QList>
#include <QStringList>
#include <QProcess>
#include <QDir>
#include <QApplication>

static char const* const MORPH_HASH_FILE = "hash.bin";

bool
printMorph(const QString& f1, const QString& f2, Model& m, QTextStream& out) {
    QFile xmlInput(f1);
    QFile outFile(f2);
    int j = 1;
    int s = 0;
    bool fSnt = true;

    if (!xmlInput.open(QIODevice::ReadOnly | QIODevice::Text)) {
        out << "Can't open input file";
        return false;
    }
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        out << "Can't open output file";
        return false;
    }
    QTextStream outF(&outFile);
    outF.setCodec("UTF-8");
    QXmlStreamReader xmlReader(&xmlInput);
    QXmlStreamReader::TokenType token;
    QString prevTag = "NONE";
    QString id;
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        token = xmlReader.readNext();
        if (token == QXmlStreamReader::StartDocument) {
            continue;
        }
        if (token == QXmlStreamReader::StartElement) {
            if (xmlReader.name() == "se") {
                j = 1;
                ++s;
                if (!fSnt) {
                    outF << endl;
                } else {
                    fSnt = false;
                }
                prevTag = "NONE";
                id = "0";
            }
            if (xmlReader.name() == "w") {
                id = xmlReader.attributes().value("id").toString();
                token = xmlReader.readNext();
                QString word = xmlReader.text().toString();
                if (!word[0].isPunct() && word[word.size() - 1].isPunct()) {
                    word.chop(1);
                }

                StringPair predicted = m.predict(prevTag, word);
                prevTag = predicted.second;
                token = xmlReader.readNext();
                if (xmlReader.name() != "rel") {
                    return false;
                }
                QString idHead = xmlReader.attributes().value("id_head").toString();
                QString type = xmlReader.attributes().value("type").toString();

                QStringList lineParts;
                lineParts = predicted.second.split(",");
                QString Pos = lineParts[0];
                QString tags = "";
                for (int i = 1; i < lineParts.size(); ++i) {
                    tags += lineParts[i];
                    tags += "|";
                }
                if (tags.size() == 0) {
                    tags = "_";
                } else {
                    tags.chop(1);
                }
                if (idHead.size() == 0) idHead = "0";
                if (type.size() == 0) type = "punct";
                outF << id << "\t"
                        << word << "\t"
                        << predicted.first << "\t"
                        << Pos << "\t"
                        << Pos << "\t"
                        << tags << "\t"
                        << idHead << "\t"
                        << type << "\t" << "_" << "\t" << "_" << endl;
                ++j;
            }
        }

    }
    outFile.close();
    return true;
}

void
printHelp() {
    QTextStream out(stdout);
    out << "Use:\n";
    out << "\t--morphtrain train.txt newMorphModel.txt\n";
    out << "\t--morphmark input.txt output.txt MorphModel.txt\n";
    out << "\t--synttrain input.txt MorphModel.txt SyntModel.txt Path/To/TurboParser\n";
    out << "\t--syntmark input.txt MorphModel.txt SyntModel.txt output.txt Path/To/TurboParser\n";
}

int
main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QDir dir = QDir::current();//(QCoreApplication::applicationDirPath());
    QTextStream out(stderr);
    out.setCodec("UTF-8");
    if (argc < 2) {
        printHelp();
        return 0;
    }
    if (QString(argv[1]) == "--morphtrain") {
        if (argc < 4) {
            printHelp();
            return 0;
        }
        Model m(MORPH_HASH_FILE, out);
        m.train(argv[2], out);
        m.save(argv[3], out);
        return 0;
    } else if (QString(argv[1]) == "--morphmark") {
        if (argc < 5) {
            printHelp();
            return 0;
        }
        Model m(MORPH_HASH_FILE, out);
        m.load(argv[4], out);
        if (!printMorph(argv[2], argv[3], m, out)) return -1;
    } else if (QString(argv[1]) == "--synttrain") {
        if (argc < 6) {
            printHelp();
            return 0;
        }
        Model m(MORPH_HASH_FILE, out);
        m.load(argv[3], out);
        if (!printMorph(argv[2], "tmpFile", m, out)) return -1;
        QProcess turboParser;
        turboParser.setProcessChannelMode(QProcess::MergedChannels);
        turboParser.start(argv[5],
                QStringList() << "--train" << "--file_train=" + dir.absoluteFilePath("tmpFile") << "--file_model=" + dir.absoluteFilePath(argv[4]));
        turboParser.waitForFinished(-1);
        QFile::remove("tmpFile");
    } else if (QString(argv[1]) == "--syntmark") {
        if (argc < 7) {
            printHelp();
            return 0;
        }
        out << "Loading model" << endl;
        Model m(MORPH_HASH_FILE, out);
        m.load(argv[3], out);
        out << "Generating tmp file" << endl;
        if (!printMorph(argv[2], "tmpFile", m, out)) return -1;
        out << "Running TurboParser" << endl;
        QProcess turboParser;
        turboParser.setProcessChannelMode(QProcess::MergedChannels);
        turboParser.start(argv[6],
                QStringList() << "--test" << "--evaluate" << "--file_test=" + dir.absoluteFilePath("tmpFile")
                        << "--file_model=" + dir.absoluteFilePath(argv[4]) << "--file_prediction=" + dir.absoluteFilePath(argv[5]));
        turboParser.waitForFinished(-1);
        QFile::remove("tmpFile");
    } else {
        printHelp();
    }
    return 0;

    /*
    */
}
