A high-throughput co-operative multitasking job framework using c++ 20;

Current features:
Classic single queue multi consumer job system
Spinlock utils for spinlocks and exponential back off
Thread cpu affinities

Upcoming work:
Co-routines or fibers for being able to pause and continue execution mid-job
Jobs can spawn and wait on newly spawned jobs completion
co-routine customization, custom allocators
Work stealing queues


