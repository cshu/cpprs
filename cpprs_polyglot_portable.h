#pragma once


//learn from boost
//note buf_size_num_of_wchar should be int/size_t
#define MALLOC_EXTRA_MEM_N_MULTIBYTETOWIDECHAR_UTF8_STRs(utfstr,pwchart,buf_size_num_of_wchar,extra_num_of_bytes,caseFail,caseSucceed){\
        buf_size_num_of_wchar = MultiByteToWideChar(CP_UTF8,0,utfstr,-1,NULL,0);\
        if(!buf_size_num_of_wchar){caseFail}\
        else{\
                pwchart = IMPLICIT_STATIC_CAST( wchar_t *,malloc((size_t)buf_size_num_of_wchar*sizeof(wchar_t)+(size_t)(extra_num_of_bytes)) );\
                if(NULL==pwchart){caseFail}\
                else{\
                        if(!MultiByteToWideChar(CP_UTF8,0,utfstr,-1,pwchart,buf_size_num_of_wchar)){free(pwchart);{caseFail}}\
                        else{caseSucceed}\
                }\
        }\
}
#define MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(utfstr,pwchart,buf_size_num_of_wchar,caseFail,caseSucceed) MALLOC_EXTRA_MEM_N_MULTIBYTETOWIDECHAR_UTF8_STRs(utfstr,pwchart,buf_size_num_of_wchar,0,caseFail,caseSucceed)

//note get_filesystem_name_with_utf8stru might use the original str object!
#ifdef _WIN32
typedef wchar_t *filesystem_name;
struct filesystem_entry{filesystem_name nm; WIN32_FILE_ATTRIBUTE_DATA fileAttrDt;};
#define GET_FILESYSTEM_NAME_WITH_UTF8STRu(ui,str,fname,caseFail,caseDone) MAKE_FILESYSTEM_NAME_WITH_UTF8STRu(ui,str,fname,caseFail,caseDone)
#define MAKE_FILESYSTEM_NAME_WITH_UTF8STRu(ui,str,fname,caseFail,caseDone){\
        int ui##sizeInWchar;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(str,fname,ui##sizeInWchar,{caseFail},{caseDone})\
}
#define FREE_GOT_FILESYSTEM_NAMEs(fnm) {free(fnm);}
#define FREE_MADE_FILESYSTEM_NAMEs(fnm) {free(fnm);}
//undone #define FILESYSTEM_ENTRY_mGET_LEN_SIZE_T
//undone #define FILESYSTEM_ENTRY_mENSURE_EXIST
//undone #define FILESYSTEM_ENTRY_mENSURE_DIR_OR_OTHER_KIND_OF_ENTRY_EXIST
//undone #define mReadfile, mWrite, etc.

//undone might need to add bit array to the struct for marking which field has been set (size, time, blabla)
#else
typedef char *filesystem_name
struct filesystem_entry{filesystem_name nm; struct stat st_stat;};
#define GET_FILESYSTEM_NAME_WITH_UTF8STRu(ui,str,fname,caseFail,caseDone){(fname)=(str);}
#define MAKE_FILESYSTEM_NAME_WITH_UTF8STRu(ui,str,fname,caseFail,caseDone){\
        if(NULL==((fname)=strdup(str))){caseFail}else{caseDone}\
}
#define FREE_GOT_FILESYSTEM_NAMEs(fnm)
#define FREE_MADE_FILESYSTEM_NAMEs(fnm) {free(fnm);}
#endif

//note all filenames here are supposed to be in UTF-8!!

//note PathFileExists is usable but requires some linking, so forget about it. (besides, it seems not many people use it.)

#ifdef _WIN32
#define ENSURE_FILESYSTEM_ENTRY_EXISTSu(ui,name,caseNotFoundOrFail,caseSucceed){\
        wchar_t *ui##filenm; int ui##sizeInWchar;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(name,ui##filenm,ui##sizeInWchar,{caseNotFoundOrFail},{\
                if(_waccess(ui##filenm,0)){free(ui##filenm); {caseNotFoundOrFail}}\
                else{free(ui##filenm); {caseSucceed}}\
        }\
}
#else
# ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64//stat syscall works on file bigger than 2gb, provided this is defined. (the same can be said of access? really?)
# elif _FILE_OFFSET_BITS != 64
#error stat cannot work properly with THIS _FILE_OFFSET_BITS
# endif
#define ENSURE_FILESYSTEM_ENTRY_EXISTSu(ui,name,caseNotFoundOrFail,caseSucceed){\
        if(access(name,F_OK)){caseNotFoundOrFail}\
        else{caseSucceed}\
}
#endif

//you can use ENOENT == errno after stat() failed to check if the reason of failure is "path doesn't exist", but it's a trouble for WIN, so forget about it
//opendir is also an optional for checking, but is it faster than stat?
#ifdef _WIN32
#define ENSURE_DIR_OR_OTHER_KIND_OF_FILESYSTEM_ENTRY_EXISTSu(ui,name,caseNotFoundOrFail,caseNotDir,caseDir){\
        wchar_t *ui##filenm; int ui##sizeInWchar;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(name,ui##filenm,ui##sizeInWchar,{caseNotFoundOrFail},{\
                WIN32_FILE_ATTRIBUTE_DATA ui##attrdt;\
                if(!GetFileAttributesExW(ui##filenm,GetFileExInfoStandard,&ui##attrdt)){free(ui##filenm);{caseNotFoundOrFail}}\
                else{\
                        free(ui##filenm);\
                        if(ui##attrdt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){caseDir}else{caseNotDir}\
                }\
        })\
}
#else
#define ENSURE_DIR_OR_OTHER_KIND_OF_FILESYSTEM_ENTRY_EXISTSu(ui,name,caseNotFoundOrFail,caseNotDir,caseDir){\
        struct stat ui##st;\
        if(stat(name,&ui##st)){caseNotFoundOrFail}else{\
                if(ui##st.st_mode & S_IFDIR){caseDir}/*should be the same as S_ISDIR()*/\
                else{caseNotDir}\
        }\
}
#endif

//SEEK_END may not be supported (e.g. possible undefined behavior on binary file), so fseek is not an option
//learn from boost
#ifdef _WIN32
#define GET_FILE_LENGTH_SIZE_Tu(ui,filename,si,caseNotFoundOrFail,caseSucceed){\
        wchar_t *ui##filenm; int ui##sizeInWchar;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(filename,ui##filenm,ui##sizeInWchar,{caseNotFoundOrFail},{\
                WIN32_FILE_ATTRIBUTE_DATA ui##attrdt;\
                if(!GetFileAttributesExW(ui##filenm,GetFileExInfoStandard,&ui##attrdt) || (ui##attrdt.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)){free(ui##filenm);{caseNotFoundOrFail}}/*be safe, when it's a dir, filesize value might be indeterminate?*/\
                else{\
                        free(ui##filenm);\
                        si=((size_t)ui##attrdt.nFileSizeHigh<<32)+(size_t)ui##attrdt.nFileSizeLow;\
                        {caseSucceed}\
                }\
        })\
}
#else
#error Not implemented
//struct stat;
//stat(,&st)
//!S_ISREG(st.st_mode)
//return (size_t) .st_size;
#endif

//note both win32 and posix versions fail when dir doesn't exist, and succeed with 0 when no file under dir
#ifdef _WIN32//it seems _wopendir is available, but there is no d_type so you cannot discriminate files and folders
#define COUNT_NUM_OF_FILESu(ui,dir,num,caseFail,caseSucceed){\
        wchar_t *ui##filenm; int ui##sizeInWchar;\
        MALLOC_EXTRA_MEM_N_MULTIBYTETOWIDECHAR_UTF8_STRs(dir,ui##filenm,ui##sizeInWchar,LEN_OF_WCHAR_T_LITERAL(L"\\*"),{caseFail},{/*excessive \ should be okay*/\
                memcpy(ui##filenm+ui##sizeInWchar-1,LITERAL_COMMA_SIZE(L"\\*"));\
                WIN32_FIND_DATAW ui##findData; HANDLE ui##hFind;\
                ui##hFind = FindFirstFileW(ui##filenm,&ui##findData);\
                free(ui##filenm);\
                if(INVALID_HANDLE_VALUE == ui##hFind){\
                        if(ERROR_FILE_NOT_FOUND!=GetLastError()){caseFail}/*note it returns ERROR_PATH_NOT_FOUND when no dir. Actually checking is probably unnecessary, bc on Windows there are always "." ".." or "System Volume Information" existing for any path*/\
                        else{num=0; {caseSucceed}}\
                }else{\
                        num=0;\
                        do{\
                                if(!(ui##findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ++num;\
                        }while(FindNextFileW(ui##hFind,&ui##findData));\
                        if(ERROR_NO_MORE_FILES!=GetLastError()){FindClose(ui##hFind); {caseFail}}else{FindClose(ui##hFind); {caseSucceed}}\
                }\
        })\
}
#else
#define COUNT_NUM_OF_FILESu(ui,dir,num,caseFail,caseSucceed){\
        DIR *ui##pdir=opendir(dir); struct dirent *ui##pdirent\
        if(!ui##pdir){caseFail}else{\
                num=0;\
                errno=0;\
                while(NULL!=(ui##pdirent=readdir(ui##pdir))){if(ui##pdirent->d_type==DT_REG) ++num;}\
                if(errno){closedir(ui##pdir); {caseFail}}\
                else{\
                        if(closedir(ui##pdir)){caseFail}else{caseSucceed}\
                }\
        }\
}
#endif

#define READ_WHOLE_FILE_WITH_NUL_APPENDEDu(ui,str,out_fileLen_size_t,filename,caseFail,caseEmptyFile,caseSucceed) READ_WHOLE_FILE_N_ALLOC_EXTRA_MEM_N_SET_NULu(ui,str,out_fileLen_size_t,filename,1,caseFail,caseEmptyFile,caseSucceed)

#define READ_WHOLE_FILEu(ui,str,out_fileLen_size_t,filename,caseFail,caseEmptyFile,caseSucceed) READ_WHOLE_FILE_N_ALLOC_EXTRA_MEMu(ui,str,out_fileLen_size_t,filename,0,caseFail,caseEmptyFile,caseSucceed)

#define READ_WHOLE_FILE_N_ALLOC_EXTRA_MEM_N_SET_NULu(ui,str,out_fileLen_size_t,filename,extraLen,caseFail,caseEmptyFile,caseSucceed){\
        assert(extraLen>0);\
        READ_WHOLE_FILE_N_ALLOC_EXTRA_MEMu(ui,str,out_fileLen_size_t,filename,extraLen,caseFail,caseEmptyFile,{ str[out_fileLen_size_t]=0; {caseSucceed} })\
}

//note caseIOFail might still be caused by mem alloc, too much bother to discriminate
#define READ_WHOLE_FILE_N_ALLOC_EXTRA_MEMu(ui,str,out_fileLen_size_t,filename,extraLen,caseFail,caseEmptyFile,caseSucceed){\
        GET_FILE_LENGTH_SIZE_Tu(ui##getLen_,filename,out_fileLen_size_t,{caseFail},{\
                if(!out_fileLen_size_t){caseEmptyFile}else{\
                        MALLOC_N_FOPEN_RB_FREAD_FCLOSEu(ui##mallocFopen_,((size_t)extraLen+out_fileLen_size_t),str,out_fileLen_size_t,filename,caseFail,caseSucceed)\
                }\
        })\
}

#ifdef _WIN32
#define FOPEN_WITH_LITERAL_MODEu(ui,filename,pf,caseFail,caseOpened,literal_mode){\
        wchar_t *ui##filenm; int ui##sizeInWchar;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(filename,ui##filenm,ui##sizeInWchar,{caseFail},{\
                pf=_wfopen(ui##filenm,L##literal_mode);\
                free(ui##filenm);\
                {caseOpened}\
        })\
}
#else
#define FOPEN_WITH_LITERAL_MODEu(ui,filename,pf,caseFail,caseOpened,literal_mode){\
        pf=fopen(filename,literal_mode); {caseOpened}\
}
#endif
#define FOPEN_RBu(ui,filename,pf,caseFail,caseOpened) FOPEN_WITH_LITERAL_MODEu(ui,filename,pf,caseFail,caseOpened,"rb")
#define FOPEN_WBu(ui,filename,pf,caseFail,caseOpened) FOPEN_WITH_LITERAL_MODEu(ui,filename,pf,caseFail,caseOpened,"wb")

#define FOPEN_RB_FREAD_FCLOSEu(ui,buf,sizeToRead,filename,caseFail,caseSucceed){\
        FILE *ui##pfile;\
        FOPEN_RBu(ui##rb_,filename,ui##pfile,{caseFail},{\
                if(NULL==ui##pfile){caseFail}else{\
                        if(1!=fread(buf,sizeToRead,1,ui##pfile)){fclose(ui##pfile); {caseFail}}else{\
                                if(fclose(ui##pfile)){caseFail}\
                                else{caseSucceed}\
                        }\
                }\
        })\
}
#define MALLOC_N_FOPEN_RB_FREAD_FCLOSEu(ui,sizeToAlloc,buf,sizeToRead,filename,caseFail,caseSucceed){\
        assert((uintmax_t)(sizeToAlloc)>=(uintmax_t)(sizeToRead));\
        buf = IMPLICIT_STATIC_CAST( char *,malloc(sizeToAlloc) );\
        if(NULL==buf){caseFail}else{\
                FOPEN_RB_FREAD_FCLOSEu(ui##fopen_,buf,sizeToRead,filename,{free(buf); {caseFail}},caseSucceed)\
        }\
}
#define FWRITE_DATA_N_FCLOSEs(data,size,pFile,caseIOFail,caseSucceed){\
        if(1!=fwrite(data,size,1,pFile)){caseIOFail}\
        else{\
                if(fclose(pFile)){caseIOFail}\
                else{caseSucceed}\
        }\
}
#define FOPEN_WB_FWRITE_FCLOSEu(ui,data,size,filename,caseFail,caseSucceed){\
        FILE *ui##pfile;\
        FOPEN_WBu(ui##wb_,filename,ui##pfile,{caseFail},{\
                if(NULL==ui##pfile){caseFail}else{\
                        FWRITE_DATA_N_FCLOSEs(data,size,ui##pfile,{fclose(ui##pfile); {caseFail}},caseSucceed)\
                }\
        })\
}

//vsize must be greater than 0, or the behavior is strange
#define WRITE_WHOLE_FILE_IF_FILE_NOT_FOUND_OR_MEMCMPu(ui,filename,pvoid,vsize,caseFail,caseWritten,caseNoNeedToWrite){\
        assert((vsize)>0);\
        size_t ui##fileLen;\
        GET_FILE_LENGTH_SIZE_Tu(ui##getLen_,filename,ui##fileLen,{\
                FOPEN_WB_FWRITE_FCLOSEu(ui##noLen_,pvoid,vsize,filename,caseFail,caseWritten)\
        },{\
                if(ui##fileLen==(vsize)){\
                        char *ui##fileCon;\
                        MALLOC_N_FOPEN_RB_FREAD_FCLOSEu(ui##mallocFopen_,ui##fileLen,ui##fileCon,ui##fileLen,filename,caseFail,{\
                                if(memcmp(pvoid,ui##fileCon,ui##fileLen)){\
                                        free(ui##fileCon);\
                                        FOPEN_WB_FWRITE_FCLOSEu(ui##sameLen_,pvoid,vsize,filename,caseFail,caseWritten)\
                                }else{free(ui##fileCon); {caseNoNeedToWrite}}\
                        })\
                }else{\
                        FOPEN_WB_FWRITE_FCLOSEu(ui##diffLen_,pvoid,vsize,filename,caseFail,caseWritten)\
                }\
        })\
}

//note all `rename_XXX` macros here probably fails when the move crosses volume / mounted file system. (For posix you can check the reason like `errno == EXDEV`, but is windows error code also EXDEV? or EACCES?)
#ifdef _WIN32
//note windows _wrename can RENAME dir, but cannot MOVE dir to a different path
#define RENAME_DIR_s_REPLACE_IF_NAME_ALREADY_EXISTS_AS_EMPTY_DIRu(ui,oldf,newf,caseFail,caseDone){\
        wchar_t *ui##oldw; int ui##oldwcount;\
        wchar_t *ui##neww; int ui##newwcount;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(oldf,ui##oldw,ui##oldwcount,{caseFail},{\
                MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(newf,ui##neww,ui##newwcount,{caseFail},{\
                        _wrmdir(ui##neww);\
                        if(MoveFileExW(ui##oldw,ui##neww,0)){free(ui##oldw); free(ui##neww); {caseDone}}\
                        else{free(ui##oldw); free(ui##neww); {caseFail}}\
                }\
        })\
}
#define RENAME_FILE_s_FAIL_IF_NAME_ALREADY_EXISTSu(ui,oldf,newf,caseFail,caseDone){\
        wchar_t *ui##oldw; int ui##oldwcount;\
        wchar_t *ui##neww; int ui##newwcount;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(oldf,ui##oldw,ui##oldwcount,{caseFail},{\
                MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(newf,ui##neww,ui##newwcount,{caseFail},{\
                        if(_wrename(ui##oldw,ui##neww)){free(ui##oldw); free(ui##neww); {caseFail}}\
                        else{free(ui##oldw); free(ui##neww); {caseDone}}\
                }\
        })\
}
#define RENAME_FILE_s_REPLACE_IF_NAME_ALREADY_EXISTS_AS_FILE(ui,oldf,newf,caseFail,caseDone){\
        wchar_t *ui##oldw; int ui##oldwcount;\
        wchar_t *ui##neww; int ui##newwcount;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(oldf,ui##oldw,ui##oldwcount,{caseFail},{\
                MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(newf,ui##neww,ui##newwcount,{caseFail},{\
                        if(MoveFileExW(ui##oldw,ui##neww,MOVEFILE_REPLACE_EXISTING)){free(ui##oldw); free(ui##neww); {caseDone}}\
                        else{free(ui##oldw); free(ui##neww); {caseFail}}\
                }\
        })\
}
#else
#define RENAME_DIR_s_REPLACE_IF_NAME_ALREADY_EXISTS_AS_EMPTY_DIRu(ui,oldf,newf,caseFail,caseDone) {if(rename(oldf,newf)){caseFail}else{caseDone}}
//note check file exists and then rename() can cause race condition: no file when you check, but before rename(), a file is created
#define RENAME_FILE_s_FAIL_IF_NAME_ALREADY_EXISTSu(ui,oldf,newf,caseFail,caseDone){\
        if(link(oldf,newf)){caseFail}else{\
                if(unlink(oldf)){caseFail}else{caseDone}\
        }\
}

#define RENAME_FILE_s_REPLACE_IF_NAME_ALREADY_EXISTS_AS_FILE(ui,oldf,newf,caseFail,caseDone) {if(rename(oldf,newf)){caseFail}else{caseDone}}
#endif


#define STDC_FUNCTION_REMOVE_N_CHECK_RETVALs(nm,caseFail,caseDone){if(remove(nm)){caseFail}else{caseDone}}

#ifdef _WIN32
#define UNLINK_FILE_OR_RMDIR_DIRu(ui,nm,caseFail,caseDone){\
        wchar_t *ui##filenm; int ui##sizeInWchar;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(nm,ui##filenm,ui##sizeInWchar,{caseFail},{\
                if(_wrmdir(ui##filenm)){\
                        if(_wunlink(ui##filenm)){free(ui##filenm); {caseFail}}\
                        else{free(ui##filenm); {caseDone}}\
                }\
                else{free(ui##filenm); {caseDone}}\
        })\
}
#else
#define UNLINK_FILE_OR_RMDIR_DIRu(ui,nm,caseFail,caseDone) STDC_FUNCTION_REMOVE_N_CHECK_RETVALs(nm,caseFail,caseDone)
#endif

#ifdef _WIN32
#define REMOVE_FILE_WITH_UTF8_NAMEu(ui,nm,caseFail,caseDone){\
        wchar_t *ui##filenm; int ui##sizeInWchar;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(nm,ui##filenm,ui##sizeInWchar,{caseFail},{\
                if(_wunlink(ui##filenm)){free(ui##filenm); {caseFail}}\
                else{free(ui##filenm); {caseDone}}\
        })\
}
#define REMOVE_EMPTY_DIR_WITH_UTF8_NAMEu(ui,nm,caseFail,caseDone){\
        wchar_t *ui##filenm; int ui##sizeInWchar;\
        MALLOC_N_MULTIBYTETOWIDECHAR_UTF8_STRs(nm,ui##filenm,ui##sizeInWchar,{caseFail},{\
                if(_wrmdir(ui##filenm)){free(ui##filenm); {caseFail}}\
                else{free(ui##filenm); {caseDone}}\
        })\
}
#else
#define REMOVE_FILE_WITH_UTF8_NAMEu(ui,nm,caseFail,caseDone) STDC_FUNCTION_REMOVE_N_CHECK_RETVALs(nm,caseFail,caseDone)
#define REMOVE_EMPTY_DIR_WITH_UTF8_NAMEu(ui,nm,caseFail,caseDone) STDC_FUNCTION_REMOVE_N_CHECK_RETVALs(nm,caseFail,caseDone)
#endif


#define COMMON_MAX_FILENAME_LENGTH 255//this value might not be accurate, and it's only of file name component, not full path

#define RT_FILESYSTEM_WINAPI_MAXPATH 260
#define RT_FILESYSTEM_WINAPI_EXTENDED_MAXPATH 32767
#define RT_FILESYSTEM_WINAPI_COMMON_lpMaximumComponentLength 255//not so sure about what this value is about

#define COMMANDPROMPTMAXSTRLEN_WINXPORLATER 8191
#define COMMANDPROMPTMAXSTRLEN_WIN2K 2047
//unix `getconf ARG_MAX` should be greater?
#define BUFFER_LEN_FOR_C_SYSTEM_FUNCTION_SAFE COMMANDPROMPTMAXSTRLEN_WIN2K

//note the type of argument
#ifdef _WIN32
#include <windows.h>
#define SLEEP_MSs(err_int,ms) {err_int=0; Sleep( (DWORD)(ms) );}
#define SLEEP_Ss(numOfSecLeft_unsigned,seconds) {numOfSecLeft_unsigned=0; Sleep( (DWORD)1000 * (DWORD)(seconds) );}
#else//posix
#include <unistd.h>
//note useconds_t "shall be an unsigned integer type capable of storing values at least in the range [0, 1000000]"
#define SLEEP_MSs(err_int,ms) {err_int = usleep( (useconds_t)1000 * (useconds_t)(ms) );}
#define SLEEP_Ss(numOfSecLeft_unsigned,seconds) {numOfSecLeft_unsigned=sleep( (unsigned)(seconds) );}
#endif

//? latest msvc supports %zu?
#ifdef _WIN32
#define LENGTH_MODIFIER_SIZE_T "I"
#else
#define LENGTH_MODIFIER_SIZE_T "z"
#endif


//on many architectures this "will turn off_t into a 64-bit type".
//stat syscall works on file bigger than 2gb, provided this is defined.
#define _FILE_OFFSET_BITS 64

#ifdef _WIN32
#define SET_RB_STDIN_IF_NEEDEDs(caseErr,caseSucceed){\
	if(-1==_setmode( _fileno( stdin ), _O_BINARY )) {caseErr}\
	else {caseSucceed}\
}
#else
#define SET_RB_STDIN_IF_NEEDEDs(caseErr,caseSucceed) {caseSucceed}
//no need for linux, but what about other OSs? possibly:
//freopen(NULL, "rb", stdin);
#endif

