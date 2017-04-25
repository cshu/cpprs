#pragma once

//note there are complicated programs that you MAY NOT want to implement with pure macros. E.g. you have a macro with lblXXX parameters. And it needs to be called from another macro, which needs to run different logic for each lblXXX case. Yet you MAY NOT want to write any label in macro because it pollutes namespace!
//note you CAN use label in macro! But even if you make the label name very long like xxx_MACRO_NAME, that still means you CANNOT use this macro twice within a function! bc "A label name is the only kind of identifier that has function scope".
//note one solution is to uip (unique identifier prefix, for labels, variable, etc.)
//you can manually write different uip, or you can use __COUNTER__. (it's not standard ISO C, but it should be fine, just like #pragma once. Even if someday you use a compiler that doesn't support __COUNTER__, you can still "run your code through a preprocessor that does support it before feeding it into the compiler", or even write a small program yourself, to search all source code and replace __COUNTER__ with integers!)
/*//note __LINE__ MAY not be very useful, multiple lines with __LINE__ passed as macro arg still give the same LINE number!
e.g.
A_MACRO({
MMMs(__COUNTER__)//if __LINE__ is used here, then 2 values are equal. if __COUNTER__ is used here, then they are unequal
NNNs(__COUNTER__)
})
*/

//note -Wshadow warning should always be turned on because it's likely to help you find bugs in deeply nested macros! e.g. you can pass {free(xx); {caseFail}} to another macro as argument also named "caseFailed", but it's possible that xx's identifier is used again in the deeper nested macros. So the free(xx); actually frees the wrong pointer! Even if xx is a very long and complicated identifier, it's still a bug when the same macro is nested!
//note unique identifier prefix can prevent it from happening, so always use uip.
//note there are 2 other benefit of uip: 1. GOTO labels within macro 2. you can print the uip using ## when error is encoutered, this way it's almost like you are printing the stack trace if your uip is semantic!
//note you can still use very long and complicated identifier in macros which don't take caseXX arguments. (there should be no chance to shadow)
//or you can use UI_IN_LIMITED_SCOPE instead (still there should be no chance to shadow)
//note it's preferred to use caseXXX parameter only once (i.e. expand only once). Bc when you have {... ui##label: ...} passed as a caseXXX argument, it causes error for multiple expansions. (still it's not mandatory because you can use goto to jump out instead of embedding lots of code in caseXXX arg)
//-Wshadow helps when you accidentally write something expanding to int x=1;{int x=x;}. The inner x is not copying the outer x. This is similar to javascript `var x=1;(()=>{var x=x;})();`. Think of it as something like hoisting. (declaration always happens before assignment. in c++ it's scope hoisting rather than func hoisting, though. in contrast, `x:=1;{x:=x;}` works in go.

#ifdef __cplusplus//be careful. only use this macro when the function always works in the same way.
#define CPPRS_COMMON_SP inline
#else
#define CPPRS_COMMON_SP static
#endif

#define NOOP_WITHOUT_COMPILER_WARNING ((void)0)//note you cast any variable/return value to (void) to tell compiler to ignore your unused parameter/return value
#define NO_OPERATION(...) __VA_ARGS__
#define EXPAND_TO_NOTHING(...)
#define EXPAND_TO_FUNC_N_ARGS(f, ... ) f(__VA_ARGS__)
#define UI_IN_LIMITED_SCOPE ui_in_limited_scope
#define UI_IN_LIMITED_SCOPE2 ui_in_limited_scope2
#define UI_IN_LIMITED_SCOPE3 ui_in_limited_scope3
#define UI_IN_LIMITED_SCOPE4 ui_in_limited_scope4
#define TOKEN_PASTING_2(x,y) x ## y//can be used to concat L and "str literal"
#define X_TOKEN_PASTING_2(x,y) TOKEN_PASTING_2(x,y)
#define TOKEN_PASTING_3(x,y,z) x ## y ## z
#define X_TOKEN_PASTING_3(x,y,z) TOKEN_PASTING_3(x,y,z)
#define TOKEN_PASTING_2_N_STRINGIZING(x,y) XSTR(TOKEN_PASTING_2(x,y))
#define HEX_AS_LITERAL(x) TOKEN_PASTING_2_N_STRINGIZING(\x,x)
#define HEX_AS_INTEGER(x) X_TOKEN_PASTING_2(0x,x)

//note you don't have to use this macro, manually written UIP can be better (semantic). For each layer, you append _XXX to the UIP so the prefix becomes longer and longer. The 1st layer is at the main() level. For each layer, you manage manually written UIP as if managing a namespace. e.g. in main() you have AAA(... , uipAaa_), and you #define AAA(... , uip) { BBB(... , uip ## bbb_)); CCC(... , uip ## ccc_); }
#define IDENTIFIER_PREFIX_USING_COUNTER X_TOKEN_PASTING_3(identifierPrefixUsingCounter, __COUNTER__, _)
#define IDENTIFIER_PREFIX_USING_LINE X_TOKEN_PASTING_3(identifierPrefixUsingLine, __LINE__, _)

//note you can use setjmp/longjmp when macro is inconvenient, but C++ zero cost exception try block has less overhead than setjmp (note C++ zero cost exception is not always superb, e.g. when you have 2 common cases to handle, so neither of them are rare.)

#define SETLINE_GOTO(line,label) {line=__LINE__;  goto label;}
//note macro ending with s means stop/sequence point? doesn't need to add semicolon after it, and probably cannot be assigned as an rvalue.
//note ending with u means ui is the 1st parameter. ending with d means it's a declaration.
//note caseXXX arguments in macro are treated equally. For all cases, it's okay to leave it blank, execution will leave the macro's scope immediately when a case is encountered and processed (in other words, you CANNOT allow any logic to be executed after a case is run, only `break`/`continue`/`goto` are allowed)
//note lblXXX arguments in macro are like caseXXX. But the arg is simply a label for simply a goto. This can be more convenient because you can leave this macro's scope, which may contains local declared variables.
//note setline_lblXXX arguments are like lblXXX. But the macro's first argument must be a integer for holding __LINE__. And SETLINE_GOTO is used.

//note you use very long temp variable name like tempRetVal_MACRO_NAME within a macro. This is for avoiding possible name conflict with outer scopes!
//note for macros containing temp variables, if you substitue a caseXXX arg with code calling that macro again (i.e. recursively), you get warning for -Wshadow. So using goto is recommended. (No need to worry too much, when you get warning you can change the logic to goto)

#if defined(NEED_TO_AVERT_RACES) == defined(NO_NEED_TO_AVERT_RACES)
#error "Must define NEED_TO_AVERT_RACES or NO_NEED_TO_AVERT_RACES"
#endif
//note c standard functions that "are not required to avoid data races" should not be used in your macros and functions. Unless the macro (or function) name EXPLICITLY reveals danger! (e.g. contains the c function name OR use XXX_DATA_RACES_POSSIBLE)

//global_tmp variables should NOT be used, unless you want ad-hoc simple code snippets.
//static void *global_tmp_pvoid_buf;//only for single-thread and really temporary usage (no funciton call/macro in-between, which might also use global variables inside)
//static int global_tmp_int_buf;
//static INT_FOR_INT_SSIZE_T global_tmp_int_for_int_ssize_t_buf;

#define VALID_PH_PTR ""//there are cases in which you just need a placeholder pointer (with object size of 0), but nullptr is not valid to use
template<class T> T ret_arg_or_valid_ph_ptr(T ptr){
	if(ptr)return ptr;
	return VALID_PH_PTR;
}
#define EQ_NULL(ptr) (ptr==NULL)//note (!ptr) should be enough, but on some old embedded platforms NULL is not 0. (Though the lastest C standard mandates it must equal 0)
#define SAFEST_TYPE_TO_READ_UNINITIALIZED_MEM unsigned char//"EXP33-EX1: Reading uninitialized memory by an lvalue of type unsigned char does not trigger undefined behavior. The unsigned char type is defined to not have a trap representation (see the C Standard, 6.2.6.1, paragraph 3), which allows for moving bytes without knowing if they are initialized. However, on some architectures, such as the Intel Itanium, registers have a bit to indicate whether or not they have been initialized. The C Standard, 6.3.2.1, paragraph 2, allows such implementations to cause a trap for an object that never had its address taken and is stored in a register if such an object is referred to in any way."

//note type naming should not use _t as suffix bc posix regard that as reserved names

#ifdef _WIN32
#define CPPRS_LINK_WITH_C __declspec( dllexport )
#else
#define CPPRS_LINK_WITH_C
#endif

#define XSTR(s) STR(s)
#define STR(s) #s
#define COMMA ,
//you can use this when define functions so you can make the change to compile everything as extern when you have to
//note you probably don't want to #define static (exactly the keyword), bc this affects everything, including in-function static variable!
#define STATIC static
#define MAX_OF_TWO(x,y) (((x) > (y)) ? (x) : (y))
#define MIN_OF_TWO(x,y) (((x) < (y)) ? (x) : (y))
#define GET_BIT_AT_INDEX(integertypval,ind) ( (integertypval) & (1 << (ind)) )
#define GET_LEFTMOST_BIT_OF_CHAR(chr) ( ((unsigned char)(chr)) >> (CHAR_BIT-1) )
#define GET_LEFTMOST_BIT_OF_OBJ(pvoid) ( (*(unsigned char *)pvoid) >> (CHAR_BIT-1) )
//when width is 0, size becomes 1
#define DEFINE_BIT_ARRAYd(width,id) unsigned char id[(((width)-1)/8)+1];//? use CHAR_BIT bc UCHAR_MAX can be greater than 255?
#define GET_BIT_FROM_OBJ(ind,obj) ( (((unsigned char *)obj)[(ind)/8]>>(ind)%8) % 2 )
#define SET_BIT_IN_OBJ(ind,obj) //undone

#include <cstring>

//"Where an argument declared as size_t n specifies the length of the array for a function, n can have the value zero on a call to that function. Unless explicitly stated otherwise in the description of a particular function in this subclause, pointer arguments on such a call shall still have valid values, as described in 7.1.4"
//so checking len is only useful when pointers can be NULL
#define MEMCPY_IF_LEN(d,s,n) ( (n) ? memcpy(d,s,n) : (d) )
#define MEMCMP_IF_LEN(x,y,n) ( (n) ? memcmp(x,y,n) : 0 )
CPPRS_COMMON_SP int memcmp_if_len(const void* x, const void* y, size_t count){
	return MEMCMP_IF_LEN(x,y,count);
}
#define MEMMOVE_IF_LEN(o,d,n) ( (n) ? memmove(o,d,n) : (o) )

#define MEMCHR_FROM_OFFSET(s,c,len,off) ( memchr( (s)+(off),(c),(len)-(off) ) )
#define MEMMOVE_IF_DIFFERENT(obj,d,objlen) ((obj)==(d) ? (obj) : memmove(obj,d,objlen))
#define MEMMOVE_IF_DIFFERENT_N_LEN(obj,d,objlen) ((obj)==(d)||!(objlen) ? (obj) : memmove(obj,d,objlen))
#define MEMMOVE_WITHIN_OBJ_TO_DISCARD_HEADs(obj,d,objlen) {objlen-=(d)-(obj); memmove(obj,d,objlen);}
#define MEMCPY_N_SET_DESTs(s1,s2,n) {memcpy(s1,s2,n);s1+=n;}
#define MEMCPY_N_SET_BOTH_PTRSs(s1,s2,n) {memcpy(s1,s2,n);s1+=n;s2+=n;}

//note this is not thread safe
#define STRFTIME_YmdHMS_GMTIME_NOWs(buf) {time_t tmp_STRFTIME_YmdHMS_GMTIME_NOWs = time(NULL); strftime(buf,15,"%Y%m%d%H%M%S",gmtime(&tmp_STRFTIME_YmdHMS_GMTIME_NOWs)); }
#define STRFTIME_YmdHMS_GMTIME_NOWd(id) char id[15]; STRFTIME_YmdHMS_GMTIME_NOWs(id)
//todo write a thread safe version, if NEED_TO_AVERT_RACES is defined then enter mutex?

#define DIFFTIMENOW0 difftime(time(NULL),(time_t)0)

#define NUM_OF_MEMB_IN_ARR(arr) ((sizeof arr) / (sizeof *arr))
#define ARR_COMMA_NUM_OF_MEMB(arr) arr, NUM_OF_MEMB_IN_ARR(arr)
#define LEN_OF_WCHAR_T_LITERAL(lit) (sizeof lit-sizeof(wchar_t))
#define LITERAL_COMMA_SIZE(lit) lit,(sizeof lit)
#define LITERAL_COMMA_LEN(lit) lit,(sizeof lit-1)
#define LITERAL_WCHAR_T_COMMA_LEN(lit) lit,LEN_OF_WCHAR_T_LITERAL(lit)

#define LITERAL_255_NUL "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
#define PLITERAL_7_NUL LITERAL_255_NUL//note you don't really need these PLITERAL_XXX, but you may need shorter substitution in the future
#define PLITERAL_15_NUL LITERAL_255_NUL
#define PLITERAL_31_NUL LITERAL_255_NUL
#define PLITERAL_63_NUL LITERAL_255_NUL
#define PLITERAL_127_NUL LITERAL_255_NUL
#define LITERAL_0123456789 "0123456789"
#define LITERAL_ABCDEFGHIJKLMNOPQRSTUVWXYZ "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LITERAL_abcdefghijklmnopqrstuvwxyz "abcdefghijklmnopqrstuvwxyz"
#define LITERAL_AtoZ_atoz LITERAL_ABCDEFGHIJKLMNOPQRSTUVWXYZ LITERAL_abcdefghijklmnopqrstuvwxyz
#define LITERAL_0to9_AtoZ_atoz LITERAL_0123456789 LITERAL_AtoZ_atoz

#define LITERAL_LOCALHOSTINBE "\x7F\0\0\1"
#define HTTP_1_1_ENDOFLINE_MARKER "\r\n"

#ifdef __cplusplus
#define PUTS_DATE_TIME_STDC_VERSION_CPLUSPLUSs puts(__DATE__ " " __TIME__ " __cplusplus: " XSTR(__cplusplus));
#else
#define PUTS_DATE_TIME_STDC_VERSION_CPLUSPLUSs puts(__DATE__ " " __TIME__ " __STDC_VERSION__: " XSTR(__STDC_VERSION__));
#endif

#ifdef __cplusplus
#define INT_OF_LITERAL(c) ((int)(c))
#define CHAR_OF_LITERAL(c) (c)//note 'unicode/multiple char(s)' may be of int type, and/or negative, in this case, nothing is done
#else
#define INT_OF_LITERAL(c) (c)
CPPRS_COMMON_SP char reinterpret_unsigned_char_as_char(unsigned char uc){ return *(char *) &uc; }
#define CHAR_OF_LITERAL(c) (reinterpret_unsigned_char_as_char(c))//possible implementation-defined behavior/signal for simple cast like (char)c?
#endif

//normally you don't use this macro, you should always specify a type you need, e.g. uint_fast8_t, uint_fast64_t
#define DEFAULT_FUNCTION_RET_TYPE int//or maybe unsigned?
#define IS_RET_VALUE_OF_DEFAULT_TYPE_ERROR(val) (val)

#define FREE_N_SET_NULLs(ptr) {free(ptr); ptr=NULL;}//ptr=NULL brings overhead, but sometimes you need this macro. e.g.: you implement your own Mem Management functions including MALLOC, FREE, etc. you need to keep records of already freed pointers. (this macro should be used when writing code that requires almost infinite number of freeing scenarios)

//void *independent_rt_memory_management//todo need a global variable to manage your customized dynamic memory
//todo change INDEPENDENT_RT_XX_ALLOC to make them faster than standard C functions
#define INDEPENDENT_RT_ALIGNED_ALLOC(alignment,size) (aligned_alloc(alignment,size))
#define INDEPENDENT_RT_CALLOC(nmemb,size) (calloc(nmemb,size))
#define INDEPENDENT_RT_FREE(ptr) (free(ptr))
#define INDEPENDENT_RT_MALLOC(size) (malloc(size))//?better to check: ( ((uintmax_t)(size)>(uintmax_t)(PTRDIFF_MAX) || (uintmax_t)(size)>(uintmax_t)(SIZE_MAX)) ? (NULL): (malloc(size))) //but you probably don't need to check for any well-implemented c runtime
#define INDEPENDENT_RT_REALLOC(ptr,size) (realloc(ptr,size))

struct singly_linked_list_of_pvoid{void *obj;struct singly_linked_list_of_pvoid *next;};
#define MALLOC_LINKED_LIST_NODE_POINTED_BY(pnode) node->next=malloc(sizeof(*pnode))
#define MALLOC_LINKED_LIST_NODE_POINTS_TOs(pnode,pnew,caseFail,caseSucceed) {pnew=malloc(sizeof(*pnode));if(pnew){pnew->next=pnode;caseSucceed}else{caseFail}}
#define MALLOC_LINKED_LIST_NODE_POINTS_TO_NODE_N_REASSIGNs(pnode,ptem,caseFail,caseSucceed) MALLOC_LINKED_LIST_NODE_POINTS_TOs(pnode,ptem,caseFail,{pnode=ptem;caseSucceed})

//undone add alloca_linked_list macro, INIT_ALLOCA_LINKED_LIST, so it can be used as temp storage space throughout the function, when leaving the function, there is no need to free

#define REALLOC_N_CHECK_RETu(ui,ptr,size,caseFail,caseSucceed) {\
	void *ui##temp = realloc(ptr,size);\
	if(ui##temp==NULL){caseFail}\
	else{ptr = ui##temp;{caseSucceed}}\
}


//not very useful
#define MALLOC_MEMCPY_AS_IF_REALLOCu(ui,ptr,size,copied_size,caseFail,caseSucceed) {\
	assert((uintmax_t)copied_size<=(uintmax_t)size);\
	void *ui##temp = malloc(size);\
	if(ui##temp==NULL){caseFail}\
	else{\
		memcpy(ui##temp,ptr,copied_size);\
		free(ptr);\
		ptr = ui##temp;{caseSucceed}\
	}\
}

//you may use this if you are paranoid about realloc copying uninitialized values (though it's unlikely any compiler chokes on such realloc)
#define REALLOC_OR_MALLOC_MEMCPY_IF_COPY_PARTIALLYu(ui,ptr,size,copied_size,old_size,caseFail,caseSucceed) {\
	if((copied_size)==(old_size)){\
		REALLOC_N_CHECK_RETu(ui##realloc_,ptr,size,caseFail,caseSucceed)\
	}else{\
		assert((copied_size)<(old_size));\
		MALLOC_MEMCPY_AS_IF_REALLOCu(ui##mallocCpy_,ptr,size,copied_size,caseFail,caseSucceed)\
	}\
}

#define DOUBLE_X_WITHOUT_Y(x,y) ((x)*2)
#define AT_LEAST_ONE_N_HALF_OF_X_AS_MULTIPLE_OF_Y(x,y) ( ((x)/(y)/2+1)*3*(y) )
#define AT_LEAST_ONE_N_HALF_OF_s_B_PLUS_C_s_AS_MULTIPLE_OF_D(a,b,c,d) AT_LEAST_ONE_N_HALF_OF_X_AS_MULTIPLE_OF_Y((b)+(c),d)

struct pvoid_with_size_n_capacity{void *obj; size_t si; size_t cap;};
struct pvoid_with_pend_n_capacity{void *obj; void *pend; size_t cap;};
//PVOID_WITH_SI_CA needs to include . or ->
//note evaluate memb only once, not even used with `sizeof` in case it's a VLA
#define APPEND_TO_PVOID_WITH_SI_CA_N_REALLOC_IF_NEEDEDu(ui,PVOID_WITH_SI_CA,memb,typ_pmemb,ARITHMETIC_INC_SIZE,caseFail,caseSucceed) {\
	assert(PVOID_WITH_SI_CA cap% (sizeof *(typ_pmemb)NULL) ==0);\
	assert(PVOID_WITH_SI_CA si% (sizeof *(typ_pmemb)NULL) ==0);\
	if(PVOID_WITH_SI_CA si==PVOID_WITH_SI_CA cap){\
		PVOID_WITH_SI_CA cap = ARITHMETIC_INC_SIZE(PVOID_WITH_SI_CA si,sizeof *(typ_pmemb)NULL);\
		REALLOC_N_CHECK_RETu(ui##realloc_,PVOID_WITH_SI_CA obj,PVOID_WITH_SI_CA cap,{\
			PVOID_WITH_SI_CA cap = PVOID_WITH_SI_CA si;\
			{caseFail}\
		},{\
			( (typ_pmemb)PVOID_WITH_SI_CA obj + (PVOID_WITH_SI_CA si/(sizeof *(typ_pmemb)NULL)) )[0]=memb;\
			PVOID_WITH_SI_CA si +=sizeof *(typ_pmemb)NULL;\
			{caseSucceed}\
		})\
	}else{\
		assert(PVOID_WITH_SI_CA si < PVOID_WITH_SI_CA cap);\
		( (typ_pmemb)PVOID_WITH_SI_CA obj + (PVOID_WITH_SI_CA si/(sizeof *(typ_pmemb)NULL)) )[0]=memb;\
		PVOID_WITH_SI_CA si +=sizeof *(typ_pmemb)NULL;\
		{caseSucceed}\
	}\
}
#define MEMCPY_TO_THE_END_OF_PVOID_N_REALLOC_IF_NEEDEDu(ui,obj,siExpression,cap,pdata,dsize,ARITHMETIC_INC_SIZE,lastarg_granular,caseFail,caseSucceed,NOT_NECESSARILY_A_FUNC_LIKE_MACRO_TAKES_DSIZE){\
	if((size_t)(cap)<(size_t)((siExpression)+(dsize))){\
		size_t ui##newca = ARITHMETIC_INC_SIZE(cap,siExpression,dsize,lastarg_granular);\
		REALLOC_N_CHECK_RETu(ui##realloc,obj,ui##newca,{caseFail},{\
			cap = ui##newca;\
			memcpy((char *)(obj)+(siExpression),pdata,dsize);\
			NOT_NECESSARILY_A_FUNC_LIKE_MACRO_TAKES_DSIZE(dsize);\
			{caseSucceed}\
		})\
	}else{\
		memcpy((char *)(obj)+(siExpression),pdata,dsize);\
		NOT_NECESSARILY_A_FUNC_LIKE_MACRO_TAKES_DSIZE(dsize);\
		{caseSucceed}\
	}\
}
#define MEMCPY_TO_THE_END_OF_PVOID_WITH_SI_CA_N_REALLOC_IF_NEEDEDu(ui,PVOID_WITH_SI_CA,pdata,dsize,ARITHMETIC_INC_SIZE,lastarg_granular,caseFail,caseSucceed) MEMCPY_TO_THE_END_OF_PVOID_N_REALLOC_IF_NEEDEDu(ui,PVOID_WITH_SI_CA obj, PVOID_WITH_SI_CA si, PVOID_WITH_SI_CA cap, pdata,dsize,ARITHMETIC_INC_SIZE,lastarg_granular,caseFail,caseSucceed,PVOID_WITH_SI_CA si+=)
#define MEMCPY_TO_THE_END_OF_PVOID_WITH_PEND_CA_N_REALLOC_IF_NEEDEDu(ui,PVOID_WITH_PEND_CA,pdata,dsize,ARITHMETIC_INC_SIZE,lastarg_granular,caseFail,caseSucceed) MEMCPY_TO_THE_END_OF_PVOID_N_REALLOC_IF_NEEDEDu(ui,PVOID_WITH_PEND_CA obj, ((char *)PVOID_WITH_PEND_CA pend-(char *)PVOID_WITH_PEND_CA obj), PVOID_WITH_PEND_CA cap, pdata,dsize,ARITHMETIC_INC_SIZE,lastarg_granular,caseFail,caseSucceed,PVOID_WITH_PEND_CA pend=((char *)PVOID_WITH_PEND_CA pend)+)

#define MEMCPY_TO_THE_END_OF_PVOID_WITH_PEND_CA_N_REALLOC_IF_NEEDED_N_SET_SOURCE_PTRu(ui,PVOID_WITH_PEND_CA,pdata,dsize,ARITHMETIC_INC_SIZE,lastarg_granular,caseFail,caseSucceed) MEMCPY_TO_THE_END_OF_PVOID_WITH_PEND_CA_N_REALLOC_IF_NEEDEDu(ui,PVOID_WITH_PEND_CA,pdata,dsize,ARITHMETIC_INC_SIZE,lastarg_granular,caseFail,{pdata+=(dsize);{caseSucceed}})

#define SET_PEND_MALLOC_PVOID_WITH_PEND_CAs(PVOID_WITH_PEND_CA, ca, caseFail, caseSucceed){\
        PVOID_WITH_PEND_CA cap=(ca);\
        PVOID_WITH_PEND_CA obj=malloc(PVOID_WITH_PEND_CA cap);\
        if(NULL==PVOID_WITH_PEND_CA obj){caseFail}\
        else{\
                PVOID_WITH_PEND_CA pend=PVOID_WITH_PEND_CA obj;\
                {caseSucceed}\
        }\
}
#define SET_SI_MALLOC_PVOID_WITH_SI_CAs(PVOID_WITH_SI_CA, ca, caseFail, caseSucceed){\
        PVOID_WITH_SI_CA si=0;\
        PVOID_WITH_SI_CA cap=(ca);\
        PVOID_WITH_SI_CA obj=malloc(PVOID_WITH_SI_CA cap);\
        if(NULL==PVOID_WITH_SI_CA obj){caseFail}\
        else{caseSucceed}\
}
#define FREE_PVOID_WITH_SI_CA(PVOID_WITH_SI_CA) (free(PVOID_WITH_SI_CA obj))
#define FREE_PVOID_WITH_PEND_CA(PVOID_WITH_PEND_CA) (free(PVOID_WITH_PEND_CA obj))



//you can use std::search in c++ instead of this macro (or find function in the case of string)
//assume hl>=nl>1
//note pchar doesn't point to the first char
#define UNSAFE_MEMMEMs(pchar,h,hl,n,nl,caseNotFound,caseFound){\
	(pchar)=(char *)memchr(h,((unsigned char *)(n))[0],(hl)-(nl)+1);\
	for(;;){\
		if(pchar){\
			++(pchar);\
			if(memcmp(pchar,(char *)(n)+1,(nl)-1)){\
				if((hl)-((pchar)-(char *)(h))>=(nl)){\
					(pchar)=(char *)memchr(pchar,((unsigned char *)(n))[0],(hl)-((pchar)-(char *)(h))-(nl)+1);\
					continue;\
				}else{caseNotFound}\
			}else{caseFound}\
		}else{caseNotFound}\
		break;\
	}\
}




//todo what about strtoimax & strtoumax
#define STRTO_U_L_LL_WITHOUT_ENDPTRu(ui,ret,func,nptr,base,caseFail,caseSucceed) {\
        char *ui##temp;\
        errno = 0;\
        ret=func((nptr),&ui##temp,(base));\
        if(errno==ERANGE||ui##temp==(nptr)){caseFail}\
        else{caseSucceed}\
}
#define STRTOUL_WITHOUT_ENDPTRu(ui,ret,nptr,base,caseFail,caseSucceed) STRTO_U_L_LL_WITHOUT_ENDPTRu(ui,ret,strtoul,nptr,base,caseFail,caseSucceed)
#define STRTOULL_WITHOUT_ENDPTRu(ui,ret,nptr,base,caseFail,caseSucceed) STRTO_U_L_LL_WITHOUT_ENDPTRu(ui,ret,strtoull,nptr,base,caseFail,caseSucceed)



#define FILE_XSTR_LINE __FILE__ "," XSTR(__LINE__)
#define STR_FILE_FUNC_XSTR_LINE (std::string(__FILE__ " ")+__func__+"," XSTR(__LINE__))










#include <iomanip>

//1st method for throwing from destructor
#if defined(__GNUC__) && __GNUC__<6
#else
#define IF_SUE_EQ_UE_THROW {if(sue()==std::uncaught_exceptions())throw;}
#define TRY_TEM_SET_SUE try{tem_set_sue temsetsueph;
#define CATCH_IF_SUE_EQ_UE_THROW }catch(...){IF_SUE_EQ_UE_THROW}
inline auto &sue(void)noexcept{//for C++17 you can use an inline variable, instead of this function?
	//When thread_local is applied to a variable of block scope the storage-class-specifier static is implied if no other storage-class-specifier appears in the decl-specifier-seq.
	thread_local int sue_;//stored(saved) uncaught_exceptions
	return sue_;
}
struct tem_set_sue{
	int prev=sue();
	tem_set_sue(){
		sue()=std::uncaught_exceptions();
	}
	~tem_set_sue(){
		sue()=prev;
	}
};
#endif

//2nd method for throwing from destructor (unusable for throwable class member)
inline auto &sue2(void){//for C++17 you can use an inline variable, instead of this function?
	thread_local bool sue2_;//note you cannot reuse sue. must use a separate variable. in case you use both methods in a program.
	return sue2_;
}
#define INIT_TRYs(init) {init try{
#define CATCH_SET_SUE_THROWs(stmts_noexcept) }catch(...){stmts_noexcept sue2()=1; throw;}}//stmts_noexcept is optional argument
#define AUTO_COPY_OF_SUE_CLEAR_SUE_TRYs bool cp_sue_=sue2();sue2()=0;try{//dtor should begin with this
#define CATCH_IF_CHECK_COPY_OF_SUE_THROWs }catch(...){if(!cp_sue_)throw;}//dtor should end with this

//3rd method for throwing from destructor (not good?)
#define TRY try{
#define CATCH_FINALLYs(b) }catch(...){try{b}catch(...){}throw;}{b}//probably useless macro, break/return/goto can skip the b entirely

#define CATCH_FOR_LOGs }catch(...){std::clog<<"\nRETHROW " __FILE__ " "<<__func__<< "," XSTR(__LINE__) "\n";throw;}

//a type for matching handler where logging error is not needed. e.g. catch(T t){}catch(...){LOG_ERRs}.
//but this type is not very useful bc you may want to log your error multiple times for stack trace like log
struct e_already_logged{};

#ifdef _WIN32
#define FOPEN_WITH_NATIVE_FNM_ENC(fnm,literal_mode) _wfopen(fnm,L##literal_mode)
#define FREOPEN_WITH_NATIVE_FNM_ENC(fnm,literal_mode,s) _wfreopen(fnm,L##literal_mode,s)
#else
#define FOPEN_WITH_NATIVE_FNM_ENC(fnm,literal_mode) fopen(fnm,literal_mode)
#define FREOPEN_WITH_NATIVE_FNM_ENC(fnm,literal_mode,s) freopen(fnm,literal_mode,s)
#endif

#ifdef _WIN32
#define GMTIME_TSs(pt,ptm) {if( gmtime_r(ptm,pt))throw std::runtime_error("GMTIME_TSs "+STR_FILE_FUNC_XSTR_LINE);}
#define LOCALTIME_TSs(pt,ptm) {if( localtime_s(ptm,pt))throw std::runtime_error("LOCALTIME_TSs "+STR_FILE_FUNC_XSTR_LINE);}
#else
#define GMTIME_TSs(pt,ptm) {if(!gmtime_r(pt,ptm))throw std::runtime_error("GMTIME_TSs "+STR_FILE_FUNC_XSTR_LINE);}
#define LOCALTIME_TSs(pt,ptm) {if(!localtime_r(pt,ptm))throw std::runtime_error("LOCALTIME_TSs "+STR_FILE_FUNC_XSTR_LINE);}
#endif

#define STD_CLOG_TIME_FILE_FUNC_LINE (std::clog<<strftime_YmdHMS_localtime_ts_now()<<" " __FILE__ " "<<__func__<<"," XSTR(__LINE__) "\n")
#define STD_CLOG_TIME_FILE_FUNC_LINE_FLUSH (STD_CLOG_TIME_FILE_FUNC_LINE<<std::flush)
#define STD_CLOG_TIME_FILE_FUNC_LINE_EX_FLUSH(e) (STD_CLOG_TIME_FILE_FUNC_LINE<<e.what()<<'\n'<<std::flush)
#define STD_CLOG_TIME_FILE_FUNC_LINE_FLUSH_NOEXCEPTs {try{STD_CLOG_TIME_FILE_FUNC_LINE_FLUSH;}catch(...){}}
#define STD_CLOG_TIME_FILE_FUNC_LINE_EX_FLUSH_NOEXCEPTs(e) {try{STD_CLOG_TIME_FILE_FUNC_LINE_EX_FLUSH(e);}catch(...){}}

#define STD_CLOG_FILE_FUNC_LINE (std::clog<<" " __FILE__ " "<<__func__<<"," XSTR(__LINE__) "\n")
#define STD_CLOG_FILE_FUNC_LINE_FLUSH (STD_CLOG_FILE_FUNC_LINE<<std::flush)
#define STD_CLOG_FILE_FUNC_LINE_EX_FLUSH(e) (STD_CLOG_FILE_FUNC_LINE<<e.what()<<'\n'<<std::flush)
#define STD_CLOG_FILE_FUNC_LINE_FLUSH_NOEXCEPTs {try{STD_CLOG_FILE_FUNC_LINE_FLUSH;}catch(...){}}
#define STD_CLOG_FILE_FUNC_LINE_EX_FLUSH_NOEXCEPTs(e) {try{STD_CLOG_FILE_FUNC_LINE_EX_FLUSH(e);}catch(...){}}

template<bool b> void static_assert_in_template(){
	static_assert(b,"");
}
#define STATIC_ASSERT_IN_LAMBDA []{static_assert(true,"");}

#define ASSERT_THROWs(b){\
	if(!(b)) throw std::runtime_error("Assert "+STR_FILE_FUNC_XSTR_LINE);\
}
#ifndef NDEBUG
#define ASSERT_DEBUG_THROWs(b){}
#else
#define ASSERT_DEBUG_THROWs ASSERT_THROWs
#endif

CPPRS_COMMON_SP std::string strftime_YmdHMS_localtime_ts_now(void){
	char buf[15];
	time_t t_=std::time(nullptr);
	if(t_==(std::time_t)(-1)) throw std::runtime_error("Time err "+STR_FILE_FUNC_XSTR_LINE);
	tm tm_;
	LOCALTIME_TSs(&t_,&tm_)
	strftime(buf,15,"%Y%m%d%H%M%S",&tm_);
	return std::string(buf);
}
CPPRS_COMMON_SP void clog_put_time_YmdHMS_localtime_ts_now(void){
	time_t t_;
	tm tm_;
	LOCALTIME_TSs(&t_,&tm_)
	std::clog<<std::put_time(&tm_,"%Y%m%d%H%M%S");
}

//decltype(auto) also works, but `auto &` is better because no explicit array? (auto &l="some literal str";)
//? does using this template a lot cause bloat in binary size?
template<class ContainerT,class LiteralT>
void insert_end_literal(ContainerT &v,LiteralT &l){
	static_assert(std::is_same<decltype(l),const char(&)[sizeof l]>::value,"");
	v.insert(v.end(),l,(const char *)(&l+1)-1);
}
template<class ContainerT,class LiteralT>
void insert_end_literal_with_nul(ContainerT &v,LiteralT &l){
	static_assert(std::is_same<decltype(l),const char(&)[sizeof l]>::value,"");
	v.insert(v.end(),l,(const char *)(&l+1));
}

#include <random>
template<class IntT>
struct integer_good_randomness{
	std::mt19937 rngmt{std::random_device()()};
	std::uniform_int_distribution<IntT> uidist{std::numeric_limits<IntT>::min(), std::numeric_limits<IntT>::max()};
	auto gener(void){
		return uidist(rngmt);
	}
};

CPPRS_COMMON_SP uint32_t u32frombytesle(unsigned char *uc){//optimize
	uint32_t tbr=uc[3];
	tbr*=256;
	tbr+=uc[2];
	tbr*=256;
	tbr+=uc[1];
	tbr*=256;
	tbr+=uc[0];
	return tbr;
}
CPPRS_COMMON_SP void write_u32le(void *b, uint32_t u){//optimize
	*(unsigned char *)b=u%0x100;
	u/=0x100;
	*((unsigned char *)b+1)=u%0x100;
	u/=0x100;
	*((unsigned char *)b+2)=u%0x100;
	*((unsigned char *)b+3)=u/0x100;
}
//note there are more than 2 kinds of endianness in this world.
//Big-endian functions can be implemented with `htonl` and `ntohl`.
template<class T>
void resize_write_u32le(T &b,uint32_t u){
	b.resize(b.size()+4);
	write_u32le(b.data()+b.size()-4,u);
}

#include <fstream>

//this function is bad? it's slow? it has side effects? it might not even work?
template<class T>
auto getfilesizeusingifstream(const T &filenm){
	std::ifstream ifstre_(filenm,std::ios_base::binary);//::ate can be added to mode? but msvc has bug so it doesn't work on big file?
	if(!ifstre_)throw std::runtime_error("file err "+STR_FILE_FUNC_XSTR_LINE);
	//no need to check ret?
	auto fsi=ifstre_.tellg();//not guaranteed to return 0? though in the real world it always returns 0?
	if(!ifstre_.seekg(0,std::ios_base::end))throw std::runtime_error("seek err "+STR_FILE_FUNC_XSTR_LINE);
	auto tbr=ifstre_.tellg()-fsi;
	ifstre_.close();
	if(!ifstre_)throw std::runtime_error("close err "+STR_FILE_FUNC_XSTR_LINE);
	return tbr;
}


template<class T>
auto readwholefileintovectorchar(const T &filenm){//optimize use filesystem::file_size? but in that case you'd use unique_ptr instead of vector<char>?
	constexpr size_t initsi=0x1000;//std::vector<char>::size_type will be deprecated in c++17?
	std::vector<char> tbr(initsi);
	std::ifstream ifstre_(filenm,std::ios_base::binary);
	if(!ifstre_)throw std::runtime_error("file err "+STR_FILE_FUNC_XSTR_LINE);
	for(size_t fsi=0;;){
		if(!ifstre_.read(tbr.data()+fsi,initsi)){
			if(!ifstre_.eof())throw std::runtime_error("read err "+STR_FILE_FUNC_XSTR_LINE);
			fsi+=ifstre_.gcount();//?
			ifstre_.clear(); ifstre_.close();
			if(!ifstre_)throw std::runtime_error("close err "+STR_FILE_FUNC_XSTR_LINE);
			tbr.resize(fsi);
			tbr.shrink_to_fit();//?
			return tbr;
		}
		fsi+=initsi;
		tbr.resize(fsi+initsi);
	}
}

template<class T=uintmax_t,class Tb,class Te>
T nthpowerof(Tb base,Te ex){//optimize in loop, square the base for the number of bit times in ex, instead of multiplying one by one
	if(!ex)return 1;
	assert(ex>=0);
	T tbr=1;
	T cob=base;
	for(;;){
		if(ex%2)tbr*=cob;
		if(!(ex/=2))break;
		cob*=cob;
	}
	return tbr;
}

//? only support char or unsigned char type?
template<class T>
auto strstrehexsetfillwrite(T pint,size_t len){
	std::stringstream oss;
	oss<<std::hex<<std::setfill('0');
	for(size_t i=0;i<len;++i)
		oss<<std::setw(2)<<(unsigned)(unsigned char)pint[i];//note unsigned char doesn't work?
	return oss;
}

//interval is [s:e)
template<class T1,class T2,class T3, class T4>
bool check2intervalsoverlap(T1 so1,T2 eo1,T3 so2,T4 eo2){
	return so1<eo2 && so2<eo1;
}
