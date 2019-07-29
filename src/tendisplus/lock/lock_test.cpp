#include <iostream>
#include <string>
#include <algorithm>
#include <thread>
#include "gtest/gtest.h"
#include "glog/logging.h"
#include "tendisplus/lock/lock.h"

namespace tendisplus {

TEST(Lock, Common) {
    bool runFlag1 = true, runFlag2 = true;
    bool locked1 = false, locked2 = false;

    auto mgr = std::make_unique<mgl::MGLockMgr>();

    std::thread thd1([&runFlag1, &locked1, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_IS, nullptr, mgr.get());
        locked1 = true;
        while (runFlag1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thd2([&runFlag2, &locked2, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_X, nullptr, mgr.get());
        locked2 = true;
        while (runFlag2) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    EXPECT_TRUE(locked1);
    EXPECT_FALSE(locked2);
    runFlag1 = false;
    thd1.join();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(locked2);
    runFlag2 = false;
    thd2.join();
}

TEST(Lock, DefaultMgr) {
    bool runFlag1 = true, runFlag2 = true;
    bool locked1 = false, locked2 = false;

    std::thread thd1([&runFlag1, &locked1]() {
        StoresLock v(mgl::LockMode::LOCK_IS, nullptr, nullptr);
        locked1 = true;
        while (runFlag1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thd2([&runFlag2, &locked2]() {
        StoresLock v(mgl::LockMode::LOCK_X, nullptr, nullptr);
        locked2 = true;
        while (runFlag2) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    EXPECT_TRUE(locked1);
    EXPECT_FALSE(locked2);
    runFlag1 = false;
    thd1.join();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(locked2);
    runFlag2 = false;
    thd2.join();
}

TEST(Lock, DiffMgr) {
    bool runFlag1 = true, runFlag2 = true, runFlag3 = true;
    bool locked1 = false, locked2 = false, locked3 = false;

    auto mgr = std::make_unique<mgl::MGLockMgr>();
    auto mgr1 = std::make_unique<mgl::MGLockMgr>();

    std::thread thd1([&runFlag1, &locked1, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_IS, nullptr, mgr.get());
        locked1 = true;
        while (runFlag1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread thd2([&runFlag2, &locked2, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_X, nullptr, mgr.get());
        locked2 = true;
        while (runFlag2) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread thd3([&runFlag3, &locked3, &mgr1]() {
        StoresLock v(mgl::LockMode::LOCK_X, nullptr, mgr1.get());
        locked3 = true;
        while (runFlag3) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_TRUE(locked1);
    EXPECT_FALSE(locked2);
    EXPECT_TRUE(locked3);  // diff mgr
    runFlag1 = false;
    thd1.join();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(locked2);
    runFlag2 = false;
    thd2.join();

    runFlag3 = false;
    thd3.join();
}

TEST(Lock, Complicated) {
    bool runFlag1 = true, runFlag2 = true;
    bool runFlag3 = true, runFlag4 = true;
    bool runFlag5 = true, runFlag6 = true;
    bool locked1 = false, locked2 = false;
    bool locked3 = false, locked4 = false;
    bool locked5 = false, locked6 = false;

    auto mgr = std::make_unique<mgl::MGLockMgr>();
    std::thread thd1([&runFlag1, &locked1, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_X, nullptr, mgr.get());
        locked1 = true;
        LOG(INFO) << "thd1 LOCK_X OK";
        while (runFlag1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thd2([&runFlag2, &locked2, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_IS, nullptr, mgr.get());
        locked2 = true;
        LOG(INFO) << "thd2 LOCK_IS OK";
        while (runFlag2) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thd3([&runFlag3, &locked3, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_IX, nullptr, mgr.get());
        locked3 = true;
        LOG(INFO) << "thd3 LOCK_IX OK";
        while (runFlag3) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thd4([&runFlag4, &locked4, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_S, nullptr, mgr.get());
        locked4 = true;
        LOG(INFO) << "thd4 LOCK_S OK";
        while (runFlag4) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thd5([&runFlag5, &locked5, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_IX, nullptr, mgr.get());
        locked5 = true;
        LOG(INFO) << "thd5 LOCK_IX OK";
        while (runFlag5) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_TRUE(locked1);
    EXPECT_FALSE(locked2);
    EXPECT_FALSE(locked3);
    EXPECT_FALSE(locked4);
    EXPECT_FALSE(locked5);

    runFlag1 = false;
    thd1.join();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(locked2);
    EXPECT_TRUE(locked3);
    EXPECT_FALSE(locked4);
    EXPECT_FALSE(locked5);

    LOG(INFO) << "thd6 LOCK_IX new, should be waiting";
    std::thread thd6([&runFlag6, &locked6, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_IX, nullptr, mgr.get());
        locked6 = true;
        LOG(INFO) << "thd6 LOCK_IX OK";
        while (runFlag6) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_TRUE(locked2);
    EXPECT_TRUE(locked3);
    EXPECT_FALSE(locked4);
    EXPECT_FALSE(locked5);
    EXPECT_FALSE(locked6);

    runFlag2 = false;
    thd2.join();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_TRUE(locked2);
    EXPECT_TRUE(locked3);
    EXPECT_FALSE(locked4);
    EXPECT_FALSE(locked5);
    EXPECT_FALSE(locked6);

    runFlag3 = false;
    thd3.join();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_TRUE(locked2);
    EXPECT_TRUE(locked3);
    EXPECT_TRUE(locked4);
    EXPECT_FALSE(locked5);
    EXPECT_FALSE(locked6);

    runFlag4 = false;
    thd4.join();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_TRUE(locked2);
    EXPECT_TRUE(locked3);
    EXPECT_TRUE(locked4);
    EXPECT_TRUE(locked5);
    EXPECT_TRUE(locked6);

    runFlag5 = false;
    thd5.join();
    runFlag6 = false;
    thd6.join();
}

TEST(Lock, KeyLock) {
    bool runFlag1 = true, runFlag2 = true;
    bool locked1 = false, locked2 = false;

    auto mgr = std::make_unique<mgl::MGLockMgr>();
    auto sess = std::make_shared<LocalSession>(nullptr);
    std::thread thd1([&runFlag1, &locked1, sess, &mgr]() {
        KeyLock v(1, "a", mgl::LockMode::LOCK_IS, sess.get(), mgr.get());
        // KeyLock v1(1, "a", mgl::LockMode::LOCK_IX, sess.get());
        locked1 = true;
        while (runFlag1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto sess1 = std::make_shared<LocalSession>(nullptr);
    std::thread thd2([&runFlag2, &locked2, sess1, &mgr]() {
        KeyLock v(1, "a", mgl::LockMode::LOCK_X, sess1.get(), mgr.get());
        locked2 = true;
        while (runFlag2) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    EXPECT_TRUE(locked1);
    EXPECT_FALSE(locked2);
    runFlag1 = false;
    thd1.join();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(locked2);
    runFlag2 = false;
    thd2.join();
}

TEST(Lock, Parent) {
    bool runFlag1 = true, runFlag2 = true;
    bool locked1 = false, locked2 = false;

    auto mgr = std::make_unique<mgl::MGLockMgr>();

    auto sess = std::make_shared<LocalSession>(nullptr);
    std::thread thd1([&runFlag1, &locked1, sess, &mgr]() {
        StoreLock v(1, mgl::LockMode::LOCK_IS, sess.get(), mgr.get());
        StoreLock v1(2, mgl::LockMode::LOCK_IS, sess.get(), mgr.get());
        locked1 = true;
        while (runFlag1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread thd2([&runFlag2, &locked2, &mgr]() {
        StoresLock v(mgl::LockMode::LOCK_X, nullptr, mgr.get());
        locked2 = true;
        while (runFlag2) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    EXPECT_TRUE(locked1);
    EXPECT_FALSE(locked2);
    runFlag1 = false;
    thd1.join();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_TRUE(locked2);
    runFlag2 = false;
    thd2.join();
}
}  // namespace tendisplus
