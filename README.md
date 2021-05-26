# Thread Pool Library

This is a Thread Pool Library for C++. The core functionality is to provide a class that manages threads and completes the callable tasks that has been submitted to it (in a concurrent manner). 

The boost version of a thread pool is currently buried within other libraries and there is limited support for non-void functions. So I have written this small library.


## Compatibility

C++ 11 and later versions are supported

## Example Usage

    int test_function(int arg1, int arg2){
    	return arg1 + arg2;
    }
    
    int main(){
    	TP::Threadpool<> TPool(8);
    	std::future\<int\> result= TPool.submit(test_function, 5, 10)
    	cout << result.get() << endl;
Prints out 15.

## Including to the project
In order to include the threadpool, include the **threadpool.h** file.

> #include  "threadpool.h"

## Usage

Everything is contained in the **TP** namespace.

### Creating a ThreadPool
To construct the ThreadPool with the default execution policy, leave the template parameters blank. The default execution policy is first in first out.

    TP::ThreadPool<>
The constructor takes a value to indicate the number of threads to be created. For example

    TP::ThreadPool<> tpool_obj(8)
 creates 8 threads to run simultaneously. 

### Getting and setting thread count

You can get the current thread count by calling **get_thread_count()** like the so:

    tpool_obj.get_thread_count();
The thread count is set by calling **set_thread_count(num_of_threads)**

    tpool_obj.set_thread_count(8);
set_thread_count returns a boolean value depending on whether it can set the threads immediately or not. The function will return false if it needs to wait for some threads to terminate first.

### Submitting a task
In order to get the ThreadPool to run a task (i.e a callable object or function) you must submit it to the ThreadPool object.

    tpool_obj.submit(Callable);
This will return a future object with the return type of the Callable object. 
 
The Callable argument can be a function, lambda or an object with a defined operator(). If Callable is an object, the ThreadPool will delete it once it is run.

Arguments can also be supplied to the Callable object.

    tpool_obj.submit(Callable, Arg1, Arg2, Arg3);
 If you need to submit a lot of tasks, you can use **submit_nf()** function for a small performance boost. submit_nf is identical to the submit() function. The only difference is that submit_nf() will not return a future object.

    tpool_obj.submit_nf(Callable); //returns void

### Copying and Moving ThreadPool Objects
Threadpool objects are **not copyable**.
However they are **moveable**.

    /*The functioning of the threads are not interrupted in the following*/
    TP::ThreadPool<> tpool_obj2(std::move(tpool_obj));
    TP::ThreadPool<> tpool_obj3 = std::move(tpool_obj2);
  
## Destruction of a ThreadPool
Once a ThreadPool object goes out of scope or otherwise gets destroyed, it will submit thread destruction commands and wait for all of the threads to join. With the default execution policy, all submitted tasks will be completed before the threads are joined.

## Creating a custom execution policy
The execution policy is responsible for the order which the threads receive tasks.

A custom execution policy can be given to a ThreadPool like so:

    TP::ThreadPool<CustomExecutionPolicy> tpool_obj(8)
The custom execution policy must have the following public member functions:

    void add_task(std::shared_ptr<TP::Utilities::TaskWrapper>);
    std::shared_ptr<TP::Utilities::TaskWrapper> get_task();
    std::size_t task_count();
Please note that access to these functions will be concurrent and the execution policy must prevent any data races. Additionally, get_task() should remove its copy of the shared_ptr of TaskWrapper to ensure that the TaskWrapper object gets destroyed once it is run.

### TaskWrapper Objects
 Every task is wrapped in a TaskWrapper object. The TaskWrapper object has two public members.

    class TaskWrapper{
	    public:
	    void run(); //Runs the task
	    WTCommand cmd;
	}    
The WTCommand object is an enum class that contains a command for the Thread that will process the task. Currently, WTCommand is defined as follows:

      enum class WTCommand{
	      run_task, //call TaskWrapper::run()
	      terminate_thread, //Terminate the thread
      };
 You can inspect the cmd member to handle *terminate_thread* commands differently in your custom execution policy.


