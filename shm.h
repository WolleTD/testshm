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
#include <sstream>
#include <exception>
#include <utility>

template <typename T>
class ShmObject {
    std::string _name;
    T* _object;
    bool _owner{};

public:
    enum shm_flag {
        create,
        attach_ro,
        attach_rw,
    };

    ShmObject(std::string name, const shm_flag flag) : _name(std::move(name)), _owner(flag == create) {
        int flags = O_RDONLY;
        if (_owner) {
            flags = O_CREAT | O_EXCL | O_RDWR;
        } else if (flag == attach_rw) {
            flags = O_RDWR;
        }
        int shm_fd = shm_open(_name.c_str(), flags, 0644);
        if (shm_fd == -1) {
            std::stringstream ss;
            ss << "Error shm_open " << errno << " (" << strerror(errno) << ")";
            throw std::runtime_error(ss.str());
        }
        if (_owner) {
            ftruncate(shm_fd, sizeof(T));
        }
        int prot = PROT_READ;
        if (flag != attach_ro) {
            prot |= PROT_WRITE;
        }
        void* mptr = mmap(nullptr, sizeof(T), prot, MAP_SHARED, shm_fd, 0);
        if (mptr == (void*)-1) {
            std::stringstream ss;
            ss << "Error mmap " << errno << " (" << strerror(errno) << ")" << std::endl;
            if (_owner) {
                shm_unlink(_name.c_str());
            }
            throw std::runtime_error(ss.str());
        }
        if (_owner) {
            _object = new(mptr) T;
        } else {
            _object = (T*)mptr;
        }
    }

    explicit ShmObject(const std::string& name) : ShmObject(name, create) {}

    ~ShmObject() {
        _object->~T();
        munmap(_object, sizeof(T));
        if (_owner) {
            shm_unlink(_name.c_str());
        }
    }

    T& operator*() { return *_object; }
    T* operator->() { return _object; }
};

#endif //TESTSHM_SHM_H
