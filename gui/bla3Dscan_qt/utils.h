#ifndef UTILS_H
#define UTILS_H

#include <QString>

namespace bla3Dscan {

    class Utils
    {
    public:
        Utils();

        static unsigned int translate_scan_precision( const QString& element );
    };
}

#endif // UTILS_H
