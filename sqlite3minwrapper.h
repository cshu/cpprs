#pragma once


/*
** must use INIT_TRYs and CATCH_SET_SUE_THROWs
*/
struct slw_stmt{
        sqlite3_stmt* pstmt;
#ifdef NDEBUG
	virtual ~slw_stmt(){//? maybe you should add noexcept(false) to keep the same signature
		sqlite3_finalize(pstmt);
	}
#else
	virtual ~slw_stmt()noexcept(false){
		AUTO_COPY_OF_SUE_CLEAR_SUE_TRYs
		int ib=sqlite3_finalize(pstmt);
		if(ib!=SQLITE_OK){LOG_Is(ib) throw 0;}
		CATCH_IF_CHECK_COPY_OF_SUE_THROWs
	}
#endif
};

/*
** must use INIT_TRYs and CATCH_SET_SUE_THROWs
*/
struct slw_prestmt{
	slw_stmt s;
	slw_prestmt(sqlite3 *db,const char *zSql,int nByte){
		int ib=sqlite3_prepare_v2(db,zSql,nByte,&s.pstmt,nullptr);//note "If there is an error, *ppStmt is set to NULL."
		if(ib!=SQLITE_OK){LOG_Is(ib) throw 0;}
	}
};

#define MAKEBINDVALTOFINDSTRWITHLIKEu(ui,str,len,caseNoesc,caseEsc){\
	ASSERT_THROWs(len);\
	vector<char> ui##tem;ui##tem.reserve((len)+2);\
	ui##tem.push_back('%');\
	if(!memchr(str,'%',len)&&!memchr(str,'_',len)){\
		ui##tem.insert(ui##tem.end(),(char *)(str),(char *)(str)+(len));\
		ui##tem.push_back('%');\
		{caseNoesc}\
	}else{\
		for(remove_cv<decltype(len)>::type ui##i=0;ui##i<(len);++ui##i){\
			switch(((char *)(str))[ui##i]){\
			case '%':\
			case '_':\
			case '`':\
				ui##tem.push_back('`');\
			}\
			ui##tem.push_back(((char *)(str))[ui##i]);\
		}\
		ui##tem.push_back('%');\
		{caseEsc}\
	}\
}
