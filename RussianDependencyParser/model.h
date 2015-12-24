#ifndef __MODEL_H__
#define __MODEL_H__

#include <QTextStream>
#include <QHash>
#include <QMultiHash>
#include <QtGlobal>
#include <QSet>

typedef QPair<QString, QString> StringPair;

class Model {
    public:
        Model(const QString &hashTableFilename, QTextStream &out);
        bool train(const QString &filename, QTextStream &out);
        double test(const QString &filename, QTextStream &out);
        void print(QTextStream &out);
        StringPair predict(const QString &prevTag, const QString &curWord);
        QList<StringPair> getTags(const QString &word, QList<ulong> &probs);
        bool save(const QString &filename, QTextStream &out);
        bool load(const QString &filename, QTextStream &out);

    private:
        QHash<StringPair, ulong> countTagsPair;
        QHash<QString, QSet<QString> > endsAndTags;
        QHash<StringPair, ulong> countEndsTags;
        QMultiHash<QString, StringPair> dict;
        ulong countWords;
};

#endif
