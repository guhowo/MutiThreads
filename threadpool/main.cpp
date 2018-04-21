#include <iostream>
#include <boost/function>

#include "ThreadPool.h"

printInt(int num)
{
	printf("%d\n",num);
}

void test(int num)
{
	ThreadPool pool;
	pool.start(2);
	for(int i-0;i<num;i++)
	    pool.run(boost::bind(printInt, i));
}

int main() 
{	
	test(10);
    return 0;
}
