#pragma once
//designed to ensure compactness (e.g. VLQ has redundancy)
//1. every integer has unique representation
//2. arbitrary octets always represent an integer (100% utilization)

template<class T1,class T2>
void encode(T1 u,T2 l){
	for(uintmax_t m=0x80;;m*=0x80){
		if(u<m){
			for(;;){
				m/=0x80;
				if(m==1){
					l.push_back(u);
					return;
				}
				l.push_back(u%0x80+0x80);
				u/=0x80;
			}
		}
		u-=m;
	}
}

template<class T>
uintmax_t decode(T l){
	uintmax_t tbr=0;
	for(uintmax_t m=0x80;;m*=0x80){
		if(*l<0x80){
			uintmax_t addend=*l;
			for(;;){
				m/=0x80;
				if(m==1){
					return addend+tbr;
				}
				addend*=0x80;
				--l;
				addend+=*l-0x80;
			}
		}
		++l;
		tbr+=m;
	}
}
