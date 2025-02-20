


//https://nachtimwald.com/2017/09/24/hex-encode-and-decode-in-c/


/*

char *bin2hex(const unsigned char *bin, size_t len)
{
	char   *out;
	size_t  i;

	if (bin == NULL || len == 0)
		return NULL;

	out = malloc(len*2+1);
	for (i=0; i<len; i++) {
		out[i*2]   = "0123456789ABCDEF"[bin[i] >> 4];
		out[i*2+1] = "0123456789ABCDEF"[bin[i] & 0x0F];
	}
	out[len*2] = '\0';

	return out;
}




int hexchr2bin(const char hex, char *out)
{
	if (out == NULL)
		return 0;

	if (hex >= '0' && hex <= '9') {
		*out = hex - '0';
	} else if (hex >= 'A' && hex <= 'F') {
		*out = hex - 'A' + 10;
	} else if (hex >= 'a' && hex <= 'f') {
		*out = hex - 'a' + 10;
	} else {
		return 0;
	}

	return 1;
}


size_t hexs2bin(const char *hex, unsigned char **out)
{
	size_t len;
	char   b1;
	char   b2;
	size_t i;

	if (hex == NULL || *hex == '\0' || out == NULL)
		return 0;

	len = strlen(hex);
	if (len % 2 != 0)
		return 0;
	len /= 2;

	*out = malloc(len);
	memset(*out, 'A', len);
	for (i=0; i<len; i++) {
		if (!hexchr2bin(hex[i*2], &b1) || !hexchr2bin(hex[i*2+1], &b2)) {
			return 0;
		}
		(*out)[i] = (b1 << 4) | b2;
	}
	return len;
}


int main(int argc, char **argv)
{
	const char    *a = "Test 123! - jklmn";
	char          *hex;
	unsigned char *bin;
	size_t         binlen;

	hex = bin2hex((unsigned char *)a, strlen(a));
	printf("%sn", hex);

	binlen = hexs2bin(hex, &bin);
	printf("%.*sn", (int)binlen, (char *)bin);

	free(bin);
	free(hex);
	return 0;
}


*/


/*

void hex(unsigned char a, char* buf) {
    // hex lookup table
    char data[] = "0123456789ABCDEF";
    buf[0] = '0';
    buf[1] = 'x';
    int i = 2;
    while (a) {
        buf[i ++] = data[a % 16];
        a /= 16;
    }    
    int j = 2;
    -- i;
    // reverse i..j
    while (j < i) {
        char t = buf[j];
        buf[j] = buf[i];
        buf[i] = t;
        i --;
        j ++;
    }
}


int main(int argc, char* argv[]) {
    if (argc == 0) {
        return 0;
    }
    for (int i = 1; i < argc; ++ i) {
        for (int j = 0; j < strlen(argv[i]); ++ j) {
            char buf[4];
            unsigned char cur = argv[i][j] & 0xFF;
            hex(cur, buf);
            printf("\"%s\", ", buf);
        }
    }
    printf("\n");
    return 0;
}
*/

// https://helloacm.com/c-program-to-convert-characters-to-hex-c-source-text/