#include <stdint.h>


uint64_t add_mod(uint64_t a,uint64_t b,uint64_t m);
uint64_t power_mod(uint64_t b,uint64_t i,uint64_t m);
uint64_t mod(uint64_t x,uint64_t m);

uint64_t multimod(uint64_t a, uint64_t b, uint64_t m) {
	uint64_t ans=0;
	
	uint64_t b_mod=mod(b,m);
	
	for (int i=0;i<64;i++)
	{
		uint64_t tmp=(a>>i)&1;
		if(tmp==1){
			uint64_t x=power_mod(b_mod,i,m);
			ans=add_mod(ans,x,m);
		}
	}
	return ans;
}



uint64_t mod(uint64_t x,uint64_t m){
	if(m<=0){return 0;}
	if(x<m){ return x;}
	
	if((m&(m-1))==0)
	{
		return x&(m-1);
	}
	
	uint64_t n=m;
	uint64_t bits;
	
	for(bits=0;n;bits++)
	{
		n=n>>1;
	}
	
	for(int i=64-bits;i>=0;i--)
	{
		if(x>=(m<<i))
			x-=(m<<i);
	}
	return x;
}

uint64_t add_mod(uint64_t a,uint64_t b,uint64_t m){
	uint64_t t=a+b;
	if(t<a || t<b)  //即a+b溢出
	{
		return mod(mod(t,m)+mod((~m)+1,m),m);
	}
	else{
		return mod(t,m);
	}
	return 0;
}

uint64_t power_mod(uint64_t b,uint64_t i,uint64_t m){  //b has been moded by m.
	if(i==0){
		return mod(b,m);
	}	
	
	uint64_t result=b;

	for(int j=0;j<i;j++){
		//result=mod(result<<1,m);
		result=add_mod(result,result,m);
	}
	return result;
	
} 























