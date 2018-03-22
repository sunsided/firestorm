//
// Created by sunside on 22.03.18.
//

#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <firestorm/synchronization/cancellation_token_source.h>

namespace {

    TEST(CancellationToken, NewToken_WhenNotCanceled_IsNotCanceled) {
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        auto ct = cts.create_token();

        // assert
        ASSERT_FALSE(ct->canceled());
    }

    TEST(CancellationToken, NewToken_WhenCanceled_IsCanceled) {
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        cts.cancel();
        auto ct = cts.create_token();

        // assert
        ASSERT_TRUE(ct->canceled());
    }

    TEST(CancellationToken, ExistingToken_WhenCanceled_IsCanceled) {
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        auto ct = cts.create_token();
        cts.cancel();

        // assert
        ASSERT_TRUE(ct->canceled());
    }

    TEST(CancellationToken, MultipleTokens_WhenCanceled_AreCanceled) {
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        auto a = cts.create_token();
        auto b = cts.create_token();
        cts.cancel();

        // assert
        ASSERT_TRUE(a->canceled());
        ASSERT_TRUE(b->canceled());
    }

    TEST(CancellationToken, TokenCallback_WhenNotCanceled_IsNotCalled) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        // assert
        ASSERT_EQ(count, 0);
    }

    TEST(CancellationToken, TokenCallback_WhenCanceled_IsCalled) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        cts.cancel();

        // assert
        ASSERT_EQ(count, 1);
    }

    TEST(CancellationToken, TokenCallback_WhenAlreadyCanceled_IsCalledImmediately) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        cts.cancel();
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        // assert
        ASSERT_EQ(count, 1);
    }

    TEST(CancellationToken, TokenCallback_WhenCanceledOften_IsCalledOnce) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        cts.cancel();
        cts.cancel();
        cts.cancel();
        cts.cancel();

        // assert
        ASSERT_EQ(count, 1);
    }

    TEST(CancellationToken, CancelAfter_CancelsToken) {
        // arrange
        firestorm::cancellation_token_source cts{std::chrono::milliseconds(10)};

        // act
        auto ct = cts.create_token();
        const auto immediate = ct->canceled();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // assert
        ASSERT_FALSE(immediate);
        ASSERT_TRUE(ct->canceled());
    }

    TEST(CancellationToken, CancelAtTime_CancelsToken) {
        // arrange
        firestorm::cancellation_token_source cts{std::chrono::steady_clock::now() + std::chrono::milliseconds(10)};

        // act
        auto ct = cts.create_token();
        const auto immediate = ct->canceled();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // assert
        ASSERT_FALSE(immediate);
        ASSERT_TRUE(ct->canceled());
    }


    // TODO: Test chained token sources / chained cancellation.
}