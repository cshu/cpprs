#pragma once


//undone can signed int be passed as parameter? if not then remove the static assert about signedness? just add doc about passed value should be positive?
/*dividend+divisor/2 can overflow*/
template<class Uint0,class Uint1>
auto roundUintDivision(Uint0 dividend, Uint1 divisor){
	static_assert(std::is_unsigned<Uint0>::value && std::is_integral<Uint0>::value && std::is_unsigned<Uint1>::value && std::is_integral<Uint1>::value,"st");
	return (dividend+divisor/2)/divisor;
}

/*divisor-1+dividend can overflow*/
template<class Uint0,class Uint1>
auto roundUpUintDivision(Uint0 dividend, Uint1 divisor){
	static_assert(std::is_unsigned<Uint0>::value && std::is_integral<Uint0>::value && std::is_unsigned<Uint1>::value && std::is_integral<Uint1>::value,"st");
	return (divisor-1+dividend)/divisor;
}

#ifndef IMPRECISE_FAST_DOWNSAMPLING_SAMPLE_RATE_LIMIT
#define IMPRECISE_FAST_DOWNSAMPLING_SAMPLE_RATE_LIMIT 15000
#endif
/*roundUpUintDivision can overflow*/
template<uint32_t sampleframesize,uint32_t sampleratelimit=IMPRECISE_FAST_DOWNSAMPLING_SAMPLE_RATE_LIMIT>//undone change to uint64_t?
auto impreciseFastDownsample(void *dt, uint32_t numofframes, uint32_t &samplerate){
	static_assert(sampleratelimit>0 && sampleframesize>0,"st");
	assert(numofframes>0);
	assert(samplerate>0);

	auto divisor {1+samplerate/sampleratelimit};
	static_assert(std::is_same<decltype(divisor),uint32_t>::value,"doc");
	auto reslen {roundUpUintDivision(numofframes,divisor)*sampleframesize};
	static_assert(std::is_same<decltype(reslen),uint32_t>::value,"doc");
	auto res {std::make_pair(new char[reslen],reslen)};
	static_assert(std::is_same<decltype(res),std::pair<char *,uint32_t>>::value,"doc");
	int offs{0};
	int offd{0};
	//auto dtsi{numofframes*sampleframesize};
	while(1){
		memcpy(res.first+offd,(char *)dt+offs,sampleframesize);
		++offd;
		if(offd==reslen)break;
		offs+=divisor;
		//if(offs>=dtsi) break;
	}
	samplerate=roundUintDivision(samplerate,divisor);
	return res;
}
