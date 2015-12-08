CC=clang
CFLAGS=-std=c11 -g

all: otp_enc_d otp_dec_d otp_enc otp_dec keygen

otp_enc_d: clear otp_d.c
	$(CC) otp_d.c -D CRYPT=0 $(CFLAGS) -o otp_enc_d

otp_dec_d: clear otp_d.c
	$(CC) otp_d.c -D CRYPT=1 $(CFLAGS) -o otp_dec_d

otp_enc: clear otp.c
	$(CC) otp.c -D CRYPT=0 $(CFLAGS) -o otp_enc

otp_dec: clear otp.c
	$(CC) otp.c -D CRYPT=1 $(CFLAGS) -o otp_dec

keygen: clear keygen.c
	$(CC) keygen.c $(CFLAGS) -o keygen

clear:
	clear

clean:
	rm -f otp_enc_d otp_dec_d otp_enc otp_dec keygen ciphertext* plaintext*_a key20 key70000
