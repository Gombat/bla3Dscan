#include "utils.h"

namespace bla3Dscan {

    Utils::Utils()
    {
    }

    unsigned int Utils::translate_scan_precision( const QString& element )
    {
        if ( element.compare( "1/200", Qt::CaseInsensitive) == 0 )
             return 200;
        else if ( element.compare( "1/100", Qt::CaseInsensitive) == 0 )
            return 100;
        else if ( element.compare( "1/50", Qt::CaseInsensitive) == 0 )
            return 50;
        else if ( element.compare( "1/40", Qt::CaseInsensitive) == 0 )
            return 40;
		else if ( element.compare( "1/32", Qt::CaseInsensitive) == 0 )
            return 32;
        else if ( element.compare( "1/25", Qt::CaseInsensitive) == 0 )
            return 25;
        else if ( element.compare( "1/20", Qt::CaseInsensitive) == 0 )
            return 20;
        else if ( element.compare( "1/16", Qt::CaseInsensitive) == 0 )
            return 16;
		else if ( element.compare( "1/10", Qt::CaseInsensitive) == 0 )
            return 10;
        else if ( element.compare( "1/8", Qt::CaseInsensitive) == 0 )
            return 8;
        else if ( element.compare( "1/4", Qt::CaseInsensitive) == 0 )
            return 4;
        else
            return -1;

    }
} // namespace bla3Dscan
