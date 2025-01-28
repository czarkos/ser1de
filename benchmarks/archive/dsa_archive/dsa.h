#ifndef _DSA_1_H_
#define _DSA_1_H_

#include <accfg/idxd.h>
#include <accfg/libaccel_config.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <unistd.h>

#include <cassert>
#include <iostream>

class Dsa {
 public:
  enum Path { Shared, Dedicated };

  Dsa(Path path) : path_(path) {}
  ~Dsa() { DeInit(); }

  int Init() {
    if (accfg_new(&ctx_)) return -1;

    if (SetupWq()) return -1;

    assert(wq_ != nullptr);
    assert(wq_reg_ != nullptr);
    assert(fd_ > 0);

    return 0;
  }

  void* GetWqRegister() const { return wq_reg_; }

  Path GetPath() const { return path_; }

  size_t GetBatchSize() const { return accfg_wq_get_max_batch_size(wq_); }

  bool IsReady() const { return wq_reg_ != nullptr; }

  static inline void movdir64b(volatile void* portal, void* desc) {
    asm volatile(
        "sfence\t\n"
        ".byte 0x66, 0x0f, 0x38, 0xf8, 0x02\t\n"
        :
        : "a"(portal), "d"(desc));
  }

  static inline unsigned char enqcmd(volatile void* portal, void* desc) {
    unsigned char retry;
    asm volatile(
        "sfence\t\n"
        ".byte 0xf2, 0x0f, 0x38, 0xf8, 0x02\t\n"
        "setz %0\t\n"
        : "=r"(retry)
        : "a"(portal), "d"(desc));
    return retry;
  }

 private:
  int SetupWq() {
    accfg_device* device;
    accfg_wq* wq;

    accfg_device_foreach(ctx_, device) {
      // Make sure that the device is enabled
      if (accfg_device_get_state(device) != ACCFG_DEVICE_ENABLED) continue;

      accfg_wq_foreach(device, wq) {
        // Make sure iaa_test will not use dsa wq, vice versa
        if (accfg_device_get_type(device) != ACCFG_DEVICE_DSA) continue;

        // Get a workqueue that's enabled
        if (accfg_wq_get_state(wq) != ACCFG_WQ_ENABLED) continue;

        // The wq type should be user
        if (accfg_wq_get_type(wq) != ACCFG_WQT_USER) continue;

        // Make sure the mode is correct
        accfg_wq_mode mode = accfg_wq_get_mode(wq);
        if ((mode == ACCFG_WQ_SHARED && path_ == Dedicated) ||
            (mode == ACCFG_WQ_DEDICATED && path_ == Shared))
          continue;

        if (SetupWqRegister(wq)) return -1;

        wq_ = wq;
        return 0;
      }
    }

    return -1;
  }

  int SetupWqRegister(accfg_wq* wq) {
    char path[PATH_MAX];

    if (accfg_wq_get_user_dev_path(wq, path, PATH_MAX)) {
      std::cerr << "Error getting uacce device path" << std::endl;
      return -1;
    }

    fd_ = open(path, O_RDWR);
    if (fd_ < 0) {
      std::cerr << "Error opening WQ file" << std::endl;
      return -1;
    }

    wq_reg_ =
        mmap(NULL, PAGE_SIZE, PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd_, 0);
    if (wq_reg_ == MAP_FAILED) {
      std::cerr << "Error mapping WQ file" << std::endl;
      close(fd_);
      return -1;
    }

    return 0;
  }

  void DeInit() {
    if (wq_reg_) munmap(wq_reg_, PAGE_SIZE);
    if (fd_ > 0) close(fd_);
    accfg_unref(ctx_);
  }

  //
  Path path_;

  //
  accfg_ctx* ctx_;
  accfg_wq* wq_;
  void* wq_reg_;
  int fd_;
};

#endif