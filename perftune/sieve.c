#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#define N 10000000

static bool is_prime[N];
static int  primes[N];

int *sieve(int n) {
  assert(n + 1 < N);
  for (int i = 1; i <= n; i++)
    is_prime[i] = true;
  primes[0]=2;

  for (int i = 3; i <= n; i+=2) {
    if(is_prime[i]){
      int jump=i<<1;
      long long tmp=(long long )i*i;
      if(tmp>(long long)n) break;
   	  for (long long j = tmp; j <= n; j += jump) {
        is_prime[j] = false;
      }
    }
  }

  int *p = primes;
  *p++ = 2;
  for (int i = 3; i <= n; i += 2)
    if (is_prime[i]) {
      *p++ = i;
  }
  *p = 0;
  return primes;
}
