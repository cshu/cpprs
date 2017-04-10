#pragma once

//equivalent in c of the java version

#define READ_ALL_VALUES_N_MAKE_THEM_NUL_TERMINATED_IN_BUF//undone

static int readNValuesAndMakeThemNulTerminatedInBuf(char *bufstr, size_t n, char **bufForPVals){
	if(bufstr[0]<'1'||bufstr[0]>'9')
		return 1;
	unsigned numoflf = bufstr[0]-'0';
	char *sep = bufstr;
	do{
		bufstr = strchr(++bufstr, '\n');
		if(!bufstr) return 1;
		--numoflf;
	}while(numoflf);
	ptrdiff_t sizeofsep = bufstr-sep;
	memmove(sep,sep+1,sizeofsep);
	bufstr[0]=0;
	++bufstr;
	do{
		bufForPVals[--n]=bufstr;
		bufstr = strstr(bufstr,sep);
		if(!bufstr){
			if(n) return 1;
			else return 0;
		}
		bufstr[0]=0;
		if(!n) return 0;
		bufstr+=sizeofsep;
	}while(1);
}
