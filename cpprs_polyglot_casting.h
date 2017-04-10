#pragma once

//in case you need to replace all XXX_CAST(t) to normal c++ xxx_cast<t> or c (t), you should replace `CONST_CAST(t,` to `const_cast<t>(` or `(t)(`

#ifdef __cplusplus


#define CONST_CAST(t,v) const_cast<t>(v)
#define IMPLICIT_STATIC_CAST(t,v) static_cast<t>(v)
#define EXPLICIT_STATIC_CAST(t,v) static_cast<t>(v)
#define REINTERPRET_CAST(t,v) reinterpret_cast<t>(v)

#else


#define CONST_CAST(t,v) (t)(v)
#define IMPLICIT_STATIC_CAST(t,v) (v)
#define EXPLICIT_STATIC_CAST(t,v) (t)(v)
#define REINTERPRET_CAST(t,v) (t)(v)


#endif



