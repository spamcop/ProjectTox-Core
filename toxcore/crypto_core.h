/* crypto_core.h
 *
 * Functions for the core crypto.
 *
 *  Copyright (C) 2013 Tox project All Rights Reserved.
 *
 *  This file is part of Tox.
 *
 *  Tox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Tox is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Tox.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef CORE_CRYPTO_H
#define CORE_CRYPTO_H

#include "network.h"

#ifndef VANILLA_NACL
/* We use libsodium by default. */
#include <sodium.h>
#else
#include <crypto_box.h>
#include <randombytes.h>
#include <crypto_hash_sha256.h>
#include <crypto_hash_sha512.h>
#include <crypto_verify_16.h>
#include <crypto_verify_32.h>
#define crypto_box_MACBYTES (crypto_box_ZEROBYTES - crypto_box_BOXZEROBYTES)
#endif

#define crypto_box_KEYBYTES (crypto_box_BEFORENMBYTES)

/* Use this instead of memcmp; not vulnerable to timing attacks.
   returns 0 if both mem locations of length are equal,
   return -1 if they are not. */
int crypto_cmp(uint8_t *mem1, uint8_t *mem2, uint32_t length);

/*  return a random number.
 *
 * random_int for a 32bin int.
 * random_64b for a 64bit int.
 */
uint32_t random_int(void);
uint64_t random_64b(void);


/* Encrypts plain of length length to encrypted of length + 16 using the
 * public key(32 bytes) of the receiver and the secret key of the sender and a 24 byte nonce.
 *
 *  return -1 if there was a problem.
 *  return length of encrypted data if everything was fine.
 */
int encrypt_data(uint8_t *public_key, uint8_t *secret_key, uint8_t *nonce,
                 uint8_t *plain, uint32_t length, uint8_t *encrypted);


/* Decrypts encrypted of length length to plain of length length - 16 using the
 * public key(32 bytes) of the sender, the secret key of the receiver and a 24 byte nonce.
 *
 *  return -1 if there was a problem (decryption failed).
 *  return length of plain data if everything was fine.
 */
int decrypt_data(uint8_t *public_key, uint8_t *secret_key, uint8_t *nonce,
                 uint8_t *encrypted, uint32_t length, uint8_t *plain);

/* Fast encrypt/decrypt operations. Use if this is not a one-time communication.
   encrypt_precompute does the shared-key generation once so it does not have
   to be preformed on every encrypt/decrypt. */
void encrypt_precompute(uint8_t *public_key, uint8_t *secret_key, uint8_t *enc_key);

/* Encrypts plain of length length to encrypted of length + 16 using a
 * secret key crypto_box_KEYBYTES big and a 24 byte nonce.
 *
 *  return -1 if there was a problem.
 *  return length of encrypted data if everything was fine.
 */
int encrypt_data_symmetric(uint8_t *secret_key, uint8_t *nonce, uint8_t *plain, uint32_t length, uint8_t *encrypted);

/* Decrypts encrypted of length length to plain of length length - 16 using a
 * secret key crypto_box_KEYBYTES big and a 24 byte nonce.
 *
 *  return -1 if there was a problem (decryption failed).
 *  return length of plain data if everything was fine.
 */
int decrypt_data_symmetric(uint8_t *secret_key, uint8_t *nonce, uint8_t *encrypted, uint32_t length, uint8_t *plain);

/* Increment the given nonce by 1. */
void increment_nonce(uint8_t *nonce);

/* increment the given nonce by num */
void increment_nonce_number(uint8_t *nonce, uint32_t num);

/* Fill the given nonce with random bytes. */
void random_nonce(uint8_t *nonce);

/* Fill a key crypto_box_KEYBYTES big with random bytes */
void new_symmetric_key(uint8_t *key);

/*Gives a nonce guaranteed to be different from previous ones.*/
void new_nonce(uint8_t *nonce);

#define MAX_CRYPTO_REQUEST_SIZE 1024

#define CRYPTO_PACKET_FRIEND_REQ    32  /* Friend request crypto packet ID. */
#define CRYPTO_PACKET_HARDENING     48  /* Hardening crypto packet ID. */
#define CRYPTO_PACKET_NAT_PING      254 /* NAT ping crypto packet ID. */
#define CRYPTO_PACKET_GROUP_CHAT_GET_NODES      48 /* Group chat get Nodes packet */
#define CRYPTO_PACKET_GROUP_CHAT_SEND_NODES     49 /* Group chat send Nodes packet */
#define CRYPTO_PACKET_GROUP_CHAT_BROADCAST      50 /* Group chat broadcast packet */

/* Create a request to peer.
 * send_public_key and send_secret_key are the pub/secret keys of the sender.
 * recv_public_key is public key of reciever.
 * packet must be an array of MAX_CRYPTO_REQUEST_SIZE big.
 * Data represents the data we send with the request with length being the length of the data.
 * request_id is the id of the request (32 = friend request, 254 = ping request).
 *
 * return -1 on failure.
 * return the length of the created packet on success.
 */
int create_request(uint8_t *send_public_key, uint8_t *send_secret_key, uint8_t *packet, uint8_t *recv_public_key,
                   uint8_t *data, uint32_t length, uint8_t request_id);

/* puts the senders public key in the request in public_key, the data from the request
   in data if a friend or ping request was sent to us and returns the length of the data.
   packet is the request packet and length is its length
   return -1 if not valid request. */
int handle_request(uint8_t *self_public_key, uint8_t *self_secret_key, uint8_t *public_key, uint8_t *data,
                   uint8_t *request_id, uint8_t *packet, uint16_t length);


#endif
