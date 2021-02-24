//
// Created by chaos on 2/23/2021.
//

#ifndef RBC_VALIDATOR_VALIDATOR_H
#define RBC_VALIDATOR_VALIDATOR_H

#include <stdlib.h>
#include <openssl/ec.h>

#include "crypto/aes256-ni_enc.h"
#include "iter/uint256_key_iter.h"

#define SEED_SIZE 32

typedef struct aes256_validator_t {
    size_t n;
    unsigned char *curr_cipher;
    const unsigned char *msg, *client_cipher;
} aes256_validator_t;

typedef struct ec_validator_t {
    const EC_GROUP *group;
    const EC_POINT *client_point;
    EC_POINT *curr_point;
    BN_CTX *ctx;
    BIGNUM *scalar;
    int ctx_started;
} ec_validator_t;

int aes256_crypto_func(unsigned char *curr_seed, void *args);
int aes256_crypto_cmp(void *args);

aes256_validator_t *aes256_validator_create(const unsigned char *msg, const unsigned char *client_cipher,
                                            size_t n);
void aes256_validator_destroy(aes256_validator_t *v);

int ec_crypto_func(unsigned char *curr_seed, void *args);
int ec_crypto_cmp(void *args);

ec_validator_t *ec_validator_create(const EC_GROUP *group, const EC_POINT *client_point);
void ec_validator_destroy(ec_validator_t *v);

/// Given a starting permutation, iterate forward through every possible permutation until one that's
/// matching last_perm is found, or until a matching cipher is found.
/// \param client_key An allocated corrupted host_seed to fill if the corrupted host_seed was found.
/// Must be at least 32 bytes big.
/// \param host_seed The original AES host_seed.
/// \param client_cipher The client cipher (16 bytes) to test against.
/// \param userId A uuid_t that's used as the plaintext to encrypt.
/// \param starting_perm The permutation to start iterating from.
/// \param last_perm The final permutation to stop iterating at, inclusively.
/// \param signal A pointer to a shared value. Used to signal the function to prematurely leave.
/// \param all If benchmark mode is set to a non-zero value, then continue even if found.
/// \param validated_keys A counter to keep track of how many keys were traversed. If NULL, then this
/// is skipped.
/// \return Returns a 1 if found or a 0 if not. Returns a -1 if an error has occurred.
int find_matching_seed(unsigned char *client_seed, const unsigned char *host_seed,
                       const uint256_t *starting_perm, const uint256_t *last_perm,
                       int all, long long int *validated_keys,
#ifdef USE_MPI
                       int *signal, int verbose, int my_rank, int nprocs,
#else
        const int* signal,
#endif
                       int (*crypto_func)(unsigned char*, void*), int (*crypto_cmp)(void*),
                       void *crypto_args
);

#endif // RBC_VALIDATOR_VALIDATOR_H