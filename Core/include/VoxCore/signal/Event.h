#pragma once
#include <algorithm>
#include <vector>
#include <VoxCore/Define.h>

#include "Delegate.h"

VOX_NS
    template<typename... Args>
    class Event {
    public:
        template<auto Method, typename T>
        void subscribe(T *listener) {
            mDelegates.push_back(Delegate<Args...>::template from<Method>(listener));
        }

        template<typename F>
        void operator+=(F &&lambda) {
            subscribe(lambda);
        }

        template<auto Method, typename T>
        void operator+=(T *listener) {
            subscribe<Method>(listener);
        }

        template<typename F>
        void subscribe(F &&lambda) {
            mDelegates.push_back(Delegate<Args...>(std::forward<F>(lambda)));
        }

        template<auto Method, typename T>
        void unsubscribe(T *listener) {
            auto target = Delegate<Args...>::template from<Method>(listener);
            mDelegates.erase(
                std::remove(mDelegates.begin(), mDelegates.end(), target),
                mDelegates.end()
            );
        }

        void invoke(Args... args) const {
            for (const auto &d: mDelegates) {
                d.invoke(args...);
            }
        }

    private:
        std::vector<Delegate<Args...> > mDelegates;
    };

NS_END
