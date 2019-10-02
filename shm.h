//
// Created by wolle on 02.10.19.
//

#ifndef TESTSHM_SHM_H
#define TESTSHM_SHM_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

template <typename T>
static T* initShm(const std::string& name, bool create) {
    int flags = create ? O_CREAT | O_EXCL | O_RDWR : O_RDWR;
    int shm_fd = shm_open(name.c_str(), flags, 0700);
    if (shm_fd == -1) {
        std::cerr << "Error shm_open " << errno << " (" << strerror(errno)
                  << ")" << std::endl;
        return nullptr;
    }
    if (create) {
        ftruncate(shm_fd, sizeof(T));
    }
    void* mptr = mmap(nullptr, sizeof(T), PROT_READ | PROT_WRITE,
                      MAP_SHARED, shm_fd, 0);
    if (mptr == (void*)-1) {
        std::cerr << "Error mmap " << errno << " (" << strerror(errno)
                  << ")" << std::endl;
        if (create) {
            shm_unlink(typeid(T).name());
        }
        return nullptr;
    }
    if (create) {
        return new(mptr) T;
    } else {
        return (T*)mptr;
    }
}

template <typename T>
static T* shm_create(const std::string& name) {
    return initShm<T>(name, true);
}

template <typename T>
static T* shm_create() {
    return shm_create<T>(typeid(T).name());
}

template <typename T>
static void shm_destroy(T* shmptr, const std::string& name) {
    shmptr->~T();
    munmap(shmptr, sizeof(T));
    shm_unlink(name.c_str());
}

template <typename T>
static void shm_destroy(T* shmptr) {
    shm_destroy(shmptr, typeid(T).name());
}

template <typename T>
static T* shm_connect(const std::string& name) {
    return initShm<T>(name, false);
}

template <typename T>
static T* shm_connect() {
    return shm_connect<T>(typeid(T).name());
}

template <typename T>
static void shm_free(T* shmptr) {
    shmptr->~T();
    munmap(shmptr, sizeof(T));
}

#endif //TESTSHM_SHM_H
