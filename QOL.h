#ifndef QOL_H
#define QOL_H

/* --- System Feature Macros --- */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <signal.h>
#include <pthread.h>

/* --- QOL Global Configuration --- */
#ifndef MAX_HTTP_BUF
#define MAX_HTTP_BUF 65536
#endif


/* Source: SimpleBUDP/simple_budp.h */
#ifndef SIMPLE_BUDP_UNIFIED_H
#ifdef BUDP_IMPLEMENTATION
#ifndef TWEETNACL_H
#define crypto_auth_PRIMITIVE "hmacsha512256"
#define crypto_auth crypto_auth_hmacsha512256
#define crypto_auth_verify crypto_auth_hmacsha512256_verify
#define crypto_auth_BYTES crypto_auth_hmacsha512256_BYTES
#define crypto_auth_KEYBYTES crypto_auth_hmacsha512256_KEYBYTES
#define crypto_auth_IMPLEMENTATION crypto_auth_hmacsha512256_IMPLEMENTATION
#define crypto_auth_VERSION crypto_auth_hmacsha512256_VERSION
#define crypto_auth_hmacsha512256_tweet_BYTES 32
#define crypto_auth_hmacsha512256_tweet_KEYBYTES 32
extern int crypto_auth_hmacsha512256_tweet(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_auth_hmacsha512256_tweet_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
#define crypto_auth_hmacsha512256_tweet_VERSION "-"
#define crypto_auth_hmacsha512256 crypto_auth_hmacsha512256_tweet
#define crypto_auth_hmacsha512256_verify crypto_auth_hmacsha512256_tweet_verify
#define crypto_auth_hmacsha512256_BYTES crypto_auth_hmacsha512256_tweet_BYTES
#define crypto_auth_hmacsha512256_KEYBYTES crypto_auth_hmacsha512256_tweet_KEYBYTES
#define crypto_auth_hmacsha512256_VERSION crypto_auth_hmacsha512256_tweet_VERSION
#define crypto_auth_hmacsha512256_IMPLEMENTATION "crypto_auth/hmacsha512256/tweet"
#define crypto_box_PRIMITIVE "curve25519xsalsa20poly1305"
#define crypto_box crypto_box_curve25519xsalsa20poly1305
#define crypto_box_open crypto_box_curve25519xsalsa20poly1305_open
#define crypto_box_keypair crypto_box_curve25519xsalsa20poly1305_keypair
#define crypto_box_beforenm crypto_box_curve25519xsalsa20poly1305_beforenm
#define crypto_box_afternm crypto_box_curve25519xsalsa20poly1305_afternm
#define crypto_box_open_afternm crypto_box_curve25519xsalsa20poly1305_open_afternm
#define crypto_box_PUBLICKEYBYTES crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES
#define crypto_box_SECRETKEYBYTES crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES
#define crypto_box_BEFORENMBYTES crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES
#define crypto_box_NONCEBYTES crypto_box_curve25519xsalsa20poly1305_NONCEBYTES
#define crypto_box_ZEROBYTES crypto_box_curve25519xsalsa20poly1305_ZEROBYTES
#define crypto_box_BOXZEROBYTES crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES
#define crypto_box_IMPLEMENTATION crypto_box_curve25519xsalsa20poly1305_IMPLEMENTATION
#define crypto_box_VERSION crypto_box_curve25519xsalsa20poly1305_VERSION
#define crypto_box_curve25519xsalsa20poly1305_tweet_PUBLICKEYBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_tweet_SECRETKEYBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_tweet_BEFORENMBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_tweet_NONCEBYTES 24
#define crypto_box_curve25519xsalsa20poly1305_tweet_ZEROBYTES 32
#define crypto_box_curve25519xsalsa20poly1305_tweet_BOXZEROBYTES 16
extern int crypto_box_curve25519xsalsa20poly1305_tweet(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tweet_open(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tweet_keypair(unsigned char *,unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tweet_beforenm(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tweet_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_box_curve25519xsalsa20poly1305_tweet_open_afternm(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
#define crypto_box_curve25519xsalsa20poly1305_tweet_VERSION "-"
#define crypto_box_curve25519xsalsa20poly1305 crypto_box_curve25519xsalsa20poly1305_tweet
#define crypto_box_curve25519xsalsa20poly1305_open crypto_box_curve25519xsalsa20poly1305_tweet_open
#define crypto_box_curve25519xsalsa20poly1305_keypair crypto_box_curve25519xsalsa20poly1305_tweet_keypair
#define crypto_box_curve25519xsalsa20poly1305_beforenm crypto_box_curve25519xsalsa20poly1305_tweet_beforenm
#define crypto_box_curve25519xsalsa20poly1305_afternm crypto_box_curve25519xsalsa20poly1305_tweet_afternm
#define crypto_box_curve25519xsalsa20poly1305_open_afternm crypto_box_curve25519xsalsa20poly1305_tweet_open_afternm
#define crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES crypto_box_curve25519xsalsa20poly1305_tweet_PUBLICKEYBYTES
#define crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES crypto_box_curve25519xsalsa20poly1305_tweet_SECRETKEYBYTES
#define crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES crypto_box_curve25519xsalsa20poly1305_tweet_BEFORENMBYTES
#define crypto_box_curve25519xsalsa20poly1305_NONCEBYTES crypto_box_curve25519xsalsa20poly1305_tweet_NONCEBYTES
#define crypto_box_curve25519xsalsa20poly1305_ZEROBYTES crypto_box_curve25519xsalsa20poly1305_tweet_ZEROBYTES
#define crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES crypto_box_curve25519xsalsa20poly1305_tweet_BOXZEROBYTES
#define crypto_box_curve25519xsalsa20poly1305_VERSION crypto_box_curve25519xsalsa20poly1305_tweet_VERSION
#define crypto_box_curve25519xsalsa20poly1305_IMPLEMENTATION "crypto_box/curve25519xsalsa20poly1305/tweet"
#define crypto_core_PRIMITIVE "salsa20"
#define crypto_core crypto_core_salsa20
#define crypto_core_OUTPUTBYTES crypto_core_salsa20_OUTPUTBYTES
#define crypto_core_INPUTBYTES crypto_core_salsa20_INPUTBYTES
#define crypto_core_KEYBYTES crypto_core_salsa20_KEYBYTES
#define crypto_core_CONSTBYTES crypto_core_salsa20_CONSTBYTES
#define crypto_core_IMPLEMENTATION crypto_core_salsa20_IMPLEMENTATION
#define crypto_core_VERSION crypto_core_salsa20_VERSION
#define crypto_core_salsa20_tweet_OUTPUTBYTES 64
#define crypto_core_salsa20_tweet_INPUTBYTES 16
#define crypto_core_salsa20_tweet_KEYBYTES 32
#define crypto_core_salsa20_tweet_CONSTBYTES 16
extern int crypto_core_salsa20_tweet(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);
#define crypto_core_salsa20_tweet_VERSION "-"
#define crypto_core_salsa20 crypto_core_salsa20_tweet
#define crypto_core_salsa20_OUTPUTBYTES crypto_core_salsa20_tweet_OUTPUTBYTES
#define crypto_core_salsa20_INPUTBYTES crypto_core_salsa20_tweet_INPUTBYTES
#define crypto_core_salsa20_KEYBYTES crypto_core_salsa20_tweet_KEYBYTES
#define crypto_core_salsa20_CONSTBYTES crypto_core_salsa20_tweet_CONSTBYTES
#define crypto_core_salsa20_VERSION crypto_core_salsa20_tweet_VERSION
#define crypto_core_salsa20_IMPLEMENTATION "crypto_core/salsa20/tweet"
#define crypto_core_hsalsa20_tweet_OUTPUTBYTES 32
#define crypto_core_hsalsa20_tweet_INPUTBYTES 16
#define crypto_core_hsalsa20_tweet_KEYBYTES 32
#define crypto_core_hsalsa20_tweet_CONSTBYTES 16
extern int crypto_core_hsalsa20_tweet(unsigned char *,const unsigned char *,const unsigned char *,const unsigned char *);
#define crypto_core_hsalsa20_tweet_VERSION "-"
#define crypto_core_hsalsa20 crypto_core_hsalsa20_tweet
#define crypto_core_hsalsa20_OUTPUTBYTES crypto_core_hsalsa20_tweet_OUTPUTBYTES
#define crypto_core_hsalsa20_INPUTBYTES crypto_core_hsalsa20_tweet_INPUTBYTES
#define crypto_core_hsalsa20_KEYBYTES crypto_core_hsalsa20_tweet_KEYBYTES
#define crypto_core_hsalsa20_CONSTBYTES crypto_core_hsalsa20_tweet_CONSTBYTES
#define crypto_core_hsalsa20_VERSION crypto_core_hsalsa20_tweet_VERSION
#define crypto_core_hsalsa20_IMPLEMENTATION "crypto_core/hsalsa20/tweet"
#define crypto_hashblocks_PRIMITIVE "sha512"
#define crypto_hashblocks crypto_hashblocks_sha512
#define crypto_hashblocks_STATEBYTES crypto_hashblocks_sha512_STATEBYTES
#define crypto_hashblocks_BLOCKBYTES crypto_hashblocks_sha512_BLOCKBYTES
#define crypto_hashblocks_IMPLEMENTATION crypto_hashblocks_sha512_IMPLEMENTATION
#define crypto_hashblocks_VERSION crypto_hashblocks_sha512_VERSION
#define crypto_hashblocks_sha512_tweet_STATEBYTES 64
#define crypto_hashblocks_sha512_tweet_BLOCKBYTES 128
extern int crypto_hashblocks_sha512_tweet(unsigned char *,const unsigned char *,unsigned long long);
#define crypto_hashblocks_sha512_tweet_VERSION "-"
#define crypto_hashblocks_sha512 crypto_hashblocks_sha512_tweet
#define crypto_hashblocks_sha512_STATEBYTES crypto_hashblocks_sha512_tweet_STATEBYTES
#define crypto_hashblocks_sha512_BLOCKBYTES crypto_hashblocks_sha512_tweet_BLOCKBYTES
#define crypto_hashblocks_sha512_VERSION crypto_hashblocks_sha512_tweet_VERSION
#define crypto_hashblocks_sha512_IMPLEMENTATION "crypto_hashblocks/sha512/tweet"
#define crypto_hashblocks_sha256_tweet_STATEBYTES 32
#define crypto_hashblocks_sha256_tweet_BLOCKBYTES 64
extern int crypto_hashblocks_sha256_tweet(unsigned char *,const unsigned char *,unsigned long long);
#define crypto_hashblocks_sha256_tweet_VERSION "-"
#define crypto_hashblocks_sha256 crypto_hashblocks_sha256_tweet
#define crypto_hashblocks_sha256_STATEBYTES crypto_hashblocks_sha256_tweet_STATEBYTES
#define crypto_hashblocks_sha256_BLOCKBYTES crypto_hashblocks_sha256_tweet_BLOCKBYTES
#define crypto_hashblocks_sha256_VERSION crypto_hashblocks_sha256_tweet_VERSION
#define crypto_hashblocks_sha256_IMPLEMENTATION "crypto_hashblocks/sha256/tweet"
#define crypto_hash_PRIMITIVE "sha512"
#define crypto_hash crypto_hash_sha512
#define crypto_hash_BYTES crypto_hash_sha512_BYTES
#define crypto_hash_IMPLEMENTATION crypto_hash_sha512_IMPLEMENTATION
#define crypto_hash_VERSION crypto_hash_sha512_VERSION
#define crypto_hash_sha512_tweet_BYTES 64
extern int crypto_hash_sha512_tweet(unsigned char *,const unsigned char *,unsigned long long);
#define crypto_hash_sha512_tweet_VERSION "-"
#define crypto_hash_sha512 crypto_hash_sha512_tweet
#define crypto_hash_sha512_BYTES crypto_hash_sha512_tweet_BYTES
#define crypto_hash_sha512_VERSION crypto_hash_sha512_tweet_VERSION
#define crypto_hash_sha512_IMPLEMENTATION "crypto_hash/sha512/tweet"
#define crypto_hash_sha256_tweet_BYTES 32
extern int crypto_hash_sha256_tweet(unsigned char *,const unsigned char *,unsigned long long);
#define crypto_hash_sha256_tweet_VERSION "-"
#define crypto_hash_sha256 crypto_hash_sha256_tweet
#define crypto_hash_sha256_BYTES crypto_hash_sha256_tweet_BYTES
#define crypto_hash_sha256_VERSION crypto_hash_sha256_tweet_VERSION
#define crypto_hash_sha256_IMPLEMENTATION "crypto_hash/sha256/tweet"
#define crypto_onetimeauth_PRIMITIVE "poly1305"
#define crypto_onetimeauth crypto_onetimeauth_poly1305
#define crypto_onetimeauth_verify crypto_onetimeauth_poly1305_verify
#define crypto_onetimeauth_BYTES crypto_onetimeauth_poly1305_BYTES
#define crypto_onetimeauth_KEYBYTES crypto_onetimeauth_poly1305_KEYBYTES
#define crypto_onetimeauth_IMPLEMENTATION crypto_onetimeauth_poly1305_IMPLEMENTATION
#define crypto_onetimeauth_VERSION crypto_onetimeauth_poly1305_VERSION
#define crypto_onetimeauth_poly1305_tweet_BYTES 16
#define crypto_onetimeauth_poly1305_tweet_KEYBYTES 32
extern int crypto_onetimeauth_poly1305_tweet(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_onetimeauth_poly1305_tweet_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
#define crypto_onetimeauth_poly1305_tweet_VERSION "-"
#define crypto_onetimeauth_poly1305 crypto_onetimeauth_poly1305_tweet
#define crypto_onetimeauth_poly1305_verify crypto_onetimeauth_poly1305_tweet_verify
#define crypto_onetimeauth_poly1305_BYTES crypto_onetimeauth_poly1305_tweet_BYTES
#define crypto_onetimeauth_poly1305_KEYBYTES crypto_onetimeauth_poly1305_tweet_KEYBYTES
#define crypto_onetimeauth_poly1305_VERSION crypto_onetimeauth_poly1305_tweet_VERSION
#define crypto_onetimeauth_poly1305_IMPLEMENTATION "crypto_onetimeauth/poly1305/tweet"
#define crypto_scalarmult_PRIMITIVE "curve25519"
#define crypto_scalarmult crypto_scalarmult_curve25519
#define crypto_scalarmult_base crypto_scalarmult_curve25519_base
#define crypto_scalarmult_BYTES crypto_scalarmult_curve25519_BYTES
#define crypto_scalarmult_SCALARBYTES crypto_scalarmult_curve25519_SCALARBYTES
#define crypto_scalarmult_IMPLEMENTATION crypto_scalarmult_curve25519_IMPLEMENTATION
#define crypto_scalarmult_VERSION crypto_scalarmult_curve25519_VERSION
#define crypto_scalarmult_curve25519_tweet_BYTES 32
#define crypto_scalarmult_curve25519_tweet_SCALARBYTES 32
extern int crypto_scalarmult_curve25519_tweet(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_scalarmult_curve25519_tweet_base(unsigned char *,const unsigned char *);
#define crypto_scalarmult_curve25519_tweet_VERSION "-"
#define crypto_scalarmult_curve25519 crypto_scalarmult_curve25519_tweet
#define crypto_scalarmult_curve25519_base crypto_scalarmult_curve25519_tweet_base
#define crypto_scalarmult_curve25519_BYTES crypto_scalarmult_curve25519_tweet_BYTES
#define crypto_scalarmult_curve25519_SCALARBYTES crypto_scalarmult_curve25519_tweet_SCALARBYTES
#define crypto_scalarmult_curve25519_VERSION crypto_scalarmult_curve25519_tweet_VERSION
#define crypto_scalarmult_curve25519_IMPLEMENTATION "crypto_scalarmult/curve25519/tweet"
#define crypto_secretbox_PRIMITIVE "xsalsa20poly1305"
#define crypto_secretbox crypto_secretbox_xsalsa20poly1305
#define crypto_secretbox_open crypto_secretbox_xsalsa20poly1305_open
#define crypto_secretbox_KEYBYTES crypto_secretbox_xsalsa20poly1305_KEYBYTES
#define crypto_secretbox_NONCEBYTES crypto_secretbox_xsalsa20poly1305_NONCEBYTES
#define crypto_secretbox_ZEROBYTES crypto_secretbox_xsalsa20poly1305_ZEROBYTES
#define crypto_secretbox_BOXZEROBYTES crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES
#define crypto_secretbox_IMPLEMENTATION crypto_secretbox_xsalsa20poly1305_IMPLEMENTATION
#define crypto_secretbox_VERSION crypto_secretbox_xsalsa20poly1305_VERSION
#define crypto_secretbox_xsalsa20poly1305_tweet_KEYBYTES 32
#define crypto_secretbox_xsalsa20poly1305_tweet_NONCEBYTES 24
#define crypto_secretbox_xsalsa20poly1305_tweet_ZEROBYTES 32
#define crypto_secretbox_xsalsa20poly1305_tweet_BOXZEROBYTES 16
extern int crypto_secretbox_xsalsa20poly1305_tweet(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_secretbox_xsalsa20poly1305_tweet_open(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
#define crypto_secretbox_xsalsa20poly1305_tweet_VERSION "-"
#define crypto_secretbox_xsalsa20poly1305 crypto_secretbox_xsalsa20poly1305_tweet
#define crypto_secretbox_xsalsa20poly1305_open crypto_secretbox_xsalsa20poly1305_tweet_open
#define crypto_secretbox_xsalsa20poly1305_KEYBYTES crypto_secretbox_xsalsa20poly1305_tweet_KEYBYTES
#define crypto_secretbox_xsalsa20poly1305_NONCEBYTES crypto_secretbox_xsalsa20poly1305_tweet_NONCEBYTES
#define crypto_secretbox_xsalsa20poly1305_ZEROBYTES crypto_secretbox_xsalsa20poly1305_tweet_ZEROBYTES
#define crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES crypto_secretbox_xsalsa20poly1305_tweet_BOXZEROBYTES
#define crypto_secretbox_xsalsa20poly1305_VERSION crypto_secretbox_xsalsa20poly1305_tweet_VERSION
#define crypto_secretbox_xsalsa20poly1305_IMPLEMENTATION "crypto_secretbox/xsalsa20poly1305/tweet"
#define crypto_sign_PRIMITIVE "ed25519"
#define crypto_sign crypto_sign_ed25519
#define crypto_sign_open crypto_sign_ed25519_open
#define crypto_sign_keypair crypto_sign_ed25519_keypair
#define crypto_sign_BYTES crypto_sign_ed25519_BYTES
#define crypto_sign_PUBLICKEYBYTES crypto_sign_ed25519_PUBLICKEYBYTES
#define crypto_sign_SECRETKEYBYTES crypto_sign_ed25519_SECRETKEYBYTES
#define crypto_sign_IMPLEMENTATION crypto_sign_ed25519_IMPLEMENTATION
#define crypto_sign_VERSION crypto_sign_ed25519_VERSION
#define crypto_sign_ed25519_tweet_BYTES 64
#define crypto_sign_ed25519_tweet_PUBLICKEYBYTES 32
#define crypto_sign_ed25519_tweet_SECRETKEYBYTES 64
extern int crypto_sign_ed25519_tweet(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_tweet_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_tweet_keypair(unsigned char *,unsigned char *);
#define crypto_sign_ed25519_tweet_VERSION "-"
#define crypto_sign_ed25519 crypto_sign_ed25519_tweet
#define crypto_sign_ed25519_open crypto_sign_ed25519_tweet_open
#define crypto_sign_ed25519_keypair crypto_sign_ed25519_tweet_keypair
#define crypto_sign_ed25519_BYTES crypto_sign_ed25519_tweet_BYTES
#define crypto_sign_ed25519_PUBLICKEYBYTES crypto_sign_ed25519_tweet_PUBLICKEYBYTES
#define crypto_sign_ed25519_SECRETKEYBYTES crypto_sign_ed25519_tweet_SECRETKEYBYTES
#define crypto_sign_ed25519_VERSION crypto_sign_ed25519_tweet_VERSION
#define crypto_sign_ed25519_IMPLEMENTATION "crypto_sign/ed25519/tweet"
#define crypto_stream_PRIMITIVE "xsalsa20"
#define crypto_stream crypto_stream_xsalsa20
#define crypto_stream_xor crypto_stream_xsalsa20_xor
#define crypto_stream_KEYBYTES crypto_stream_xsalsa20_KEYBYTES
#define crypto_stream_NONCEBYTES crypto_stream_xsalsa20_NONCEBYTES
#define crypto_stream_IMPLEMENTATION crypto_stream_xsalsa20_IMPLEMENTATION
#define crypto_stream_VERSION crypto_stream_xsalsa20_VERSION
#define crypto_stream_xsalsa20_tweet_KEYBYTES 32
#define crypto_stream_xsalsa20_tweet_NONCEBYTES 24
extern int crypto_stream_xsalsa20_tweet(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_xsalsa20_tweet_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
#define crypto_stream_xsalsa20_tweet_VERSION "-"
#define crypto_stream_xsalsa20 crypto_stream_xsalsa20_tweet
#define crypto_stream_xsalsa20_xor crypto_stream_xsalsa20_tweet_xor
#define crypto_stream_xsalsa20_KEYBYTES crypto_stream_xsalsa20_tweet_KEYBYTES
#define crypto_stream_xsalsa20_NONCEBYTES crypto_stream_xsalsa20_tweet_NONCEBYTES
#define crypto_stream_xsalsa20_VERSION crypto_stream_xsalsa20_tweet_VERSION
#define crypto_stream_xsalsa20_IMPLEMENTATION "crypto_stream/xsalsa20/tweet"
#define crypto_stream_salsa20_tweet_KEYBYTES 32
#define crypto_stream_salsa20_tweet_NONCEBYTES 8
extern int crypto_stream_salsa20_tweet(unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int crypto_stream_salsa20_tweet_xor(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
#define crypto_stream_salsa20_tweet_VERSION "-"
#define crypto_stream_salsa20 crypto_stream_salsa20_tweet
#define crypto_stream_salsa20_xor crypto_stream_salsa20_tweet_xor
#define crypto_stream_salsa20_KEYBYTES crypto_stream_salsa20_tweet_KEYBYTES
#define crypto_stream_salsa20_NONCEBYTES crypto_stream_salsa20_tweet_NONCEBYTES
#define crypto_stream_salsa20_VERSION crypto_stream_salsa20_tweet_VERSION
#define crypto_stream_salsa20_IMPLEMENTATION "crypto_stream/salsa20/tweet"
#define crypto_verify_PRIMITIVE "16"
#define crypto_verify crypto_verify_16
#define crypto_verify_BYTES crypto_verify_16_BYTES
#define crypto_verify_IMPLEMENTATION crypto_verify_16_IMPLEMENTATION
#define crypto_verify_VERSION crypto_verify_16_VERSION
#define crypto_verify_16_tweet_BYTES 16
extern int crypto_verify_16_tweet(const unsigned char *,const unsigned char *);
#define crypto_verify_16_tweet_VERSION "-"
#define crypto_verify_16 crypto_verify_16_tweet
#define crypto_verify_16_BYTES crypto_verify_16_tweet_BYTES
#define crypto_verify_16_VERSION crypto_verify_16_tweet_VERSION
#define crypto_verify_16_IMPLEMENTATION "crypto_verify/16/tweet"
#define crypto_verify_32_tweet_BYTES 32
extern int crypto_verify_32_tweet(const unsigned char *,const unsigned char *);
#define crypto_verify_32_tweet_VERSION "-"
#define crypto_verify_32 crypto_verify_32_tweet
#define crypto_verify_32_BYTES crypto_verify_32_tweet_BYTES
#define crypto_verify_32_VERSION crypto_verify_32_tweet_VERSION
#define crypto_verify_32_IMPLEMENTATION "crypto_verify/32/tweet"
#endif
#define FOR(i,n) for (i = 0;i < n;++i)
#define sv static void

typedef unsigned char u8;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef long long i64;
typedef i64 gf[16];
extern void randombytes(u8 *,u64);

static const u8
  _0[16],
  _9[32] = {9};
static const gf
  gf0,
  gf1 = {1},
  _121665 = {0xDB41,1},
  D = {0x78a3, 0x1359, 0x4dca, 0x75eb, 0xd8ab, 0x4141, 0x0a4d, 0x0070, 0xe898, 0x7779, 0x4079, 0x8cc7, 0xfe73, 0x2b6f, 0x6cee, 0x5203},
  D2 = {0xf159, 0x26b2, 0x9b94, 0xebd6, 0xb156, 0x8283, 0x149a, 0x00e0, 0xd130, 0xeef3, 0x80f2, 0x198e, 0xfce7, 0x56df, 0xd9dc, 0x2406},
  X = {0xd51a, 0x8f25, 0x2d60, 0xc956, 0xa7b2, 0x9525, 0xc760, 0x692c, 0xdc5c, 0xfdd6, 0xe231, 0xc0a4, 0x53fe, 0xcd6e, 0x36d3, 0x2169},
  Y = {0x6658, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666},
  I = {0xa0b0, 0x4a0e, 0x1b27, 0xc4ee, 0xe478, 0xad2f, 0x1806, 0x2f43, 0xd7a7, 0x3dfb, 0x0099, 0x2b4d, 0xdf0b, 0x4fc1, 0x2480, 0x2b83};

static u32 L32(u32 x,int c) { return (x << c) | ((x&0xffffffff) >> (32 - c)); }

static u32 ld32(const u8 *x)
{
  u32 u = x[3];
  u = (u<<8)|x[2];
  u = (u<<8)|x[1];
  return (u<<8)|x[0];
}

static u64 dl64(const u8 *x)
{
  u64 i,u=0;
  FOR(i,8) u=(u<<8)|x[i];
  return u;
}

sv st32(u8 *x,u32 u)
{
  int i;
  FOR(i,4) { x[i] = u; u >>= 8; }
}

sv ts64(u8 *x,u64 u)
{
  int i;
  for (i = 7;i >= 0;--i) { x[i] = u; u >>= 8; }
}

static int vn(const u8 *x,const u8 *y,int n)
{
  u32 i,d = 0;
  FOR(i,n) d |= x[i]^y[i];
  return (1 & ((d - 1) >> 8)) - 1;
}

int crypto_verify_16(const u8 *x,const u8 *y)
{
  return vn(x,y,16);
}

int crypto_verify_32(const u8 *x,const u8 *y)
{
  return vn(x,y,32);
}

sv core(u8 *out,const u8 *in,const u8 *k,const u8 *c,int h)
{
  u32 w[16],x[16],y[16],t[4];
  int i,j,m;

  FOR(i,4) {
    x[5*i] = ld32(c+4*i);
    x[1+i] = ld32(k+4*i);
    x[6+i] = ld32(in+4*i);
    x[11+i] = ld32(k+16+4*i);
  }

  FOR(i,16) y[i] = x[i];

  FOR(i,20) {
    FOR(j,4) {
      FOR(m,4) t[m] = x[(5*j+4*m)%16];
      t[1] ^= L32(t[0]+t[3], 7);
      t[2] ^= L32(t[1]+t[0], 9);
      t[3] ^= L32(t[2]+t[1],13);
      t[0] ^= L32(t[3]+t[2],18);
      FOR(m,4) w[4*j+(j+m)%4] = t[m];
    }
    FOR(m,16) x[m] = w[m];
  }

  if (h) {
    FOR(i,16) x[i] += y[i];
    FOR(i,4) {
      x[5*i] -= ld32(c+4*i);
      x[6+i] -= ld32(in+4*i);
    }
    FOR(i,4) {
      st32(out+4*i,x[5*i]);
      st32(out+16+4*i,x[6+i]);
    }
  } else
    FOR(i,16) st32(out + 4 * i,x[i] + y[i]);
}

int crypto_core_salsa20(u8 *out,const u8 *in,const u8 *k,const u8 *c)
{
  core(out,in,k,c,0);
  return 0;
}

int crypto_core_hsalsa20(u8 *out,const u8 *in,const u8 *k,const u8 *c)
{
  core(out,in,k,c,1);
  return 0;
}

static const u8 sigma[16] = "expand 32-byte k";

int crypto_stream_salsa20_xor(u8 *c,const u8 *m,u64 b,const u8 *n,const u8 *k)
{
  u8 z[16],x[64];
  u32 u,i;
  if (!b) return 0;
  FOR(i,16) z[i] = 0;
  FOR(i,8) z[i] = n[i];
  while (b >= 64) {
    crypto_core_salsa20(x,z,k,sigma);
    FOR(i,64) c[i] = (m?m[i]:0) ^ x[i];
    u = 1;
    for (i = 8;i < 16;++i) {
      u += (u32) z[i];
      z[i] = u;
      u >>= 8;
    }
    b -= 64;
    c += 64;
    if (m) m += 64;
  }
  if (b) {
    crypto_core_salsa20(x,z,k,sigma);
    FOR(i,b) c[i] = (m?m[i]:0) ^ x[i];
  }
  return 0;
}

int crypto_stream_salsa20(u8 *c,u64 d,const u8 *n,const u8 *k)
{
  return crypto_stream_salsa20_xor(c,0,d,n,k);
}

int crypto_stream(u8 *c,u64 d,const u8 *n,const u8 *k)
{
  u8 s[32];
  crypto_core_hsalsa20(s,n,k,sigma);
  return crypto_stream_salsa20(c,d,n+16,s);
}

int crypto_stream_xor(u8 *c,const u8 *m,u64 d,const u8 *n,const u8 *k)
{
  u8 s[32];
  crypto_core_hsalsa20(s,n,k,sigma);
  return crypto_stream_salsa20_xor(c,m,d,n+16,s);
}

sv add1305(u32 *h,const u32 *c)
{
  u32 j,u = 0;
  FOR(j,17) {
    u += h[j] + c[j];
    h[j] = u & 255;
    u >>= 8;
  }
}

static const u32 minusp[17] = {
  5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252
} ;

int crypto_onetimeauth(u8 *out,const u8 *m,u64 n,const u8 *k)
{
  u32 s,i,j,u,x[17],r[17],h[17],c[17],g[17];

  FOR(j,17) r[j]=h[j]=0;
  FOR(j,16) r[j]=k[j];
  r[3]&=15;
  r[4]&=252;
  r[7]&=15;
  r[8]&=252;
  r[11]&=15;
  r[12]&=252;
  r[15]&=15;

  while (n > 0) {
    FOR(j,17) c[j] = 0;
    for (j = 0;(j < 16) && (j < n);++j) c[j] = m[j];
    c[j] = 1;
    m += j; n -= j;
    add1305(h,c);
    FOR(i,17) {
      x[i] = 0;
      FOR(j,17) x[i] += h[j] * ((j <= i) ? r[i - j] : 320 * r[i + 17 - j]);
    }
    FOR(i,17) h[i] = x[i];
    u = 0;
    FOR(j,16) {
      u += h[j];
      h[j] = u & 255;
      u >>= 8;
    }
    u += h[16]; h[16] = u & 3;
    u = 5 * (u >> 2);
    FOR(j,16) {
      u += h[j];
      h[j] = u & 255;
      u >>= 8;
    }
    u += h[16]; h[16] = u;
  }

  FOR(j,17) g[j] = h[j];
  add1305(h,minusp);
  s = -(h[16] >> 7);
  FOR(j,17) h[j] ^= s & (g[j] ^ h[j]);

  FOR(j,16) c[j] = k[j + 16];
  c[16] = 0;
  add1305(h,c);
  FOR(j,16) out[j] = h[j];
  return 0;
}

int crypto_onetimeauth_verify(const u8 *h,const u8 *m,u64 n,const u8 *k)
{
  u8 x[16];
  crypto_onetimeauth(x,m,n,k);
  return crypto_verify_16(h,x);
}

int crypto_secretbox(u8 *c,const u8 *m,u64 d,const u8 *n,const u8 *k)
{
  int i;
  if (d < 32) return -1;
  crypto_stream_xor(c,m,d,n,k);
  crypto_onetimeauth(c + 16,c + 32,d - 32,c);
  FOR(i,16) c[i] = 0;
  return 0;
}

int crypto_secretbox_open(u8 *m,const u8 *c,u64 d,const u8 *n,const u8 *k)
{
  int i;
  u8 x[32];
  if (d < 32) return -1;
  crypto_stream(x,32,n,k);
  if (crypto_onetimeauth_verify(c + 16,c + 32,d - 32,x) != 0) return -1;
  crypto_stream_xor(m,c,d,n,k);
  FOR(i,32) m[i] = 0;
  return 0;
}

sv set25519(gf r, const gf a)
{
  int i;
  FOR(i,16) r[i]=a[i];
}

sv car25519(gf o)
{
  int i;
  i64 c;
  FOR(i,16) {
    o[i]+=(1LL<<16);
    c=o[i]>>16;
    o[(i+1)*(i<15)]+=c-1+37*(c-1)*(i==15);
    o[i]-=c<<16;
  }
}

sv sel25519(gf p,gf q,int b)
{
  i64 t,i,c=~(b-1);
  FOR(i,16) {
    t= c&(p[i]^q[i]);
    p[i]^=t;
    q[i]^=t;
  }
}

sv pack25519(u8 *o,const gf n)
{
  int i,j,b;
  gf m,t;
  FOR(i,16) t[i]=n[i];
  car25519(t);
  car25519(t);
  car25519(t);
  FOR(j,2) {
    m[0]=t[0]-0xffed;
    for(i=1;i<15;i++) {
      m[i]=t[i]-0xffff-((m[i-1]>>16)&1);
      m[i-1]&=0xffff;
    }
    m[15]=t[15]-0x7fff-((m[14]>>16)&1);
    b=(m[15]>>16)&1;
    m[14]&=0xffff;
    sel25519(t,m,1-b);
  }
  FOR(i,16) {
    o[2*i]=t[i]&0xff;
    o[2*i+1]=t[i]>>8;
  }
}

static int neq25519(const gf a, const gf b)
{
  u8 c[32],d[32];
  pack25519(c,a);
  pack25519(d,b);
  return crypto_verify_32(c,d);
}

static u8 par25519(const gf a)
{
  u8 d[32];
  pack25519(d,a);
  return d[0]&1;
}

sv unpack25519(gf o, const u8 *n)
{
  int i;
  FOR(i,16) o[i]=n[2*i]+((i64)n[2*i+1]<<8);
  o[15]&=0x7fff;
}

sv A(gf o,const gf a,const gf b)
{
  int i;
  FOR(i,16) o[i]=a[i]+b[i];
}

sv Z(gf o,const gf a,const gf b)
{
  int i;
  FOR(i,16) o[i]=a[i]-b[i];
}

sv M(gf o,const gf a,const gf b)
{
  i64 i,j,t[31];
  FOR(i,31) t[i]=0;
  FOR(i,16) FOR(j,16) t[i+j]+=a[i]*b[j];
  FOR(i,15) t[i]+=38*t[i+16];
  FOR(i,16) o[i]=t[i];
  car25519(o);
  car25519(o);
}

sv S(gf o,const gf a)
{
  M(o,a,a);
}

sv inv25519(gf o,const gf i)
{
  gf c;
  int a;
  FOR(a,16) c[a]=i[a];
  for(a=253;a>=0;a--) {
    S(c,c);
    if(a!=2&&a!=4) M(c,c,i);
  }
  FOR(a,16) o[a]=c[a];
}

sv pow2523(gf o,const gf i)
{
  gf c;
  int a;
  FOR(a,16) c[a]=i[a];
  for(a=250;a>=0;a--) {
    S(c,c);
    if(a!=1) M(c,c,i);
  }
  FOR(a,16) o[a]=c[a];
}

int crypto_scalarmult(u8 *q,const u8 *n,const u8 *p)
{
  u8 z[32];
  i64 x[80],r,i;
  gf a,b,c,d,e,f;
  FOR(i,31) z[i]=n[i];
  z[31]=(n[31]&127)|64;
  z[0]&=248;
  unpack25519(x,p);
  FOR(i,16) {
    b[i]=x[i];
    d[i]=a[i]=c[i]=0;
  }
  a[0]=d[0]=1;
  for(i=254;i>=0;--i) {
    r=(z[i>>3]>>(i&7))&1;
    sel25519(a,b,r);
    sel25519(c,d,r);
    A(e,a,c);
    Z(a,a,c);
    A(c,b,d);
    Z(b,b,d);
    S(d,e);
    S(f,a);
    M(a,c,a);
    M(c,b,e);
    A(e,a,c);
    Z(a,a,c);
    S(b,a);
    Z(c,d,f);
    M(a,c,_121665);
    A(a,a,d);
    M(c,c,a);
    M(a,d,f);
    M(d,b,x);
    S(b,e);
    sel25519(a,b,r);
    sel25519(c,d,r);
  }
  FOR(i,16) {
    x[i+16]=a[i];
    x[i+32]=c[i];
    x[i+48]=b[i];
    x[i+64]=d[i];
  }
  inv25519(x+32,x+32);
  M(x+16,x+16,x+32);
  pack25519(q,x+16);
  return 0;
}

int crypto_scalarmult_base(u8 *q,const u8 *n)
{ 
  return crypto_scalarmult(q,n,_9);
}

int crypto_box_keypair(u8 *y,u8 *x)
{
  randombytes(x,32);
  return crypto_scalarmult_base(y,x);
}

int crypto_box_beforenm(u8 *k,const u8 *y,const u8 *x)
{
  u8 s[32];
  crypto_scalarmult(s,x,y);
  return crypto_core_hsalsa20(k,_0,s,sigma);
}

int crypto_box_afternm(u8 *c,const u8 *m,u64 d,const u8 *n,const u8 *k)
{
  return crypto_secretbox(c,m,d,n,k);
}

int crypto_box_open_afternm(u8 *m,const u8 *c,u64 d,const u8 *n,const u8 *k)
{
  return crypto_secretbox_open(m,c,d,n,k);
}

int crypto_box(u8 *c,const u8 *m,u64 d,const u8 *n,const u8 *y,const u8 *x)
{
  u8 k[32];
  crypto_box_beforenm(k,y,x);
  return crypto_box_afternm(c,m,d,n,k);
}

int crypto_box_open(u8 *m,const u8 *c,u64 d,const u8 *n,const u8 *y,const u8 *x)
{
  u8 k[32];
  crypto_box_beforenm(k,y,x);
  return crypto_box_open_afternm(m,c,d,n,k);
}

static u64 R(u64 x,int c) { return (x >> c) | (x << (64 - c)); }
static u64 Ch(u64 x,u64 y,u64 z) { return (x & y) ^ (~x & z); }
static u64 Maj(u64 x,u64 y,u64 z) { return (x & y) ^ (x & z) ^ (y & z); }
static u64 Sigma0(u64 x) { return R(x,28) ^ R(x,34) ^ R(x,39); }
static u64 Sigma1(u64 x) { return R(x,14) ^ R(x,18) ^ R(x,41); }
static u64 sigma0(u64 x) { return R(x, 1) ^ R(x, 8) ^ (x >> 7); }
static u64 sigma1(u64 x) { return R(x,19) ^ R(x,61) ^ (x >> 6); }

static const u64 K[80] = 
{
  0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
  0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
  0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
  0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
  0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
  0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
  0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
  0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
  0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
  0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
  0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
  0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
  0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
  0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
  0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
  0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
  0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
  0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
  0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
  0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

int crypto_hashblocks(u8 *x,const u8 *m,u64 n)
{
  u64 z[8],b[8],a[8],w[16],t;
  int i,j;

  FOR(i,8) z[i] = a[i] = dl64(x + 8 * i);

  while (n >= 128) {
    FOR(i,16) w[i] = dl64(m + 8 * i);

    FOR(i,80) {
      FOR(j,8) b[j] = a[j];
      t = a[7] + Sigma1(a[4]) + Ch(a[4],a[5],a[6]) + K[i] + w[i%16];
      b[7] = t + Sigma0(a[0]) + Maj(a[0],a[1],a[2]);
      b[3] += t;
      FOR(j,8) a[(j+1)%8] = b[j];
      if (i%16 == 15)
	FOR(j,16)
	  w[j] += w[(j+9)%16] + sigma0(w[(j+1)%16]) + sigma1(w[(j+14)%16]);
    }

    FOR(i,8) { a[i] += z[i]; z[i] = a[i]; }

    m += 128;
    n -= 128;
  }

  FOR(i,8) ts64(x+8*i,z[i]);

  return n;
}

static const u8 iv[64] = {
  0x6a,0x09,0xe6,0x67,0xf3,0xbc,0xc9,0x08,
  0xbb,0x67,0xae,0x85,0x84,0xca,0xa7,0x3b,
  0x3c,0x6e,0xf3,0x72,0xfe,0x94,0xf8,0x2b,
  0xa5,0x4f,0xf5,0x3a,0x5f,0x1d,0x36,0xf1,
  0x51,0x0e,0x52,0x7f,0xad,0xe6,0x82,0xd1,
  0x9b,0x05,0x68,0x8c,0x2b,0x3e,0x6c,0x1f,
  0x1f,0x83,0xd9,0xab,0xfb,0x41,0xbd,0x6b,
  0x5b,0xe0,0xcd,0x19,0x13,0x7e,0x21,0x79
} ;

int crypto_hash(u8 *out,const u8 *m,u64 n)
{
  u8 h[64],x[256];
  u64 i,b = n;

  FOR(i,64) h[i] = iv[i];

  crypto_hashblocks(h,m,n);
  m += n;
  n &= 127;
  m -= n;

  FOR(i,256) x[i] = 0;
  FOR(i,n) x[i] = m[i];
  x[n] = 128;

  n = 256-128*(n<112);
  x[n-9] = b >> 61;
  ts64(x+n-8,b<<3);
  crypto_hashblocks(h,x,n);

  FOR(i,64) out[i] = h[i];

  return 0;
}

sv add(gf p[4],gf q[4])
{
  gf a,b,c,d,t,e,f,g,h;
  
  Z(a, p[1], p[0]);
  Z(t, q[1], q[0]);
  M(a, a, t);
  A(b, p[0], p[1]);
  A(t, q[0], q[1]);
  M(b, b, t);
  M(c, p[3], q[3]);
  M(c, c, D2);
  M(d, p[2], q[2]);
  A(d, d, d);
  Z(e, b, a);
  Z(f, d, c);
  A(g, d, c);
  A(h, b, a);

  M(p[0], e, f);
  M(p[1], h, g);
  M(p[2], g, f);
  M(p[3], e, h);
}

sv cswap(gf p[4],gf q[4],u8 b)
{
  int i;
  FOR(i,4)
    sel25519(p[i],q[i],b);
}

sv pack(u8 *r,gf p[4])
{
  gf tx, ty, zi;
  inv25519(zi, p[2]); 
  M(tx, p[0], zi);
  M(ty, p[1], zi);
  pack25519(r, ty);
  r[31] ^= par25519(tx) << 7;
}

sv scalarmult(gf p[4],gf q[4],const u8 *s)
{
  int i;
  set25519(p[0],gf0);
  set25519(p[1],gf1);
  set25519(p[2],gf1);
  set25519(p[3],gf0);
  for (i = 255;i >= 0;--i) {
    u8 b = (s[i/8]>>(i&7))&1;
    cswap(p,q,b);
    add(q,p);
    add(p,p);
    cswap(p,q,b);
  }
}

sv scalarbase(gf p[4],const u8 *s)
{
  gf q[4];
  set25519(q[0],X);
  set25519(q[1],Y);
  set25519(q[2],gf1);
  M(q[3],X,Y);
  scalarmult(p,q,s);
}

int crypto_sign_keypair(u8 *pk, u8 *sk)
{
  u8 d[64];
  gf p[4];
  int i;

  randombytes(sk, 32);
  crypto_hash(d, sk, 32);
  d[0] &= 248;
  d[31] &= 127;
  d[31] |= 64;

  scalarbase(p,d);
  pack(pk,p);

  FOR(i,32) sk[32 + i] = pk[i];
  return 0;
}

static const u64 L[32] = {0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x10};

sv modL(u8 *r,i64 x[64])
{
  i64 carry,i,j;
  for (i = 63;i >= 32;--i) {
    carry = 0;
    for (j = i - 32;j < i - 12;++j) {
      x[j] += carry - 16 * x[i] * L[j - (i - 32)];
      carry = (x[j] + 128) >> 8;
      x[j] -= carry << 8;
    }
    x[j] += carry;
    x[i] = 0;
  }
  carry = 0;
  FOR(j,32) {
    x[j] += carry - (x[31] >> 4) * L[j];
    carry = x[j] >> 8;
    x[j] &= 255;
  }
  FOR(j,32) x[j] -= carry * L[j];
  FOR(i,32) {
    x[i+1] += x[i] >> 8;
    r[i] = x[i] & 255;
  }
}

sv reduce(u8 *r)
{
  i64 x[64],i;
  FOR(i,64) x[i] = (u64) r[i];
  FOR(i,64) r[i] = 0;
  modL(r,x);
}

int crypto_sign(u8 *sm,u64 *smlen,const u8 *m,u64 n,const u8 *sk)
{
  u8 d[64],h[64],r[64];
  i64 i,j,x[64];
  gf p[4];

  crypto_hash(d, sk, 32);
  d[0] &= 248;
  d[31] &= 127;
  d[31] |= 64;

  *smlen = n+64;
  FOR(i,n) sm[64 + i] = m[i];
  FOR(i,32) sm[32 + i] = d[32 + i];

  crypto_hash(r, sm+32, n+32);
  reduce(r);
  scalarbase(p,r);
  pack(sm,p);

  FOR(i,32) sm[i+32] = sk[i+32];
  crypto_hash(h,sm,n + 64);
  reduce(h);

  FOR(i,64) x[i] = 0;
  FOR(i,32) x[i] = (u64) r[i];
  FOR(i,32) FOR(j,32) x[i+j] += h[i] * (u64) d[j];
  modL(sm + 32,x);

  return 0;
}

static int unpackneg(gf r[4],const u8 p[32])
{
  gf t, chk, num, den, den2, den4, den6;
  set25519(r[2],gf1);
  unpack25519(r[1],p);
  S(num,r[1]);
  M(den,num,D);
  Z(num,num,r[2]);
  A(den,r[2],den);

  S(den2,den);
  S(den4,den2);
  M(den6,den4,den2);
  M(t,den6,num);
  M(t,t,den);

  pow2523(t,t);
  M(t,t,num);
  M(t,t,den);
  M(t,t,den);
  M(r[0],t,den);

  S(chk,r[0]);
  M(chk,chk,den);
  if (neq25519(chk, num)) M(r[0],r[0],I);

  S(chk,r[0]);
  M(chk,chk,den);
  if (neq25519(chk, num)) return -1;

  if (par25519(r[0]) == (p[31]>>7)) Z(r[0],gf0,r[0]);

  M(r[3],r[0],r[1]);
  return 0;
}

int crypto_sign_open(u8 *m,u64 *mlen,const u8 *sm,u64 n,const u8 *pk)
{
  int i;
  u8 t[32],h[64];
  gf p[4],q[4];

  *mlen = -1;
  if (n < 64) return -1;

  if (unpackneg(q,pk)) return -1;

  FOR(i,n) m[i] = sm[i];
  FOR(i,32) m[i+32] = pk[i];
  crypto_hash(h,m,n);
  reduce(h);
  scalarmult(p,q,h);

  scalarbase(q,sm + 32);
  add(p,q);
  pack(t,p);

  n -= 64;
  if (crypto_verify_32(sm, t)) {
    FOR(i,n) m[i] = 0;
    return -1;
  }

  FOR(i,n) m[i] = sm[i + 64];
  *mlen = n;
  return 0;
}
#endif
#ifdef BUDP_IMPLEMENTATION
#ifndef SIMPLE_TYPES_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// --- Base Types ---
typedef uint8_t u8;   typedef uint16_t u16; typedef uint32_t u32; typedef uint64_t u64;
typedef int8_t s8;    typedef int16_t s16;  typedef int32_t s32;  typedef int64_t s64;

// --- Structural Definitions ---
#define budp_DEF_STRUCTS(BITS, LIMBS) \
    typedef struct { u32 limbs[LIMBS]; } budp_suint##BITS; \
    typedef struct { s32 limbs[LIMBS]; } budp_sint##BITS; \
    typedef struct { budp_suint##BITS mantissa; s32 exponent; } budp_sfloat##BITS;

budp_DEF_STRUCTS(32, 1)    budp_DEF_STRUCTS(64, 2)    budp_DEF_STRUCTS(128, 4)
budp_DEF_STRUCTS(256, 8)   budp_DEF_STRUCTS(512, 16)  budp_DEF_STRUCTS(1024, 32)
budp_DEF_STRUCTS(2048, 64) budp_DEF_STRUCTS(4096, 128) budp_DEF_STRUCTS(8192, 256)
budp_DEF_STRUCTS(12288, 384)

// --- Internal Helpers ---
static inline void _budp_internal_raw_hex(u32* limbs, int count) {
    printf("0x");
    int started = 0;
    for (int i = count - 1; i >= 0; i--) {
        if (limbs[i] != 0 || started || i == 0) {
            if (started) printf("%08X", limbs[i]);
            else printf("%X", limbs[i]);
            started = 1;
        }
    }
}

static inline void _budp_internal_dec_ascii(u32* limbs, int count) {
    u32 temp[count];
    memcpy(temp, limbs, count * sizeof(u32));
    char digits[5000]; 
    int pos = 0, digit_count = 0;
    while (1) {
        u64 remainder = 0;
        int all_zero = 1;
        for (int i = count - 1; i >= 0; i--) {
            u64 val = temp[i] + (remainder << 32);
            temp[i] = (u32)(val / 10);
            remainder = val % 10;
            if (temp[i] > 0) all_zero = 0;
        }
        if (digit_count > 0 && digit_count % 3 == 0) digits[pos++] = ',';
        digits[pos++] = (char)('0' + remainder);
        digit_count++;
        if (all_zero) break;
    }
    for (int i = pos - 1; i >= 0; i--) putchar(digits[i]);
}

// --- Shift Operations ---
#define budp_DEF_SHIFT(BITS, COUNT) \
    static inline void budp_suint##BITS##_shl(budp_suint##BITS *v, int shift) { \
        if (shift <= 0) return; \
        int limb_shift = shift / 32; \
        int bit_shift = shift % 32; \
        if (limb_shift >= COUNT) { memset(v->limbs, 0, sizeof(v->limbs)); return; } \
        for (int i = COUNT - 1; i >= 0; i--) { \
            if (i >= limb_shift) { \
                v->limbs[i] = v->limbs[i - limb_shift] << bit_shift; \
                if (bit_shift > 0 && i - limb_shift - 1 >= 0) \
                    v->limbs[i] |= v->limbs[i - limb_shift - 1] >> (32 - bit_shift); \
            } else v->limbs[i] = 0; \
        } \
    }

budp_DEF_SHIFT(32, 1)    budp_DEF_SHIFT(64, 2)    budp_DEF_SHIFT(128, 4)
budp_DEF_SHIFT(256, 8)   budp_DEF_SHIFT(512, 16)  budp_DEF_SHIFT(1024, 32)
budp_DEF_SHIFT(2048, 64) budp_DEF_SHIFT(4096, 128) budp_DEF_SHIFT(8192, 256)
budp_DEF_SHIFT(12288, 384)

// --- Modulo (Remainder) ---
// This implements res = a % b using a limb-by-limb approach.
// Note: This is a basic implementation. For high-performance 12288-bit 
// division, one would typically use Knuth's Algorithm D.
#define DEF_MOD(BITS, COUNT) \
    static inline void budp_suint##BITS##_mod(budp_suint##BITS *res, budp_suint##BITS a, budp_suint##BITS b) { \
        budp_suint##BITS rem = {0}; \
        /* Check for division by zero */ \
        int is_zero = 1; \
        for(int i = 0; i < COUNT; i++) if(b.limbs[i] != 0) is_zero = 0; \
        if (is_zero) { *res = rem; return; } \
        \
        for (int i = BITS - 1; i >= 0; i--) { \
            /* Shift remainder left by 1 */ \
            u32 carry = 0; \
            for (int j = 0; j < COUNT; j++) { \
                u32 next_carry = rem.limbs[j] >> 31; \
                rem.limbs[j] = (rem.limbs[j] << 1) | carry; \
                carry = next_carry; \
            } \
            /* Bring down the next bit from 'a' */ \
            int limb_idx = i / 32; \
            int bit_idx = i % 32; \
            if ((a.limbs[limb_idx] >> bit_idx) & 1) { \
                rem.limbs[0] |= 1; \
            } \
            /* If rem >= b, then rem = rem - b */ \
            int greater_equal = 1; \
            for (int j = COUNT - 1; j >= 0; j--) { \
                if (rem.limbs[j] < b.limbs[j]) { greater_equal = 0; break; } \
                if (rem.limbs[j] > b.limbs[j]) { greater_equal = 1; break; } \
            } \
            if (greater_equal) { \
                u64 borrow = 0; \
                for (int j = 0; j < COUNT; j++) { \
                    u64 sub = (u64)rem.limbs[j] - b.limbs[j] - borrow; \
                    rem.limbs[j] = (u32)sub; \
                    borrow = (sub >> 63) & 1; \
                } \
            } \
        } \
        *res = rem; \
    }

// Generate Modulo functions for all bit sizes
DEF_MOD(32, 1)      DEF_MOD(64, 2)      DEF_MOD(128, 4)
DEF_MOD(256, 8)     DEF_MOD(512, 16)    DEF_MOD(1024, 32)
DEF_MOD(2048, 64)   DEF_MOD(4096, 128)  DEF_MOD(8192, 256)
DEF_MOD(12288, 384)

// --- Printers ---
#define budp_DEF_PRINTERS(BITS, COUNT) \
    static inline void _budp_nf_u##BITS(budp_suint##BITS v) { _budp_internal_raw_hex(v.limbs, COUNT); } \
    static inline void _budp_f_u##BITS(budp_suint##BITS v)  { _budp_internal_dec_ascii(v.limbs, COUNT); } \
    static inline void _budp_nf_s##BITS(budp_sint##BITS v)  { printf("(S)"); _budp_internal_raw_hex((u32*)v.limbs, COUNT); } \
    static inline void _budp_f_s##BITS(budp_sint##BITS v)   { if(v.limbs[COUNT-1] < 0) printf("-"); _budp_internal_dec_ascii((u32*)v.limbs, COUNT); } \
    static inline void _budp_nf_flt##BITS(budp_sfloat##BITS v) { _budp_internal_raw_hex(v.mantissa.limbs, COUNT); printf(" E%d", v.exponent); } \
    static inline void _budp_f_flt##BITS(budp_sfloat##BITS v)  { _budp_internal_dec_ascii(v.mantissa.limbs, COUNT); printf(" * 2^%d", v.exponent); }

budp_DEF_PRINTERS(32, 1)    budp_DEF_PRINTERS(64, 2)    budp_DEF_PRINTERS(128, 4)
budp_DEF_PRINTERS(256, 8)   budp_DEF_PRINTERS(512, 16)  budp_DEF_PRINTERS(1024, 32)
budp_DEF_PRINTERS(2048, 64) budp_DEF_PRINTERS(4096, 128) budp_DEF_PRINTERS(8192, 256)
budp_DEF_PRINTERS(12288, 384)

#define budp_nfprint(val) _Generic((val), \
    budp_suint32: _budp_nf_u32, budp_suint64: _budp_nf_u64, budp_suint128: _budp_nf_u128, budp_suint256: _budp_nf_u256, budp_suint512: _budp_nf_u512, \
    budp_suint1024: _budp_nf_u1024, budp_suint2048: _budp_nf_u2048, budp_suint4096: _budp_nf_u4096, budp_suint8192: _budp_nf_u8192, budp_suint12288: _budp_nf_u12288, \
    budp_sint32: _budp_nf_s32, budp_sint64: _budp_nf_s64, budp_sint128: _budp_nf_s128, budp_sint256: _budp_nf_s256, budp_sint512: _budp_nf_s512, \
    budp_sint1024: _budp_nf_s1024, budp_sint2048: _budp_nf_s2048, budp_sint4096: _budp_nf_s4096, budp_sint8192: _budp_nf_s8192, budp_sint12288: _budp_nf_s12288, \
    budp_sfloat32: _budp_nf_flt32, budp_sfloat64: _budp_nf_flt64, budp_sfloat128: _budp_nf_flt128, budp_sfloat256: _budp_nf_flt256, budp_sfloat512: _budp_nf_flt512, \
    budp_sfloat1024: _budp_nf_flt1024, budp_sfloat2048: _budp_nf_flt2048, budp_sfloat4096: _budp_nf_flt4096, budp_sfloat8192: _budp_nf_flt8192, budp_sfloat12288: _budp_nf_flt12288 \
)(val)

#define budp_print(val) _Generic((val), \
    budp_suint32: _budp_f_u32, budp_suint64: _budp_f_u64, budp_suint128: _budp_f_u128, budp_suint256: _budp_f_u256, budp_suint512: _budp_f_u512, \
    budp_suint1024: _budp_f_u1024, budp_suint2048: _budp_f_u2048, budp_suint4096: _budp_f_u4096, budp_suint8192: _budp_f_u8192, budp_suint12288: _budp_f_u12288, \
    budp_sint32: _budp_f_s32, budp_sint64: _budp_f_s64, budp_sint128: _budp_f_s128, budp_sint256: _budp_f_s256, budp_sint512: _budp_f_s512, \
    budp_sint1024: _budp_f_s1024, budp_sint2048: _budp_f_s2048, budp_sint4096: _budp_f_s4096, budp_sint8192: _budp_f_s8192, budp_sint12288: _budp_f_s12288, \
    budp_sfloat32: _budp_f_flt32, budp_sfloat64: _budp_f_flt64, budp_sfloat128: _budp_f_flt128, budp_sfloat256: _budp_f_flt256, budp_sfloat512: _budp_f_flt512, \
    budp_sfloat1024: _budp_f_flt1024, budp_sfloat2048: _budp_f_flt2048, budp_sfloat4096: _budp_f_flt4096, budp_sfloat8192: _budp_f_flt8192, budp_sfloat12288: _budp_f_flt12288 \
)(val)
// --- Addition ---
#define budp_DEF_ADD(BITS, COUNT) \
    static inline u32 budp_suint##BITS##_add(budp_suint##BITS *res, budp_suint##BITS a, budp_suint##BITS b) { \
        u64 carry = 0; \
        for (int i = 0; i < COUNT; i++) { \
            u64 sum = (u64)a.limbs[i] + b.limbs[i] + carry; \
            res->limbs[i] = (u32)sum; \
            carry = sum >> 32; \
        } \
        return (u32)carry; \
    }

// --- Multiplication ---
#define budp_DEF_MUL(BITS, COUNT) \
    static inline void budp_suint##BITS##_mul(budp_suint##BITS *res, budp_suint##BITS a, budp_suint##BITS b) { \
        budp_suint##BITS out = {0}; \
        for (int i = 0; i < COUNT; i++) { \
            if (a.limbs[i] == 0) continue; \
            u64 carry = 0; \
            for (int j = 0; i + j < COUNT; j++) { \
                u64 cur = out.limbs[i + j] + (u64)a.limbs[i] * b.limbs[j] + carry; \
                out.limbs[i + j] = (u32)cur; \
                carry = cur >> 32; \
            } \
        } \
        *res = out; \
    }

budp_DEF_ADD(12288, 384)
budp_DEF_MUL(12288, 384)
// Power function: res = base ^ exp
static inline void budp_suint12288_pow(budp_suint12288 *res, u32 base_val, u32 exp) {
    budp_suint12288 b = {0}; b.limbs[0] = base_val;
    budp_suint12288 result = {0}; result.limbs[0] = 1;
    
    for (u32 i = 0; i < exp; i++) {
        budp_suint12288_mul(&result, result, b);
    }
    *res = result;
}

// Tetration: base ^^ height
static inline void budp_suint12288_tetrate(budp_suint12288 *res, u32 base, u32 height) {
    if (height == 0) { res->limbs[0] = 1; return; }
    if (height == 1) { res->limbs[0] = base; return; }
    
    // Starting with base^^2
    budp_suint12288 current_val;
    budp_suint12288_pow(&current_val, base, base);
    
    for (u32 i = 2; i < height; i++) {
        // This is the "Horrid" part. We use the previous result 
        // as the exponent for the next layer.
        // NOTE: We only take the first limb as the exponent here 
        // because budp_suint12288_pow expects a u32 exponent.
        budp_suint12288_pow(&current_val, base, current_val.limbs[0]);
    }
    *res = current_val;
}
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
    #define _WIN32_WINNT 0x0600
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")
    typedef SOCKET net_fd;
    #define b_sleep(s) Sleep((s) * 1000)
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    #include <netdb.h>
    typedef int net_fd;
    #define b_sleep(s) sleep(s)
#endif

// --- TweetNaCl External Declarations ---
extern int crypto_box_keypair(unsigned char *y, unsigned char *x);
extern int crypto_scalarmult(unsigned char *q, const unsigned char *n, const unsigned char *p);
extern int crypto_stream_xor(unsigned char *c, const unsigned char *m, unsigned long long dlen, const unsigned char *n, const unsigned char *k);

#define MAX_B_BUF 65536
#define SLICE_SIZE 1000
#define MAX_B_SESSIONS 150

// Reliability & Handshake Constants
#define BUDP_DATA   0
#define BUDP_SYN    1
#define BUDP_SYNACK 2
#define BUDP_ACK    3
#define BUDP_NACK   4
#define BUDP_FIN   5
typedef enum { B_IDLE, B_CONNECTING, B_WAITING_ACK, B_READY, B_DEAD } BudpState;

#pragma pack(push, 1)
typedef struct {
    uint8_t type;
    uint32_t seq;
    uint16_t slice_idx;
    uint16_t slice_total;
    uint32_t checksum;
    uint32_t data_len;
    char data[SLICE_SIZE];
} BudpPacket;
#pragma pack(pop)

typedef struct {
    net_fd fd;
    BudpState state;
    struct sockaddr_in remote_addr;
    int is_server;
    unsigned char my_priv_key[32];
    unsigned char my_pub_key[32];
    unsigned char peer_pub_key[32];
    unsigned char shared_secret[32];
    int dh_established;
    uint32_t seq_out;
    uint32_t seq_in_expected;
    char assembly_buf[MAX_B_BUF];
    
    // --- Advanced ARQ & Reliability ---
    uint64_t slice_ack_mask;      // Selective Repeat bitmask (up to 64 slices)
    uint32_t last_nack_seq;       // Last processed sequence for NACK
    char reorder_buf[MAX_B_BUF * 4]; // Buffer for out-of-order packets
    
    // --- Timing & Termination ---
    uint64_t last_send_time;
    uint32_t srtt;
    uint32_t rto;
    int is_closing;
} BudpSession;
static BudpSession* _b_sessions = NULL;

void randombytes(unsigned char *buf, unsigned long long len) {
#ifdef _WIN32
    BCryptGenRandom(NULL, buf, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
#else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd != -1) { read(fd, buf, len); close(fd); }
#endif
}

void _b_setup_dh(int id) {
    crypto_box_keypair(_b_sessions[id].my_pub_key, _b_sessions[id].my_priv_key);
}

void _b_compute_shared(int id) {
    crypto_scalarmult(_b_sessions[id].shared_secret, 
                      _b_sessions[id].my_priv_key, 
                      _b_sessions[id].peer_pub_key);
    _b_sessions[id].dh_established = 1;
}

static uint32_t _b_crc32(const void* data, size_t n_bytes) {
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* p = (const uint8_t*)data;
    while (n_bytes--) {
        crc ^= *p++;
        for (int i = 0; i < 8; i++)
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
    }
    return ~crc;
}

void binit(const char* seed) {
    if (_b_sessions) return;
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    _b_sessions = (BudpSession*)calloc(MAX_B_SESSIONS, sizeof(BudpSession));
    for(int i=0; i<MAX_B_SESSIONS; i++) { _b_sessions[i].fd = -1; _b_sessions[i].state = B_IDLE; }
}

int bopen(int id, int port) {
    net_fd fd = socket(AF_INET, SOCK_DGRAM, 0);
#ifdef _WIN32
    unsigned long m = 1; ioctlsocket(fd, FIONBIO, &m);
#else
    fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    
    _b_sessions[id].fd = fd;
    _b_sessions[id].is_server = (port != 0);
    return 1;
}

void _bsend_packet(int id, uint8_t type, const char* data, uint32_t len, uint32_t seq, uint16_t idx, uint16_t total, int encrypt) {
    BudpPacket pkt = {0};
    pkt.type = type; pkt.seq = seq; pkt.slice_idx = idx; pkt.slice_total = total; pkt.data_len = len;
    if (data) memcpy(pkt.data, data, len);
    
    if (encrypt && _b_sessions[id].dh_established) {
        unsigned char nonce[24] = {0};
        memcpy(nonce, &pkt.seq, sizeof(pkt.seq));
        memcpy(nonce + 4, &pkt.slice_idx, sizeof(pkt.slice_idx));
        crypto_stream_xor((unsigned char*)pkt.data, (unsigned char*)pkt.data, pkt.data_len, nonce, _b_sessions[id].shared_secret);
    }
    
    pkt.checksum = _b_crc32(pkt.data, pkt.data_len);
    sendto(_b_sessions[id].fd, (char*)&pkt, sizeof(BudpPacket), 0, (struct sockaddr*)&_b_sessions[id].remote_addr, sizeof(struct sockaddr_in));
}

void bsend(int id, char* data) {
    uint32_t len = (uint32_t)strlen(data);
    uint16_t total = (len / SLICE_SIZE) + 1;
    uint32_t current_seq = _b_sessions[id].seq_out++;
    for (uint16_t i = 0; i < total; i++) {
        uint32_t off = i * SLICE_SIZE;
        uint32_t d_len = (len - off > SLICE_SIZE) ? SLICE_SIZE : (len - off);
        _bsend_packet(id, BUDP_DATA, data + off, d_len, current_seq, i, total, 1);
    }
}

void bconnect(int id, const char* host, int port) {
    struct hostent *hp = gethostbyname(host);
    if (!hp) return;
    _b_sessions[id].remote_addr.sin_family = AF_INET;
    _b_sessions[id].remote_addr.sin_port = htons(port);
    memcpy(&_b_sessions[id].remote_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
    _b_sessions[id].state = B_CONNECTING;
    _b_setup_dh(id);
    _bsend_packet(id, BUDP_SYN, (char*)_b_sessions[id].my_pub_key, 32, 0, 0, 1, 0);
}
void bclose(int id) {
    if (id < 0 || _b_sessions[id].fd == -1) return;
#ifdef _WIN32
    closesocket(_b_sessions[id].fd);
#else
    close(_b_sessions[id].fd);
#endif
    _b_sessions[id].fd = -1;
    _b_sessions[id].state = B_DEAD; // Optional: mark as permanently closed
}
char* bupdate(int id) {
    if (id < 0 || _b_sessions[id].fd == -1) return NULL;
    BudpPacket in;
    struct sockaddr_in src;
    socklen_t slen = sizeof(src);
    int res = recvfrom(_b_sessions[id].fd, (char*)&in, sizeof(BudpPacket), 0, (struct sockaddr*)&src, &slen);
    if (res < (int)sizeof(BudpPacket) || _b_crc32(in.data, in.data_len) != in.checksum) return NULL;

    if (in.type == BUDP_DATA) _bsend_packet(id, BUDP_ACK, NULL, 0, in.seq, in.slice_idx, 1, 0);

    switch(in.type) {
    case BUDP_SYN:
    if (_b_sessions[id].is_server) {
        // Validate against known client alias (e.g., "trusted_client")
        if (verify_peer_key("trusted_client", (unsigned char*)in.data)) {
            _b_sessions[id].remote_addr = src; 
            memcpy(_b_sessions[id].peer_pub_key, in.data, 32);
            _b_setup_dh(id);
            _b_sessions[id].state = B_WAITING_ACK;
            _bsend_packet(id, BUDP_SYNACK, (char*)_b_sessions[id].my_pub_key, 32, 0, 0, 1, 0);
        } else {
            // Identity mismatch: Reject and do not transition state
            return (char*)"UNAUTHORIZED_PEER";
        }
    } break;
        case BUDP_SYNACK:
    if (!_b_sessions[id].is_server && _b_sessions[id].state == B_CONNECTING) {
        // Validate against known server alias
        if (verify_peer_key("trusted_server", (unsigned char*)in.data)) {
            memcpy(_b_sessions[id].peer_pub_key, in.data, 32);
            _b_compute_shared(id);
            _b_sessions[id].state = B_READY;
            _bsend_packet(id, BUDP_ACK, "READY", 5, 0, 0, 1, 0);
            return (char*)"CONNECTED";
        } else {
            return (char*)"UNAUTHORIZED_PEER";
        }
    } break;
        case BUDP_ACK:
            if (_b_sessions[id].is_server && _b_sessions[id].state == B_WAITING_ACK) {
                _b_compute_shared(id);
                _b_sessions[id].state = B_READY;
                return (char*)"CONNECTED";
            } break;
                case BUDP_DATA:
        if (_b_sessions[id].state != B_READY) break;
        if (_b_sessions[id].dh_established) {
            unsigned char nonce[24] = {0};
            memcpy(nonce, &in.seq, sizeof(in.seq));
            memcpy(nonce + 4, &in.slice_idx, sizeof(in.slice_idx));
            crypto_stream_xor((unsigned char*)in.data, (unsigned char*)in.data, in.data_len, nonce, _b_sessions[id].shared_secret);
        }
        memcpy(_b_sessions[id].assembly_buf + (in.slice_idx * SLICE_SIZE), in.data, in.data_len);
        if (in.slice_idx == in.slice_total - 1) {
            _b_sessions[id].a
}
    return NULL;
}
// Define a simple file structure for trusted keys: ~/.local/BUDP/peers/{alias}.pub
int verify_peer_key(const char* alias, const unsigned char* received_key) {
    unsigned char stored_key[32];
    char path[256];
    snprintf(path, sizeof(path), "%s/.local/BUDP/peers/%s.pub", getenv("HOME"), alias);
    
    FILE* f = fopen(path, "rb");
    if (!f) return 0; // Untrusted: No key on file
    
    fread(stored_key, 1, 32, f);
    fclose(f);
    
    // Cryptographically compare keys
    return (memcmp(received_key, stored_key, 32) == 0);
}
#endif
#endif


/* Source: SimpleConflictsFixer/simple_conflicts_fixer.h */
#ifndef SimpleConflictsFixer
#define SimpleConflictsFixer

#ifdef _WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600
#define CloseWindow WinCloseWindow
#define ShowCursor WinShowCursor
#define Rectangle WinRectangle
#define DrawText WinDrawText
#include <winsock2.h>
#include <windows.h>
#include <objbase.h>
#undef CloseWindow
#undef ShowCursor
#undef Rectangle
#undef DrawText
#ifdef interface
#undef interface
#endif
#endif

#endif


/* Source: SimpleFS/simple_fs.h */
#ifndef SIMPLE_FS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* --- Platform Paths & Directory Headers --- */
#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define PATH_SEP '\\'
    #define ALT_SEP '/'
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
    #define PATH_SEP '/'
    #define ALT_SEP '\\'
#endif

/* --- Internal Path Normalizer --- */
static inline void _fs_normalize(char* path) {
    if (!path) return;
    for (int i = 0; path[i]; i++) {
        if (path[i] == ALT_SEP) path[i] = PATH_SEP;
    }
}

/* --- Core Functions --- */

static inline int char_to_int(const void *ptr, int len) {
    uint32_t hash = 2166136261u;
    const unsigned char *p = (const unsigned char *)ptr;
    for (int i = 0; i < len; i++) {
        hash ^= p[i];
        hash *= 16777619u;
    }
    return (int)hash;
}

static inline float char_to_float(const void *ptr, int len) {
    unsigned int seed = (unsigned int)char_to_int(ptr, len);
    return (float)seed / (float)0xFFFFFFFF;
}

static inline long fs_size(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    return size;
}

static inline char* fs_read(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) { fclose(f); return NULL; }
    fread(buffer, 1, size, f);
    buffer[size] = 0;
    fclose(f);
    return buffer;
}

static inline int fs_write(const char* filename, const char* data) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "wb");
    if (!f) return 0;
    fprintf(f, "%s", data);
    fclose(f);
    return 1;
}

static inline int fs_append(const char* filename, const char* data) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "ab");
    if (!f) return 0;
    fprintf(f, "%s", data);
    fclose(f);
    return 1;
}

static inline int fs_delete(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    return remove(clean_path) == 0;
}

static inline int fs_exists(const char* filename) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

/* --- Directory Functions --- */

static inline int fs_mkdir(const char* path) {
    if (!path) return 0;
    char clean_path[256]; strncpy(clean_path, path, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    return mkdir(clean_path, 0777) == 0;
}

static inline int fs_dirremove(const char* path) {
    if (!path) return 0;
    char clean_path[256]; strncpy(clean_path, path, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    char search_path[280];
    sprintf(search_path, "%s\\*", clean_path);
    HANDLE hFind = FindFirstFileA(search_path, &findData);
    if (hFind == INVALID_HANDLE_VALUE) return 0;
    do {
        if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
            char full_path[512];
            sprintf(full_path, "%s\\%s", clean_path, findData.cFileName);
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) fs_dirremove(full_path);
            else DeleteFileA(full_path);
        }
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);
    return RemoveDirectoryA(clean_path) != 0;
#else
    DIR* d = opendir(clean_path);
    if (!d) return 0;
    struct dirent* p;
    while ((p = readdir(d))) {
        if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) continue;
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", clean_path, p->d_name);
        struct stat statbuf;
        if (!stat(full_path, &statbuf)) {
            if (S_ISDIR(statbuf.st_mode)) fs_dirremove(full_path);
            else unlink(full_path);
        }
    }
    closedir(d);
    return rmdir(clean_path) == 0;
#endif
}

/**
 * Returns a piped string "name:type|name:type". 
 * CALLER MUST FREE RESULT.
 */
static inline char* fs_listdir(const char* path) {
    if (!path) return NULL;
    char clean_path[256]; strncpy(clean_path, path, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);

    size_t cap = 1024;
    char* result = (char*)malloc(cap);
    if (!result) return NULL;
    result[0] = '\0';

#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    char search_path[280];
    sprintf(search_path, "%s\\*", clean_path);
    HANDLE hFind = FindFirstFileA(search_path, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                int is_dir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
                size_t needed = strlen(result) + strlen(findData.cFileName) + 10;
                if (needed >= cap) { cap *= 2; result = (char*)realloc(result, cap); }
                if (result[0] != '\0') strcat(result, "|");
                strcat(result, findData.cFileName);
                strcat(result, is_dir ? ":dir" : ":file");
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR* d = opendir(clean_path);
    if (d) {
        struct dirent* p;
        while ((p = readdir(d))) {
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) continue;
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", clean_path, p->d_name);
            struct stat st;
            int is_dir = (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode));
            size_t needed = strlen(result) + strlen(p->d_name) + 10;
            if (needed >= cap) { cap *= 2; result = (char*)realloc(result, cap); }
            if (result[0] != '\0') strcat(result, "|");
            strcat(result, p->d_name);
            strcat(result, is_dir ? ":dir" : ":file");
        }
        closedir(d);
    }
#endif
    return result;
}

/* --- Utility Functions --- */

static inline int fs_count(const char* data, char target) {
    int count = 0;
    for (int i = 0; data[i]; i++) if (data[i] == target) count++;
    return count;
}

static inline char* fs_getline(char* data, int line_num) {
    static char line_buf[512];
    char* cur = data;
    for (int i = 0; i < line_num; i++) {
        cur = strchr(cur, '\n');
        if (!cur) return NULL;
        cur++;
    }
    char* next = strchr(cur, '\n');
    int len = next ? (int)(next - cur) : (int)strlen(cur);
    if (len > 511) len = 511;
    strncpy(line_buf, cur, len);
    line_buf[len] = 0;
    return line_buf;
}

static inline char* fs_readto(const char* filename, int endbyte) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return NULL;
    char* buffer = (char*)malloc(endbyte + 1);
    if (!buffer) { fclose(f); return NULL; }
    int bytesRead = (int)fread(buffer, 1, endbyte, f);
    buffer[bytesRead] = 0;
    fclose(f);
    return buffer;
}

static inline char* fs_readamount(const char* filename, int startbyte, int endbyte) {
    char clean_path[256]; strncpy(clean_path, filename, 255); clean_path[255] = '\0'; _fs_normalize(clean_path);
    FILE* f = fopen(clean_path, "rb");
    if (!f) return NULL;
    fseek(f, startbyte, SEEK_SET);
    int amount = endbyte - startbyte;
    if (amount <= 0) { fclose(f); return NULL; }
    char* buffer = (char*)malloc(amount + 1);
    if (!buffer) { fclose(f); return NULL; }
    int bytesRead = (int)fread(buffer, 1, amount, f);
    buffer[bytesRead] = 0;
    fclose(f);
    return buffer;
}

#endif


/* Source: SimpleHTTP/simple_http.h */
#ifndef SIMPLE_HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Platform Detection & Cross-Platform Sleep --- */
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET http_fd;
    #define usleep(us) Sleep((us)/1000)
    #define sleep(s) Sleep((s)*1000)
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <fcntl.h>
    typedef int http_fd;
    #define INVALID_SOCKET -1
#endif
#if defined(_WIN32) && defined(HTTP_SSL)
/* * FIX: MSYS2/UCRT Linker Compatibility 
 * Provides the vsnprintf symbol expected by some OpenSSL static builds
 * when using vanilla MinGW cross-compilers.
 */
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif
int __cdecl __imp__vsnprintf(char *buffer, size_t count, const char *format, va_list argptr) {
    return vsnprintf(buffer, count, format, argptr);
}
#ifdef __cplusplus
}
#endif
#endif
/* --- Modular SSL (Server Termination) --- */
#ifdef HTTP_SSL
    #include <openssl/ssl.h>
    #include <openssl/err.h>
    static SSL_CTX* _http_ssl_srv_ctx = NULL;
    static int _is_https_srv = 0;
#endif

/* --- Modular libcurl (Client Support) --- */
#ifdef HTTP_CURL
    #include <curl/curl.h>
    static size_t _curl_write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
        size_t total = size * nmemb;
        strncat((char*)userdata, (char*)ptr, total);
        return total;
    }
#endif

#ifndef MAX_HTTP_BUF
#endif

typedef struct {
    char method[10];
    char path[256];
    int client_fd;
    int active;
    int verbose;
#ifdef HTTP_SSL
    SSL* ssl_handle;
#endif
} HttpState;

static HttpState _http_req = {0};
static http_fd _http_server_fd = INVALID_SOCKET;
static char _http_buffer[MAX_HTTP_BUF];

/* --- Internal Init --- */
static inline void _http_init_net() {
#ifdef _WIN32
    static int init = 0; if(!init) { WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa); init=1; }
#endif
}

/* --- [NEW] The libcurl Client --- */
static inline char* http_request(const char* url) {
    memset(_http_buffer, 0, MAX_HTTP_BUF);
#ifdef HTTP_CURL
    CURL *curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, _http_buffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Optional
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        return _http_buffer;
    }
    return "Error: Curl Init Failed";
#else
    return "Error: Define HTTP_CURL to use client functions.";
#endif
}

/* --- [NEW] SSL Server Termination --- */
static inline void http_host_ssl(int port, const char* cert, const char* key) {
#ifdef HTTP_SSL
    _http_init_net();
    SSL_library_init();
    _http_ssl_srv_ctx = SSL_CTX_new(TLS_server_method());
    if (SSL_CTX_use_certificate_chain_file(_http_ssl_srv_ctx, cert) <= 0 ||
        SSL_CTX_use_PrivateKey_file(_http_ssl_srv_ctx, key, SSL_FILETYPE_PEM) <= 0) return;
    _is_https_srv = 1;
    _http_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(port)};
    bind(_http_server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(_http_server_fd, 10);
#endif
}

/* --- Standard Server Logic --- */
static inline void http_host(int port) {
    _http_init_net();
    _http_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(port)};
    bind(_http_server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(_http_server_fd, 10);
}

static inline int http_reqready() {
    if (_http_req.active) return 1;
    struct sockaddr_in caddr;
    socklen_t clen = sizeof(caddr);
    int c_fd = accept(_http_server_fd, (struct sockaddr*)&caddr, &clen);
    if (c_fd >= 0) {
        _http_req.client_fd = c_fd;
#ifdef HTTP_SSL
        if (_is_https_srv) {
            _http_req.ssl_handle = SSL_new(_http_ssl_srv_ctx);
            SSL_set_fd(_http_req.ssl_handle, c_fd);
            SSL_accept(_http_req.ssl_handle);
            SSL_read(_http_req.ssl_handle, _http_buffer, MAX_HTTP_BUF-1);
        } else {
#endif
            recv(c_fd, _http_buffer, MAX_HTTP_BUF-1, 0);
#ifdef HTTP_SSL
        }
#endif
        sscanf(_http_buffer, "%s %s", _http_req.method, _http_req.path);
        _http_req.active = 1;
        return 1;
    }
    return 0;
}

static inline void http_reply(const char* html) {
    char h[512];
    sprintf(h, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", (int)strlen(html));
#ifdef HTTP_SSL
    if (_http_req.ssl_handle) {
        SSL_write(_http_req.ssl_handle, h, (int)strlen(h));
        SSL_write(_http_req.ssl_handle, html, (int)strlen(html));
        SSL_free(_http_req.ssl_handle);
        _http_req.ssl_handle = NULL;
    } else {
#endif
        send(_http_req.client_fd, h, (int)strlen(h), 0);
        send(_http_req.client_fd, html, (int)strlen(html), 0);
#ifdef HTTP_SSL
    }
#endif
#ifdef _WIN32
    closesocket(_http_req.client_fd);
#else
    close(_http_req.client_fd);
#endif
    _http_req.active = 0;
}

static inline int http_is(const char* p) { return (_http_req.active && strcmp(_http_req.path, p) == 0); }

static inline char* http_param(const char* key) {
    static char v[256];
    char s[256]; sprintf(s, "%s=", key);
    char* st = strstr(_http_buffer, s);
    if (!st) return NULL;
    st += strlen(s);
    char* e = strpbrk(st, " &\r\n");
    int len = e ? (int)(e - st) : (int)strlen(st);
    if (len >= 256) len = 255;
    strncpy(v, st, len); v[len] = 0;
    return v;
}

#endif


/* Source: SimpleNet/simple_net.h */
#ifndef SIMPLE_NET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Platform Shims & Headers --- */
#ifdef _WIN32
#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET net_fd;
typedef int socklen_t;
typedef CRITICAL_SECTION net_mutex;
#define net_mutex_init(m) InitializeCriticalSection(m)
#define net_lock(m)      EnterCriticalSection(m)
#define net_unlock(m)    LeaveCriticalSection(m)
#define net_close closesocket
#define net_errno WSAGetLastError()
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
typedef int net_fd;
typedef pthread_mutex_t net_mutex;
#define net_mutex_init(m) pthread_mutex_init(m, NULL)
#define net_lock(m)      pthread_mutex_lock(m)
#define net_unlock(m)    pthread_mutex_unlock(m)
#define net_close close
#define net_errno errno
#define INVALID_SOCKET -1
#endif

/* --- Definitions --- */
#define UDP 0
#define TCP 1
#define MAX_IDS 150
#define MAX_NET_BUF 65536

typedef struct {
    char host[128];
    int port;
} ClientInfo;

typedef struct {
    char* data;
    char host[128];
    int port;
} Packet;

/* --- Internal State --- */
static net_mutex _net_global_lock;
static net_fd _net_fds[MAX_IDS];
static int _net_types[MAX_IDS];
static int _net_active[MAX_IDS];
static ClientInfo _last_client[MAX_IDS];

/* --- Initialization --- */
static inline void net_init() {
    static int init = 0;
    if (init) return;

    net_mutex_init(&_net_global_lock);
    net_lock(&_net_global_lock);

    for(int i = 0; i < MAX_IDS; i++) {
        _net_fds[i] = (net_fd)-1;
        _net_types[i] = 0;
        _net_active[i] = 0;
    }

    #ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    #endif

    init = 1;
    net_unlock(&_net_global_lock);
}

/* --- Core Functions --- */

static inline void closesocket_id(int id) {
    if (id >= 0 && id < MAX_IDS) {
        net_lock(&_net_global_lock);
        if (_net_fds[id] != (net_fd)-1) {
            net_close(_net_fds[id]);
            _net_fds[id] = (net_fd)-1;
            _net_active[id] = 0;
        }
        net_unlock(&_net_global_lock);
    }
}

static inline int isconnected(int id) {
    int connected = 0;
    if (id < 0 || id >= MAX_IDS) return 0;

    net_lock(&_net_global_lock);
    if (_net_fds[id] == (net_fd)-1) {
        net_unlock(&_net_global_lock);
        return 0;
    }
    if (_net_types[id] == UDP || _net_active[id] == 0) {
        net_unlock(&_net_global_lock);
        return 1;
    }

    char temp;
    int res = recv(_net_fds[id], &temp, 1, MSG_PEEK | MSG_DONTWAIT);
    if (res == 0) connected = 0;
    else if (res < 0) {
        #ifdef _WIN32
        connected = (net_errno == WSAEWOULDBLOCK);
        #else
        connected = (errno == EAGAIN || errno == EWOULDBLOCK);
        #endif
    } else {
        connected = 1;
    }

    net_unlock(&_net_global_lock);
    return connected;
}

static inline int opensocket(char* interface, int port, int id, int type) {
    net_init();
    if (id < 0 || id >= MAX_IDS) return 0;
    closesocket_id(id);

    int sock_type = (type == TCP) ? SOCK_STREAM : SOCK_DGRAM;
    net_fd fd = socket(AF_INET, sock_type, 0);
    if (fd == (net_fd)-1) return 0;

    #ifdef _WIN32
    unsigned long m = 1; ioctlsocket(fd, FIONBIO, &m);
    #else
    fcntl(fd, F_SETFL, O_NONBLOCK);
    #endif

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = (interface == NULL) ? INADDR_ANY : inet_addr(interface);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        if (type == TCP || port != 0) { net_close(fd); return 0; }
    }

    if (type == TCP && port != 0) listen(fd, 5);

    net_lock(&_net_global_lock);
    _net_fds[id] = fd;
    _net_types[id] = type;
    _net_active[id] = 0;
    net_unlock(&_net_global_lock);
    return 1;
}

static inline int openudp(char* interface, int port, int id) { return opensocket(interface, port, id, UDP); }
static inline int opentcp(char* interface, int port, int id) { return opensocket(interface, port, id, TCP); }

static inline int connecttcp(int id, char* host, int port) {
    net_init();
    if (id < 0 || id >= MAX_IDS) return 0;
    closesocket_id(id);

    net_fd fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == (net_fd)-1) return 0;

    struct hostent *he = gethostbyname(host);
    if (!he) { net_close(fd); return 0; }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr.s_addr, he->h_addr, he->h_length);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        #ifdef _WIN32
        if (net_errno != WSAEWOULDBLOCK) { net_close(fd); return 0; }
        #else
        if (errno != EINPROGRESS) { net_close(fd); return 0; }
        #endif
    }

    #ifdef _WIN32
    unsigned long m = 1; ioctlsocket(fd, FIONBIO, &m);
    #else
    fcntl(fd, F_SETFL, O_NONBLOCK);
    #endif

    net_lock(&_net_global_lock);
    _net_fds[id] = fd;
    _net_types[id] = TCP;
    _net_active[id] = 1;
    net_unlock(&_net_global_lock);
    return 1;
}

static inline int senddata(int id, char* host, int port, char* data) {
    net_init();
    if (id < 0 || id >= MAX_IDS) return 0;

    net_lock(&_net_global_lock);
    if (_net_fds[id] == (net_fd)-1) {
        net_unlock(&_net_global_lock);
        if (!openudp("0.0.0.0", 0, id)) return 0;
        net_lock(&_net_global_lock);
    }

    struct hostent *he = gethostbyname(host);
    if (!he) { net_unlock(&_net_global_lock); return 0; }

    struct sockaddr_in dest = {0};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    memcpy(&dest.sin_addr.s_addr, he->h_addr, he->h_length);

    int res = -1;
    if (_net_types[id] == TCP) {
        res = send(_net_fds[id], data, (int)strlen(data), 0);
    } else {
        res = sendto(_net_fds[id], data, (int)strlen(data), 0, (struct sockaddr*)&dest, sizeof(dest));
    }
    net_unlock(&_net_global_lock);
    return (res >= 0);
}

/* Updated for thread safety: Pass in a buffer to avoid shared state corruption */
static inline int receivefrom_safe(int id, char* out_buf, int out_size) {
    if (id < 0 || id >= MAX_IDS || out_buf == NULL) return -1;

    net_lock(&_net_global_lock);
    net_fd fd = _net_fds[id];
    if (fd == (net_fd)-1) { net_unlock(&_net_global_lock); return -1; }

    int bytes = -1;
    if (_net_types[id] == TCP) {
        if (_net_active[id] == 1) {
            bytes = recv(fd, out_buf, out_size - 1, 0);
        } else {
            struct sockaddr_in caddr;
            socklen_t clen = sizeof(caddr);
            net_fd n_fd = accept(fd, (struct sockaddr*)&caddr, &clen);
            if (n_fd != (net_fd)-1) {
                #ifdef _WIN32
                unsigned long m = 1; ioctlsocket(n_fd, FIONBIO, &m);
                #else
                fcntl(n_fd, F_SETFL, O_NONBLOCK);
                #endif
                net_close(_net_fds[id]);
                _net_fds[id] = n_fd;
                _net_active[id] = 1;
                strcpy(_last_client[id].host, inet_ntoa(caddr.sin_addr));
                _last_client[id].port = ntohs(caddr.sin_port);
                bytes = recv(_net_fds[id], out_buf, out_size - 1, 0);
            }
        }
    } else {
        struct sockaddr_in src;
        socklen_t src_len = sizeof(src);
        bytes = recvfrom(fd, out_buf, out_size - 1, 0, (struct sockaddr*)&src, &src_len);
        if (bytes > 0) {
            strcpy(_last_client[id].host, inet_ntoa(src.sin_addr));
            _last_client[id].port = ntohs(src.sin_port);
        }
    }

    if (bytes > 0) out_buf[bytes] = 0;
    net_unlock(&_net_global_lock);
    return bytes;
}

static inline ClientInfo getclientinfo(int id) {
    ClientInfo info = {"", 0};
    if (id >= 0 && id < MAX_IDS) {
        net_lock(&_net_global_lock);
        info = _last_client[id];
        net_unlock(&_net_global_lock);
    }
    return info;
}

#endif


/* Source: SimpleParse/simple_parse.h */
#ifndef SIMPLE_PARSE_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef struct {
    int byte;          // Global offset in file
    int lbyte;         // Offset relative to the start of the line (Column)
    int line;          // Line number
    char fullstr[256]; // Match: "spawn_player(hero);"
    char str[256];     // Signature: "spawn_player()"
    char con[256];     // Content: "hero"
} Parsed;

/* --- Internal Helpers --- */

static inline int _sp_count_lines(const char* start, const char* end) {
    int lines = 1;
    for (const char* p = start; p < end; p++) if (*p == '\n') lines++;
    return lines;
}

static inline const char* _sp_get_line_ptr(const char* haystack, int line_num) {
    if (line_num <= 1) return haystack;
    const char* p = haystack;
    int current_line = 1;
    while (*p) {
        if (*p == '\n') {
            current_line++;
            if (current_line == line_num) return p + 1;
        }
        p++;
    }
    return NULL;
}

static inline const char* _sp_find_line_start(const char* haystack, const char* pos) {
    const char* p = pos;
    while (p > haystack && *(p - 1) != '\n') p--;
    return p;
}

/* --- Core Logic --- */

static inline int parse(const char* haystack, const char* needle) {
    return (haystack && needle && strstr(haystack, needle) != NULL);
}

static inline int parseex(const char* input, const char* format, ...) {
    if (!input || !format) return 0;
    va_list args;
    va_start(args, format);
    int items = vsscanf(input, format, args);
    va_end(args);
    return items;
}

static inline Parsed advparse(const char* haystack, const char* needle) {
    Parsed p = { -1, -1, -1, {0}, {0}, {0} };
    if (!haystack || !needle) return p;
    const char* pos = strstr(haystack, needle);
    if (pos) {
        p.byte = (int)(pos - haystack);
        p.line = _sp_count_lines(haystack, pos);
        p.lbyte = (int)(pos - _sp_find_line_start(haystack, pos));
        strncpy(p.fullstr, needle, 255);
    }
    return p;
}

static inline Parsed parsecon(const char* haystack, const char* pattern) {
    Parsed p = { -1, -1, -1, {0}, {0}, {0} };
    char fmt[256]; strncpy(fmt, pattern, 255);
    char* split = strstr(fmt, "%%");
    if (!split) return p;
    *split = '\0';
    char* prefix = fmt; 
    char* suffix = split + 2;

    const char* start_ptr = strstr(haystack, prefix);
    if (!start_ptr) return p;

    const char* val_start = start_ptr + strlen(prefix);
    const char* end_ptr = strstr(val_start, suffix);
    if (!end_ptr) return p;

    p.byte = (int)(start_ptr - haystack);
    p.line = _sp_count_lines(haystack, start_ptr);
    p.lbyte = (int)(start_ptr - _sp_find_line_start(haystack, start_ptr));

    int full_len = (int)((end_ptr + strlen(suffix)) - start_ptr);
    if (full_len > 255) full_len = 255;
    strncpy(p.fullstr, start_ptr, full_len); p.fullstr[full_len] = '\0';
    snprintf(p.str, 255, "%s%s", prefix, suffix);

    int con_len = (int)(end_ptr - val_start);
    if (con_len > 255) con_len = 255;
    strncpy(p.con, val_start, con_len); p.con[con_len] = '\0';

    return p;
}

/* --- Line Logic --- */

static inline int parseline(const char* haystack, int line_num, char* dest, int max_len) {
    const char* line_ptr = _sp_get_line_ptr(haystack, line_num);
    if (!line_ptr) return 0;
    int i = 0;
    while (line_ptr[i] && line_ptr[i] != '\n' && line_ptr[i] != '\r' && i < max_len - 1) {
        dest[i] = line_ptr[i];
        i++;
    }
    dest[i] = '\0';
    return 1;
}

static inline Parsed parselineadv(const char* haystack, int line_num) {
    Parsed p = { -1, 0, line_num, {0}, {0}, {0} };
    const char* line_ptr = _sp_get_line_ptr(haystack, line_num);
    if (line_ptr) {
        p.byte = (int)(line_ptr - haystack);
        parseline(haystack, line_num, p.fullstr, 256);
        strncpy(p.str, p.fullstr, 255);
    }
    return p;
}

static inline Parsed parselinecon(const char* haystack, int line_num, const char* pattern) {
    const char* line_ptr = _sp_get_line_ptr(haystack, line_num);
    if (!line_ptr) { Parsed empty = {-1,-1,-1}; return empty; }
    Parsed p = parsecon(line_ptr, pattern);
    if (p.byte != -1) {
        p.lbyte = p.byte; 
        p.byte += (int)(line_ptr - haystack); 
        p.line = line_num;
    }
    return p;
}

#endif


/* Source: SimpleThreads/simple_threads.h */
#ifndef SIMPLE_THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    typedef HANDLE native_t;
#else
    #define _GNU_SOURCE
    #include <pthread.h>
    #include <unistd.h>
    typedef pthread_t native_t;
#endif

#define MAX_THREADS 256
static native_t _st_pool[MAX_THREADS] = {0};
static void* _st_results[MAX_THREADS] = {0};

typedef struct {
    void* user_fn;
    char* args_str;
    int id;
    int type; 
    int arg_count; 
} _st_pkt;

static char* _st_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* d = malloc(len);
    if (d) memcpy(d, s, len);
    return d;
}

#ifdef _WIN32
static DWORD WINAPI _st_entry(LPVOID p) {
#else
static void* _st_entry(void* p) {
#endif
    _st_pkt* pkt = (_st_pkt*)p;
    int i[4] = {0};
    char s_buf[256] = {0};

    if (pkt->args_str && pkt->args_str[0] == '\"') {
        sscanf(pkt->args_str, "\"%[^\"]\"", s_buf);
    } else if (pkt->args_str) {
        sscanf(pkt->args_str, "%d, %d, %d, %d", &i[0], &i[1], &i[2], &i[3]);
    }

    if (pkt->type == 1) {
        int r = 0;
        if (pkt->arg_count == 0) r = ((int (*)())pkt->user_fn)();
        else if (pkt->arg_count == 1) r = ((int (*)(int))pkt->user_fn)(i[0]);
        else r = ((int (*)(int, int))pkt->user_fn)(i[0], i[1]);
        int* b = malloc(sizeof(int)); *b = r; _st_results[pkt->id] = b;
    } else {
        void* r = NULL;
        if (pkt->arg_count == 0) r = ((void* (*)())pkt->user_fn)();
        else if (pkt->args_str && pkt->args_str[0] == '\"') r = ((void* (*)(char*))pkt->user_fn)(s_buf);
        else if (pkt->arg_count == 1) r = ((void* (*)(int))pkt->user_fn)(i[0]);
        else r = ((void* (*)(int, int))pkt->user_fn)(i[0], i[1]);
        _st_results[pkt->id] = r;
    }
    free(pkt->args_str); free(pkt); return 0;
}

static inline void _st_spawn(void* fn, char* args, int id, int type, int count) {
    if (id < 0 || id >= MAX_THREADS) return;
    _st_pkt* p = malloc(sizeof(_st_pkt));
    p->user_fn = fn; p->args_str = _st_strdup(args);
    p->id = id; p->type = type; p->arg_count = count;
#ifdef _WIN32
    _st_pool[id] = CreateThread(NULL, 0, _st_entry, p, 0, NULL);
#else
    pthread_create(&_st_pool[id], NULL, _st_entry, p);
#endif
}

#define spawnthread(fn, args, id) { \
    int cnt = 0; \
    if (strlen(args) > 0) { cnt = 1; for(int _j=0; args[_j]; _j++) if(args[_j]==',') cnt++; } \
    _st_spawn(fn, args, id, _Generic((fn), \
        int (*)(): 1, int (*)(int): 1, int (*)(int,int): 1, \
        int (*)(int,int,int): 1, int (*)(int,int,int,int): 1, \
        default: 0), cnt); \
}

static inline int isrunning(int id) {
    if (id < 0 || id >= MAX_THREADS || !_st_pool[id]) return 0;
#ifdef _WIN32
    return WaitForSingleObject(_st_pool[id], 0) == WAIT_TIMEOUT;
#else
    return pthread_kill(_st_pool[id], 0) == 0;
#endif
}

static inline void* getreturn(int id) {
    if (id < 0 || id >= MAX_THREADS || !_st_pool[id]) return NULL;
#ifdef _WIN32
    WaitForSingleObject(_st_pool[id], INFINITE); CloseHandle(_st_pool[id]);
#else
    pthread_join(_st_pool[id], NULL);
#endif
    _st_pool[id] = 0; return _st_results[id];
}

static inline void secondsleep(float s) { 
#ifdef _WIN32
    Sleep((int)(s * 1000));
#else
    usleep((int)(s * 1000000));
#endif
}

static inline void killthread(int id) {
    if (id >= 0 && id < MAX_THREADS && _st_pool[id]) {
#ifdef _WIN32
        TerminateThread(_st_pool[id], 0); CloseHandle(_st_pool[id]);
#else
        pthread_cancel(_st_pool[id]);
#endif
        _st_pool[id] = 0;
    }
}

static inline void bombthreads() { for(int i=0; i<MAX_THREADS; i++) killthread(i); }

#endif


/* Source: SimpleTools/simple_tools.h */
#ifndef SIMPLE_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

/* --- Cross-Platform Compatibility --- */

#ifdef _WIN32
    #include <windows.h>
    #define usleep(us) Sleep((us)/1000)
    #define sleep(s) Sleep((s)*1000)
#else
    #include <unistd.h>
#endif

#ifndef MAX_HTTP_BUF
#endif

/* --- Internal String Engine --- */

static inline void hexdmp(void* ptr, int buflen) {
    unsigned char* buf = (unsigned char*)ptr;
    for (int i = 0; i < buflen; i++) {
        // Print the hex value of the byte
        printf("%02x ", buf[i]);

        // Every 16 bytes, start a new line for readability
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");
}

static inline char* _str_replace_logic(const char* src, const char* find, const char* replace, int start_idx, int end_idx) {
    if (!src || !find || !replace) return NULL;
    size_t find_len = strlen(find);
    size_t replace_len = strlen(replace);
    size_t src_len = strlen(src);
    char* result = (char*)malloc(src_len * 10 + 1); 
    char* p_res = result;
    const char* p_src = src;
    int current_occurrence = 0;

    while (*p_src) {
        const char* pos = strstr(p_src, find);
        if (pos) {
            current_occurrence++;
            int should_replace = (current_occurrence >= start_idx && current_occurrence <= end_idx);
            size_t prefix_len = pos - p_src;
            memcpy(p_res, p_src, prefix_len);
            p_res += prefix_len;
            if (should_replace) {
                memcpy(p_res, replace, replace_len);
                p_res += replace_len;
            } else {
                memcpy(p_res, find, find_len);
                p_res += find_len;
            }
            p_src = pos + find_len;
        } else {
            strcpy(p_res, p_src);
            break;
        }
    }
    return result;
}

/* --- String Replacement API --- */

static inline char* str_replace_first(const char* s, const char* f, const char* r) { return _str_replace_logic(s, f, r, 1, 1); }
static inline char* str_replace_all(const char* s, const char* f, const char* r)   { return _str_replace_logic(s, f, r, 1, 999999); }
static inline char* str_replace_nth(const char* s, const char* f, const char* r, int n) { return _str_replace_logic(s, f, r, n, n); }
static inline char* str_replace_count(const char* s, const char* f, const char* r, int c) { return _str_replace_logic(s, f, r, 1, c); }
static inline char* str_replace_range(const char* s, const char* f, const char* r, int s_idx, int e_idx) { return _str_replace_logic(s, f, r, s_idx, e_idx); }

/* --- String Search & Logic --- */

static inline int str_starts(const char* s, const char* prefix) {
    if (!s || !prefix) return 0;
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

static inline int str_ends(const char* s, const char* suffix) {
    if (!s || !suffix) return 0;
    size_t slen = strlen(s);
    size_t flen = strlen(suffix);
    return slen >= flen && strcmp(s + slen - flen, suffix) == 0;
}

static inline int str_contains(const char* s, const char* find) {
    return (s && find) ? strstr(s, find) != NULL : 0;
}

static inline int str_is_numeric(const char* s) {
    if (!s || *s == '\0') return 0;
    while (*s) { if (!isdigit(*s)) return 0; s++; }
    return 1;
}

/* --- String Formatting & Case --- */

static inline void str_lower(char* s) { for(; *s; ++s) *s = tolower(*s); }
static inline void str_upper(char* s) { for(; *s; ++s) *s = toupper(*s); }

static inline char* str_trim(char* s) {
    char* end;
    while(isspace((unsigned char)*s)) s++;
    if(*s == 0) return s;
    end = s + strlen(s) - 1;
    while(end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

static inline char* str_format(const char* fmt, ...) {
    char* res = (char*)malloc(MAX_HTTP_BUF);
    va_list args; va_start(args, fmt);
    vsprintf(res, fmt, args);
    va_end(args);
    return res;
}

static inline char* str_url_decode(const char* src) {
    if (!src) return NULL;
    char *res = (char*)malloc(strlen(src) + 1);
    char *p = res;
    while (*src) {
        if (*src == '%' && isxdigit(src[1]) && isxdigit(src[2])) {
            char hex[3] = { src[1], src[2], 0 };
            *p++ = (char)strtol(hex, NULL, 16);
            src += 3;
        } else if (*src == '+') {
            *p++ = ' '; src++;
        } else {
            *p++ = *src++;
        }
    }
    *p = '\0';
    return res;
}

/* --- Time Tools --- */

static inline char* time_now() {
    static char buf[32];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return buf;
}

static inline long time_unix() { return (long)time(NULL); }

/* --- JSON Builder --- */

static inline char* json_obj(const char* fmt, ...) {
    char* res = (char*)malloc(8192);
    strcpy(res, "{");
    va_list args; va_start(args, fmt);
    char* fmt_copy = strdup(fmt);
    char* token = strtok(fmt_copy, ", ");
    while (token) {
        char type = token[0];
        char* key = strchr(token, ':');
        if (key) {
            key++; strcat(res, "\""); strcat(res, key); strcat(res, "\":");
            if (type == 's') {
                char* val = va_arg(args, char*);
                strcat(res, "\""); strcat(res, val); strcat(res, "\"");
            } else if (type == 'i') {
                sprintf(res + strlen(res), "%d", va_arg(args, int));
            } else if (type == 'f') {
                sprintf(res + strlen(res), "%.2f", va_arg(args, double));
            } else if (type == 'b') {
                strcat(res, va_arg(args, int) ? "true" : "false");
            }
        }
        token = strtok(NULL, ", ");
        if (token) strcat(res, ", ");
    }
    strcat(res, "}");
    free(fmt_copy); va_end(args);
    return res;
}

#endif


/* Source: SimpleTypes/simple_types.h */
#ifndef SIMPLE_TYPES_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// --- Base Types ---
typedef uint8_t u8;   typedef uint16_t u16; typedef uint32_t u32; typedef uint64_t u64;
typedef int8_t s8;    typedef int16_t s16;  typedef int32_t s32;  typedef int64_t s64;

// --- Structural Definitions ---
#define DEF_STRUCTS(BITS, LIMBS) \
    typedef struct { u32 limbs[LIMBS]; } suint##BITS; \
    typedef struct { s32 limbs[LIMBS]; } sint##BITS; \
    typedef struct { suint##BITS mantissa; s32 exponent; } sfloat##BITS;

DEF_STRUCTS(32, 1)    DEF_STRUCTS(64, 2)    DEF_STRUCTS(128, 4)
DEF_STRUCTS(256, 8)   DEF_STRUCTS(512, 16)  DEF_STRUCTS(1024, 32)
DEF_STRUCTS(2048, 64) DEF_STRUCTS(4096, 128) DEF_STRUCTS(8192, 256)
DEF_STRUCTS(12288, 384)

// --- Internal Helpers ---
static inline void _internal_raw_hex(u32* limbs, int count) {
    printf("0x");
    int started = 0;
    for (int i = count - 1; i >= 0; i--) {
        if (limbs[i] != 0 || started || i == 0) {
            if (started) printf("%08X", limbs[i]);
            else printf("%X", limbs[i]);
            started = 1;
        }
    }
}

static inline void _internal_dec_ascii(u32* limbs, int count) {
    u32 temp[count];
    memcpy(temp, limbs, count * sizeof(u32));
    char digits[5000]; 
    int pos = 0, digit_count = 0;
    while (1) {
        u64 remainder = 0;
        int all_zero = 1;
        for (int i = count - 1; i >= 0; i--) {
            u64 val = temp[i] + (remainder << 32);
            temp[i] = (u32)(val / 10);
            remainder = val % 10;
            if (temp[i] > 0) all_zero = 0;
        }
        if (digit_count > 0 && digit_count % 3 == 0) digits[pos++] = ',';
        digits[pos++] = (char)('0' + remainder);
        digit_count++;
        if (all_zero) break;
    }
    for (int i = pos - 1; i >= 0; i--) putchar(digits[i]);
}

// --- Shift Operations ---
#define DEF_SHIFT(BITS, COUNT) \
    static inline void suint##BITS##_shl(suint##BITS *v, int shift) { \
        if (shift <= 0) return; \
        int limb_shift = shift / 32; \
        int bit_shift = shift % 32; \
        if (limb_shift >= COUNT) { memset(v->limbs, 0, sizeof(v->limbs)); return; } \
        for (int i = COUNT - 1; i >= 0; i--) { \
            if (i >= limb_shift) { \
                v->limbs[i] = v->limbs[i - limb_shift] << bit_shift; \
                if (bit_shift > 0 && i - limb_shift - 1 >= 0) \
                    v->limbs[i] |= v->limbs[i - limb_shift - 1] >> (32 - bit_shift); \
            } else v->limbs[i] = 0; \
        } \
    }

DEF_SHIFT(32, 1)    DEF_SHIFT(64, 2)    DEF_SHIFT(128, 4)
DEF_SHIFT(256, 8)   DEF_SHIFT(512, 16)  DEF_SHIFT(1024, 32)
DEF_SHIFT(2048, 64) DEF_SHIFT(4096, 128) DEF_SHIFT(8192, 256)
DEF_SHIFT(12288, 384)

// --- Modulo (Remainder) ---
// This implements res = a % b using a limb-by-limb approach.
// Note: This is a basic implementation. For high-performance 12288-bit 
// division, one would typically use Knuth's Algorithm D.
#define DEF_MOD(BITS, COUNT) \
    static inline void suint##BITS##_mod(suint##BITS *res, suint##BITS a, suint##BITS b) { \
        suint##BITS rem = {0}; \
        /* Check for division by zero */ \
        int is_zero = 1; \
        for(int i = 0; i < COUNT; i++) if(b.limbs[i] != 0) is_zero = 0; \
        if (is_zero) { *res = rem; return; } \
        \
        for (int i = BITS - 1; i >= 0; i--) { \
            /* Shift remainder left by 1 */ \
            u32 carry = 0; \
            for (int j = 0; j < COUNT; j++) { \
                u32 next_carry = rem.limbs[j] >> 31; \
                rem.limbs[j] = (rem.limbs[j] << 1) | carry; \
                carry = next_carry; \
            } \
            /* Bring down the next bit from 'a' */ \
            int limb_idx = i / 32; \
            int bit_idx = i % 32; \
            if ((a.limbs[limb_idx] >> bit_idx) & 1) { \
                rem.limbs[0] |= 1; \
            } \
            /* If rem >= b, then rem = rem - b */ \
            int greater_equal = 1; \
            for (int j = COUNT - 1; j >= 0; j--) { \
                if (rem.limbs[j] < b.limbs[j]) { greater_equal = 0; break; } \
                if (rem.limbs[j] > b.limbs[j]) { greater_equal = 1; break; } \
            } \
            if (greater_equal) { \
                u64 borrow = 0; \
                for (int j = 0; j < COUNT; j++) { \
                    u64 sub = (u64)rem.limbs[j] - b.limbs[j] - borrow; \
                    rem.limbs[j] = (u32)sub; \
                    borrow = (sub >> 63) & 1; \
                } \
            } \
        } \
        *res = rem; \
    }

// Generate Modulo functions for all bit sizes
DEF_MOD(32, 1)      DEF_MOD(64, 2)      DEF_MOD(128, 4)
DEF_MOD(256, 8)     DEF_MOD(512, 16)    DEF_MOD(1024, 32)
DEF_MOD(2048, 64)   DEF_MOD(4096, 128)  DEF_MOD(8192, 256)
DEF_MOD(12288, 384)

// --- Printers ---
#define DEF_PRINTERS(BITS, COUNT) \
    static inline void _nf_u##BITS(suint##BITS v) { _internal_raw_hex(v.limbs, COUNT); } \
    static inline void _f_u##BITS(suint##BITS v)  { _internal_dec_ascii(v.limbs, COUNT); } \
    static inline void _nf_s##BITS(sint##BITS v)  { printf("(S)"); _internal_raw_hex((u32*)v.limbs, COUNT); } \
    static inline void _f_s##BITS(sint##BITS v)   { if(v.limbs[COUNT-1] < 0) printf("-"); _internal_dec_ascii((u32*)v.limbs, COUNT); } \
    static inline void _nf_flt##BITS(sfloat##BITS v) { _internal_raw_hex(v.mantissa.limbs, COUNT); printf(" E%d", v.exponent); } \
    static inline void _f_flt##BITS(sfloat##BITS v)  { _internal_dec_ascii(v.mantissa.limbs, COUNT); printf(" * 2^%d", v.exponent); }

DEF_PRINTERS(32, 1)    DEF_PRINTERS(64, 2)    DEF_PRINTERS(128, 4)
DEF_PRINTERS(256, 8)   DEF_PRINTERS(512, 16)  DEF_PRINTERS(1024, 32)
DEF_PRINTERS(2048, 64) DEF_PRINTERS(4096, 128) DEF_PRINTERS(8192, 256)
DEF_PRINTERS(12288, 384)

#define slibnfprint(val) _Generic((val), \
    suint32: _nf_u32, suint64: _nf_u64, suint128: _nf_u128, suint256: _nf_u256, suint512: _nf_u512, \
    suint1024: _nf_u1024, suint2048: _nf_u2048, suint4096: _nf_u4096, suint8192: _nf_u8192, suint12288: _nf_u12288, \
    sint32: _nf_s32, sint64: _nf_s64, sint128: _nf_s128, sint256: _nf_s256, sint512: _nf_s512, \
    sint1024: _nf_s1024, sint2048: _nf_s2048, sint4096: _nf_s4096, sint8192: _nf_s8192, sint12288: _nf_s12288, \
    sfloat32: _nf_flt32, sfloat64: _nf_flt64, sfloat128: _nf_flt128, sfloat256: _nf_flt256, sfloat512: _nf_flt512, \
    sfloat1024: _nf_flt1024, sfloat2048: _nf_flt2048, sfloat4096: _nf_flt4096, sfloat8192: _nf_flt8192, sfloat12288: _nf_flt12288 \
)(val)

#define slibprint(val) _Generic((val), \
    suint32: _f_u32, suint64: _f_u64, suint128: _f_u128, suint256: _f_u256, suint512: _f_u512, \
    suint1024: _f_u1024, suint2048: _f_u2048, suint4096: _f_u4096, suint8192: _f_u8192, suint12288: _f_u12288, \
    sint32: _f_s32, sint64: _f_s64, sint128: _f_s128, sint256: _f_s256, sint512: _f_s512, \
    sint1024: _f_s1024, sint2048: _f_s2048, sint4096: _f_s4096, sint8192: _f_s8192, sint12288: _f_s12288, \
    sfloat32: _f_flt32, sfloat64: _f_flt64, sfloat128: _f_flt128, sfloat256: _f_flt256, sfloat512: _f_flt512, \
    sfloat1024: _f_flt1024, sfloat2048: _f_flt2048, sfloat4096: _f_flt4096, sfloat8192: _f_flt8192, sfloat12288: _f_flt12288 \
)(val)
// --- Addition ---
#define DEF_ADD(BITS, COUNT) \
    static inline u32 suint##BITS##_add(suint##BITS *res, suint##BITS a, suint##BITS b) { \
        u64 carry = 0; \
        for (int i = 0; i < COUNT; i++) { \
            u64 sum = (u64)a.limbs[i] + b.limbs[i] + carry; \
            res->limbs[i] = (u32)sum; \
            carry = sum >> 32; \
        } \
        return (u32)carry; \
    }

// --- Multiplication ---
#define DEF_MUL(BITS, COUNT) \
    static inline void suint##BITS##_mul(suint##BITS *res, suint##BITS a, suint##BITS b) { \
        suint##BITS out = {0}; \
        for (int i = 0; i < COUNT; i++) { \
            if (a.limbs[i] == 0) continue; \
            u64 carry = 0; \
            for (int j = 0; i + j < COUNT; j++) { \
                u64 cur = out.limbs[i + j] + (u64)a.limbs[i] * b.limbs[j] + carry; \
                out.limbs[i + j] = (u32)cur; \
                carry = cur >> 32; \
            } \
        } \
        *res = out; \
    }

DEF_ADD(12288, 384)
DEF_MUL(12288, 384)
// Power function: res = base ^ exp
static inline void suint12288_pow(suint12288 *res, u32 base_val, u32 exp) {
    suint12288 b = {0}; b.limbs[0] = base_val;
    suint12288 result = {0}; result.limbs[0] = 1;
    
    for (u32 i = 0; i < exp; i++) {
        suint12288_mul(&result, result, b);
    }
    *res = result;
}

// Tetration: base ^^ height
static inline void suint12288_tetrate(suint12288 *res, u32 base, u32 height) {
    if (height == 0) { res->limbs[0] = 1; return; }
    if (height == 1) { res->limbs[0] = base; return; }
    
    // Starting with base^^2
    suint12288 current_val;
    suint12288_pow(&current_val, base, base);
    
    for (u32 i = 2; i < height; i++) {
        // This is the "Horrid" part. We use the previous result 
        // as the exponent for the next layer.
        // NOTE: We only take the first limb as the exponent here 
        // because suint12288_pow expects a u32 exponent.
        suint12288_pow(&current_val, base, current_val.limbs[0]);
    }
    *res = current_val;
}
#endif


#endif /* QOL_H */
