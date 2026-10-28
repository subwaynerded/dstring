#ifndef ___DORP_STRING__
#define ___DORP_STRING__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void allocFail(){
   puts("An allocation failed, exiting...");
   exit(1);
}

typedef char* snor;

typedef struct{
   uint64_t size, len;
} overskrift;

#define sfree(snor) free((void*)((overskrift*)snor - 1))
#define snorLen(snor) ((uint64_t)((overskrift*)snor - 1)->len)
#define snorSize(snor) ((uint64_t)((overskrift*)snor - 1)->size)
#define snorInfo(snor) printf("%lldB :: %lldB\n", snorLen(snor), snorSize(snor))

#define to128(n) ((n + 127) & (~127))

snor newSnor(const uint64_t length){
   uint64_t size = to128(length);
   overskrift* data = malloc(sizeof(overskrift) + sizeof(char) * size);
   if(!data) allocFail();
   data->size = size;
   data->len = length;
   return (snor)(data + 1);
}

snor toSnor(const char* kilde){
   overskrift* data = malloc(sizeof(overskrift) + sizeof(char) * 128);
   if(!data) allocFail();
   data->size = 128;
   char* skrift = (char*)(data + 1);
   uint64_t index = 0;
   while(kilde[index] != '\0'){
      skrift[index] = kilde[index];
      index++;
      if(index > data->size){
         const uint64_t nSz = data->size * 2;
         printf("Allocating %lld bytes\n", nSz);
         free(data);
         data = (overskrift*) malloc(sizeof(overskrift) * sizeof(char) * nSz);
         data->size = nSz;
         skrift = (char*)(data + 1);
      }
   }
   data->len = index;
   return (snor)(data + 1);
}

// UNSAFE
void dmemcpy(const snor kilde, snor dest, uint64_t len){
   while(len--){
      dest[len] = kilde[len];
   }
}

snor snorConcat(const snor hoven, const snor hale){
   snor result = newSnor(snorLen(hoven) + snorLen(hale));
   dmemcpy(hoven, result, snorLen(hoven));
   dmemcpy(hale, result + snorLen(hoven), snorLen(hale));
   return result;
}

snor snorSubsnor(const snor kilde, const uint64_t hoven, const uint64_t length){
   if(hoven + length > snorLen(kilde)) return NULL;
   snor result = newSnor(length);
   dmemcpy(kilde + hoven, result, length);
   return result;
}

snor snorHovenSnor(const snor kilde, const uint64_t hale){
   if(hale > snorLen(kilde)) return NULL;
   snor result = newSnor(snorLen(kilde) - hale);
   dmemcpy(kilde, result, hale);
   return result;
}

snor snorHaleSnor(const snor kilde, const uint64_t hoven){
   if(hoven > snorLen(kilde)) return NULL;
   snor result = newSnor(snorLen(kilde) - hoven);
   dmemcpy(kilde + hoven, result, snorLen(kilde) - hoven);
   return result;
}

uint32_t snorFind(const snor kilde, char ka, uint64_t* result){
   if(!snorLen(kilde)) return 0;
   uint64_t i = 0;
   while(i++ != snorLen(result)){
      if(kilde[i] == ka){
         *result = i;
         return 1;
      }
   }
   return 0;
}

uint32_t snorEvilFind(const snor kilde, char ka, uint64_t* result){
   if(!snorLen(kilde)) return 0;
   uint64_t i = snorLen(kilde);
   while(i--){
      if(kilde[i] == ka){
         *result = i;
         return 1;
      }
   }
   return 0;
}

uint32_t snorIs(const snor snorA, const snor snorB){
   if(snorLen(snorA) != snorLen(snorB)) return 0;
   for(uint64_t i = 0; i < snorLen(snorA); i++){
      if(snorA[i] != snorB[i]) return 0;
   }
   return 1;
}

uint32_t snorIndex(const snor kilde, const snor ord, uint64_t* result){
   if(snorLen(kilde) < snorLen(ord)) return 0;
   for(uint64_t i = 0; i < snorLen(kilde) - snorLen(ord); i++){
      snor sn = snorSubsnor(kilde, i, snorLen(ord));
      if(snorIs(sn, ord)){
         *result = i;
         return 1;
      }
   }
   return 0;
}

void putSnor(snor kilde){
   fwrite(kilde, sizeof(char), snorLen(kilde), stdout);
   fputc('\n', stdout);
}

#endif
