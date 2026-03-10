#pragma once

#include <vector>
#include <algorithm>
#include <memory>
#include <mutex>

namespace hero::core {

template<typename EventType>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void onNotify(const EventType& event) = 0;
};

template<typename EventType>
class Observable {
public:
    void addObserver(std::shared_ptr<Observer<EventType>> observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
            observers_.push_back(observer);
        }
    }

    void removeObserver(std::shared_ptr<Observer<EventType>> observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        observers_.erase(
            std::remove(observers_.begin(), observers_.end(), observer),
            observers_.end()
        );
    }

protected:
    void notifyObservers(const EventType& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& observer : observers_) {
            if (observer) {
                observer->onNotify(event);
            }
        }
    }

private:
    std::vector<std::shared_ptr<Observer<EventType>>> observers_;
    std::mutex mutex_;
};

} // namespace hero::core
