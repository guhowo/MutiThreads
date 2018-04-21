#ifndef THREADPOOL_THREAD_H
#define THREADPOOL_THREAD_H

#include <pthread.h>
#include <stdexcept>
#include <boost/function.hpp>

#define THREAD_MIN_STACK_SIZE 1024*10

    typedef boost::function<void ()> ThreadFunc;

    void* proxyThreadFunction( void * agrv)
	    {
		            boost::function< void*(void *)>* threadFunction = reinterpret_cast<boost::function <void*(void *)> *>(agrv);

			            (*threadFunction)(NULL);
				            return NULL;
					        
class Thread {
public:
    Thread(ThreadFunc &func) : _started(false), _tid(0), _func(func)
    {
        pthread_attr_init(&_tattr);
        pthread_attr_setstacksize(&_tattr, THREAD_MIN_STACK_SIZE);
    }

    Thread(const Thread &t) : _started(t._started), _tid(t._tid), _func(t._func) {}

    ~Thread()
    {
        pthread_attr_destroy(&_tattr);
    }

    Thread &operator=(const Thread &t)
    {
        _started = t._started;
        _tid = t._tid;
        _func = t._func;

        return *this;
    }

    inline void start(void *instance)      //why static inline is necessary
    {
        _started = true;
        if (pthread_create(&_tid, &_tattr, proxyThreadFunction, instance)) {
            _started = false;
            throw std::runtime_error("create pthread failed");
        }
    }

    inline void join()
    {
        if (_started)
            pthread_join(_tid, (void **)0);
    }

    inline operator bool() { return _started; }

private:
    bool _started;
    pthread_t _tid;
    pthread_attr_t _tattr;
    ThreadFunc _func;
};


#endif //THREADPOOL_THREAD_H
