#include<openssl/md5.h>
typedef struct map_node {
	char* key;
	char* value;
} map_node;

//create_table
typedef map_node** table;

table
create_table(unsigned int size) {
	table hashmap = malloc(sizeof(map_node*) * size);
	for(int i=0;i<size;i++) {
		hashmap[i] = malloc(sizeof(map_node));
		hashmap[i]->key = NULL;
		hashmap[i]->value = NULL;
	}
	return hashmap;
}

void
free_table(unsigned int size, table hashmap) {
	for(int i=0;i<size;i++) {
		// Free key
		if(hashmap[i]->key != NULL) {
			free(hashmap[i]->key);
		}
		// Free value
		if(hashmap[i]->value != NULL) {
			free(hashmap[i]->value);
		}
		// Free pointer
		free(hashmap[i]);
	}
	free(hashmap);
}

void
add_value(unsigned int index, const char* key, const char* value, table hashmap) {
	hashmap[index]->key = malloc(strlen(key)+1);
	hashmap[index]->value = malloc(strlen(value)+1);
	strcpy(hashmap[index]->key, key);
	strcpy(hashmap[index]->value, value);
}

void
rem_value(unsigned int index, unsigned int* size, table hashmap) {
	free(hashmap[index]->key);
	free(hashmap[index]->value);
	for(int i=index;i<*size-1;i++) {
		hashmap[i]->key = hashmap[i+1]->key;
		hashmap[i]->value = hashmap[i+1]->value;
	}
	free(hashmap[*size-1]);
	*size -= 1;
}

void
print_table(unsigned int size, table hashmap) {
	for(int i=0;i<size;i++) {
        printf("hashmap[%d] = %p\n", i, hashmap[i]);
        printf("hashmap[%d]->key = %s\n", i, hashmap[i]->key);
        printf("hashmap[%d]->value = %s\n", i, hashmap[i]->value);
	}
}

// Kik specific stuff

void
kik_hashcode_subfunc(char* keybuf, char* rev_keybuf,  long long* array) {
	long long j = 0;
	unsigned char* digest[6];
	unsigned int digest_len;
	char* ptr = keybuf;

	for(int i=0;i<6;i++) {
		// Switch to rev_keybuf
		if(i == 3) {
			ptr = rev_keybuf;
		}
		switch(i % 3) {
			case 0:
				digest[i] = SHA256(ptr, strlen(ptr), NULL);
				digest_len = 32;
				break;
			case 1:
				digest[i] = SHA1(ptr, strlen(ptr), NULL);
				digest_len = 20;
				break;
			case 2:
				digest[i] = MD5(ptr, strlen(ptr), NULL);
				digest_len = 16;
				break;
		}
		for(int x=0;x<digest_len;x+=4) {
			j ^= ((((char)(digest[i][x+3])) << 24 | ((char)(digest[i][x+2])) << 16) | ((char)(digest[i][x+1])) << 8) | (char)(digest[i][i]);
		}
		array[i] = j;
	}
}

// sign_extend_with_mask
long long
sewm(long long x) {
	x &= 4294967295;
    if (x & (1 << 31)) {
    	return x - (1 << 32);
	}
    return x;
}

long long
kik_hashcode(unsigned int size, table hashmap) {
	char keybuf[4096];
	memset(keybuf, 0, 4096);
	for(int i=0;i<size;i++) {
		strcat(keybuf, hashmap[i]->key);
		strcat(keybuf, hashmap[i]->value);
	}
	char rev_keybuf[4096];
	memset(rev_keybuf, 0, 4096);
	for(int i=size-1;i>=0;i--) {
		strcat(rev_keybuf, hashmap[i]->key);
		strcat(rev_keybuf, hashmap[i]->value);
	}

	long long array[6];
	kik_hashcode_subfunc(keybuf, rev_keybuf, array);
	for(int i=0;i<6;i++) {
		printf("array[%d] = %lld\n", i, array[i]);
	} printf("\n");
	unsigned long hashcode_base = -1964139357;
	return (((hashcode_base ^ sewm(array[0] << 7)) ^ sewm(array[5] << 14)) ^ sewm(array[1] << 7)) ^ array[0];
}

char*
sort_kik_map(unsigned int size, table hashmap) {
	char* buffer = malloc(4096);
	strcpy(buffer, "<k");
	int index = 2;
	unsigned int original_size = size;
	for(int i=0;i<original_size;i++) {
		long long hashcode = kik_hashcode(size, hashmap);
		if(hashcode > 0) {
			hashcode %= size;
		} else {
			hashcode = (hashcode * -1) % size;
		}
		index += sprintf(&buffer[index], " %s=\"%s\"", hashmap[hashcode]->key, hashmap[hashcode]->value);
		if(size != 1) {
			rem_value(hashcode, &size, hashmap);
		} else {
			free_table(1, hashmap);
		}
	}
	strcat(buffer, ">");
	return buffer;
}
