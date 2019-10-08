

//todo write another version using filesystem::temp_directory_path
#ifndef CREATE_TMP_FILE_CASE_FAIL
# define CREATE_TMP_FILE_CASE_FAIL throw std::runtime_error(STR_FILE_FUNC_XSTR_LINE);
#endif
#include <iostream>
#include <cstdio>
decltype(auto) create_tmp_file(void){
	unique_ptr<char[]> chbuf(new char[L_tmpnam]);
	if(!std::tmpnam(chbuf.get())){
		CREATE_TMP_FILE_CASE_FAIL
	}
#ifdef _WIN32
	auto fildes = _open(chbuf.get(), _O_CREAT | _O_EXCL, _S_IREAD | _S_IWRITE);
	if(fildes==-1){
		CREATE_TMP_FILE_CASE_FAIL
	}
	if(_close(fildes)){
		CREATE_TMP_FILE_CASE_FAIL
	}
#else
	auto filep = fopen(chbuf.get(),"wx");//mingw-w64 supports x, but how about msvc??
	if(!filep){
		CREATE_TMP_FILE_CASE_FAIL
	}
	if(flose(filep)){
		CREATE_TMP_FILE_CASE_FAIL
	}
#endif
	return chbuf;
}
