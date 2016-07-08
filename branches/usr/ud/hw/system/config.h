// EPOS Configuration Engine

#ifndef __config_unified_h
#define __config_unified_h

//============================================================================
// NAMESPACES AND DEFINITIONS
//============================================================================
namespace EPOS {
    namespace S {
        namespace U {}
        using namespace U;
    }
}

#define __BEGIN_API             namespace EPOS {
#define __END_API               }
#define _API                    ::EPOS

#define __BEGIN_UTIL            namespace EPOS { namespace S { namespace U {
#define __END_UTIL              }}}
#define __USING_UTIL            using namespace S::U;
#define _UTIL                   ::EPOS::S::U

#define __BEGIN_SYS             namespace EPOS { namespace S {
#define __END_SYS               }}
#define __USING_SYS             using namespace EPOS::S;
#define _SYS                    ::EPOS::S

#ifndef __mode_kernel__
namespace EPOS {
    using namespace S;
    using namespace S::U;
}
#endif

//============================================================================
// CONFIGURATION
//============================================================================
#include "../../sw/include/system/types.h"
#include <system/meta.h>
#include <system/traits.h>

#endif
