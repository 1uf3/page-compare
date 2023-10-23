#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include <openssl/sha.h>

#define PAGE_SIZE 4096

uint8_t getRandomByte() {
    return (uint8_t)(rand() % 256);
}

double page_xor_cmp() {
    uint8_t* page1 = (uint8_t*)malloc(PAGE_SIZE);
    uint8_t* page2 = (uint8_t*)malloc(PAGE_SIZE);

    if (!page1 || !page2) {
        perror("Memory allocation failed");
        return 1;
    }

    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < PAGE_SIZE; ++i) {
        uint8_t randomByte = getRandomByte();
        page1[i] = randomByte;
        page2[i] = randomByte;
    }

    // benchmark start
    clock_t start, end;
    double cpu_time_used;
    start = clock(); 

    int result = 0;

    for (size_t i = 0; i < PAGE_SIZE; ++i) {
        result = page1[i] ^ page2[i];
    }

    if (!result) {
        printf("XOR result is all zeros.\n");
    } else {
        printf("XOR result is not all zeros.\n");
    }

    // benchmark stop
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    free(page1);
    free(page2);

    return cpu_time_used;
}

double page_memcmp() {
    uint8_t* page1 = (uint8_t*)malloc(PAGE_SIZE);
    uint8_t* page2 = (uint8_t*)malloc(PAGE_SIZE);

    if (!page1 || !page2) {
        perror("Memory allocation failed");
        return 1;
    }

    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < PAGE_SIZE; ++i) {
        uint8_t randomByte = getRandomByte();
        page1[i] = randomByte;
        page2[i] = randomByte;
    }

    // benchmark start
    clock_t start, end;
    double cpu_time_used;
    start = clock(); 
    
    int result = memcmp(page1, page2, SHA256_DIGEST_LENGTH);
    if (result == 0) {
        printf("Hash values match. Data is unchanged.\n");
    } else {
        printf("Hash values do not match. Data may have been modified.\n");
    }

    // benchmark stop
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    free(page1);
    free(page2);

    return cpu_time_used;
}

double page_hash_memcmp() {
    uint8_t* page1 = (uint8_t*)malloc(PAGE_SIZE);
    uint8_t* page2 = (uint8_t*)malloc(PAGE_SIZE);
    
    if (!page1 || !page2) {
        perror("Memory allocation failed");
        return 1;
    }

    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < PAGE_SIZE; ++i) {
        uint8_t randomByte = getRandomByte();
        page1[i] = randomByte;
        page2[i] = randomByte;
    }

    unsigned char hash1[SHA256_DIGEST_LENGTH];
    SHA256(page1, PAGE_SIZE, hash1);

    // benchmark start
    clock_t start, end;
    double cpu_time_used;
    start = clock(); 

    unsigned char hash2[SHA256_DIGEST_LENGTH];
    SHA256(page2, PAGE_SIZE, hash2);

    int result = memcmp(hash1, hash2, SHA256_DIGEST_LENGTH);
    if (result == 0) {
        printf("Hash values match. Data is unchanged.\n");
    } else {
        printf("Hash values do not match. Data may have been modified.\n");
    }

    // benchmark stop
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    free(page1);
    free(page2);

    return cpu_time_used;
}

int main() {
    int numIterations = 100000; 
    double result[3];

    for(int i = 0; i < numIterations; i++) {
        double pxc = page_xor_cmp();
        double pmc = page_memcmp();
        double phmc = page_hash_memcmp();
        result[0] += pxc;
        result[1] += pmc;
        result[2] += phmc;
    }


    puts("=================================================");
    printf("page_xor_cmp() Execution Time: %fs\n", result[0]/ numIterations);
    puts("=================================================");
    printf("page_memcmp() Execution Time: %fs\n", result[1]/ numIterations);
    puts("=================================================");
    printf("page_hash_memcmp() Execution Time: %fs\n", result[2]/ numIterations);
    puts("=================================================");

}
